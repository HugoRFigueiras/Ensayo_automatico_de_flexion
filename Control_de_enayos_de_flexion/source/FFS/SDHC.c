/*
 * SDHC.c
 *
 *  Created on: 11 abr. 2022
 *      Author: hugoj
 */
#include "SDHC.h"
#include "LPTMR0.h"
#include "SPI.h"


#define IDLE_MOSI 0xFF  //Byte que no cambia el estado ocioso de mosi
uint8_t sdType;
static volatile uint8_t Stat = STA_NOINIT;

/******************************************************************************************************************
 * Nombre: SDHC_setCs
 * Definicion: controla el estado del pin CS (chip select) del modulo SPI.
 * Parametros: select, selector de estado, sus valores validos son 1 y 0 u se recomienda usar las mascaras del
 * enum u8cs_state.
 * */
void SDHC_setCs(uint8_t select)
{
	void SPIO_csCtrl(uint8_t select);
}
/*****************************************************************************************************
 * Nombre: SDHC_writeCommand
 *Definicion: Utiliza la funcion SPI0_writeByte para enviar consecutivamente los 5 bytes
 *correspondientes a un comando SD-SPI, posteriormente envia una serie de pulsos de reloj
 *para permitirle al dispositivo responder con al comando.
 *Parametros: cmdn; numero de comando, se espera que se introduzca en formato decimal. ej CMD55 = 55.
 *            arg; Los 4 bytes correspondientes al argumento, en una unica expresion hexadecimal.
 *            crc; codigo crc, solo necesario para cmd0 (0x95) y cmd8 (0x87).
 *Return: tempRes;  la variable donde se almacena la respuesta del dispositivo SD al comando.
 * */
uint8_t SDHC_writeCommand(uint8_t cmdn,uint32_t arg,uint8_t crc)
{
	uint8_t  tempRes = 0xFF;
	uint16_t cont = 512;
	if(cmdn > 63)
		return '!'; //comando invalido
	SPIO_csCtrl(csS_High);
	SPI0_writeByte(IDLE_MOSI);
	SPIO_csCtrl(csS_Low);             //assert SDHC CS
	SPI0_writeByte(0x40 | cmdn); //sends command number
	SPI0_writeByte((uint8_t)(arg  >> 24));
	SPI0_writeByte((uint8_t)(arg  >> 16));
	SPI0_writeByte((uint8_t)(arg  >> 8));
	SPI0_writeByte((uint8_t)(arg));
	SPI0_writeByte(crc);
	do
	{
		tempRes = SPI0_writeByte(IDLE_MOSI);
		cont--;
	}while((cont != 0) && (tempRes == 0xFF));
	return tempRes;
}
/*****************************************************************************************
 * Nombre: SDHC_waitReady
 * Descripcion: Mantiene la linea MOSI en alto hasta recibir una respuesta distinta a 0x00
 * la cual es la respuesta IDLE de la tarjeta SDHC.
 * Return: RES_OK = Ready, RES_NOTRDY = timeout superado.
 * */
uint8_t SDHC_waitReady(void)
{
	uint16_t  u8temp = 0;
    while((SPI0_writeByte(IDLE_MOSI) == 0x00) && (u8temp < 0xFFFE))
    	u8temp++;
    if(u8temp >= 0xFFFE)
    {
    	return RES_NOTRDY;
    }
    return RES_OK;
}
/****************************************************************************************
 * Nombre: SDHC_initialize
 * Definicion: Ejecuta la rutina de inicializacion de una tarjeta tipo SDHC en modo SPI.
 * Parametros: Ninguno.
 * Return: DRESULT
 * */
uint8_t SDHC_initialize()
{
	uint16_t i,cont2 = 0;
	uint8_t u8bufResp[5],u8resp = 0x01;
	SPI0_initialize();
	LPTMR0_Init();
	while(timeout() != 0x01);
	/****   Inicia Secuencia, envia mas de 74 pulsos de reloj a la tarjeta*/
	for(i = 0; i < 0x0F; i++)
		SPI0_writeByte(IDLE_MOSI);
	//***   Envia el comando de reset hasta que responda con R1
	//while((contCmd++ < 255) && (u8resp != 0x01))
	u8resp = SDHC_writeCommand(0,0x00000000,0x95);
	u8resp = SDHC_writeCommand(0,0x00000000,0x95);
	if(u8resp != 0x01)
	{
		SDHC_setCs(csS_High);
		Stat = STA_NODISK;
		return Stat; //Reset Fail
	}
	//***   Envia Cmd8
	u8resp = SDHC_writeCommand(8,0x000001AA,0x87);
	if(u8resp == 0x01)
	{
		while(cont2 < 5)
			u8bufResp[cont2++] = SPI0_writeByte(IDLE_MOSI);
	}
	else
	{
		SDHC_setCs(csS_High);
		Stat = STA_NOINIT;
		return Stat;
	}
	cont2 = 0xFFFF;
	while((cont2 > 0 ) && (u8resp !=  0x00))//***   Envia Cmd41 hasta que regrese  R1 = 0x00
	{
		SDHC_writeCommand(55, 0, 0xff );
		u8resp = SDHC_writeCommand(41,0x40000000,0xff);
		cont2--;
	}
	u8resp = SDHC_writeCommand(58,0x0,0x0); //***   Leer OCR para obener la version de la tarjeta
	if (u8resp != 0x00 )
	{
		SPIO_csCtrl(csS_High);
		return STA_NOINIT;//
	}
	for ( i = 0; i <= 4; i++ )
		u8bufResp[i] = SPI0_writeByte(IDLE_MOSI);
	if(u8bufResp[0] & 0x40)
	{
		sdType = 1;
		Stat = RES_OK;
	}
	else
	{
		SPIO_csCtrl(csS_High);
		Stat = STA_NODISK;
		return Stat;
	}
	SPIO_csCtrl(csS_High);
	return RES_OK;
}
/************************************************************************************************
 * Regresa el status actual del disco
 * */
uint8_t SDHC_diskStat(void)
{
	return Stat;
}
/************************************************************************************************
 *Descripcion: Envia los bytes a escribir en un bloque de memoria, y espera a que la tarjta
 *Termine de programarlos
 *Parametros *buffer; es el buffer donde estan los datos a escribir
 *           Token; token de ninicio de transbicion 0xFE para un unico bloque y 0xFC para
 *           multibloque
 * */
uint8_t SDHC_writeBlock(const uint8_t *buffer,uint8_t token)
{
	uint16_t i;
	uint8_t u8Resp;
	SDHC_waitReady();
	SPI0_writeByte(token);               //Envia el token
	for(i = 0; i < 512; i++)             //Escribe los bytes
		SPI0_writeByte(*buffer++);
	SPI0_writeByte(IDLE_MOSI); SPI0_writeByte(IDLE_MOSI);  //Dummy crc

	u8Resp = SPI0_writeByte(IDLE_MOSI);       //obtener el token de respuesta.
    u8Resp &= 0x1F;
    if(u8Resp != 0x05)
    {
    	SPIO_csCtrl(csS_High);
        return 1;
    }
    SDHC_waitReady();
    return RES_OK;
}
/****************************************************************************
 *Nombre:  SDHC_dataWrite
 *Descripcion: Envia comandos para recibir bloques de memoria a partir de la
 *direccion determinada por sectorAdr (esta direccion se basa en setores 1 cada
 *512 bytes).
 *Parametros: buffe; direccion del buffer del que se tomaran los datos a escribir
 *            sectorAdr; direccion del sector de la tarjeta donde se escribiran los
 *            datos
 *            count; numero de sectores a escribir.
 *Return: DRESULT
 * */
uint8_t SDHC_dataWrite(const uint8_t *buffer, uint32_t sectorAdr, uint8_t count)
{
	uint8_t u8Resp;
	if(sdType != 1)
		sectorAdr = sectorAdr << 9;
	if(count == 1)
	{
		//escribe un solo bloque
		u8Resp = SDHC_writeCommand(24,sectorAdr,0x01);
		if(u8Resp != 0x00)
			return RES_ERROR;
		SPIO_csCtrl(csS_Low);
		if(SDHC_writeBlock(buffer,0xFE) != 0)
			return RES_ERROR;
        SPI0_writeByte(IDLE_MOSI);
        SPIO_csCtrl(csS_High);
	}
	else if(count > 1)
	{
		SDHC_writeCommand(55, 0, 0xff);
		SDHC_writeCommand(23, count, 0x00 ); //Sugerido para obtimizar escritura.
		u8Resp = SDHC_writeCommand(25,sectorAdr,0xFF);
		if(u8Resp != 0x00)
			return RES_ERROR;
		SPIO_csCtrl(csS_Low);
		do
		{
			SDHC_writeBlock(buffer,0xFC);
			buffer += 512;
			count--;
		}while(count != 0x00);
		SPI0_writeByte(0xFD);     //STOP TOKEN 11111101
		SDHC_waitReady();
        SPIO_csCtrl(csS_High);
	}
	else
		return RES_PARERR;//valor invalido de bloques.
	SDHC_waitReady();
	return RES_OK;
}
/***************************************************************************************************
 * Nombre: SDHC_dataRead
 * Descripcion: Se encarga de leer uno o varios bloques de memoria del dispositivo SD, el tamaño
 * de cada bloque es de 512 bytes.
 * Parametros: *buffer; puntero al buffer en el que se almacenaran los datos leidos.
 *             sectorAdr;  del sector a partir del cual se leeran los datos
 *             de la tarjeta.
 *             count; cantidad de bloques a leer.
 *Return: DRESULT
 * */
uint8_t SDHC_dataRead(uint8_t *buffer, uint32_t sectorAdr, uint8_t count )
{
	uint8_t  u8Resp,u8timeOut = 0;
	uint16_t i;
	if(sdType != 1)
		sectorAdr *= 512;
	if(count == 1)
	{
		SPIO_csCtrl(csS_Low);
		u8Resp = SDHC_writeCommand(17,sectorAdr,0x01);   //Envia el comando de lectura hasta recibir un r1 valido.
		if(u8Resp != 0)
			return RES_ERROR;
		do  //Espera a que la tarjeta salga de estado busy y proporcione el token de inicio de datos
		{
			u8Resp = SPI0_writeByte(IDLE_MOSI);// Espera hasta que el dispositivo responda con un statrt token de lectura.
		}while((u8Resp == 0xFF) && (u8timeOut++ < 254));
		if(u8Resp != 0xFE)
			return RES_ERROR;
		for(i = 0; i < 512; i++)                 //Lee los bytes
			buffer[i] = SPI0_writeByte(IDLE_MOSI);
		SPI0_writeByte(IDLE_MOSI);  //Los bytes de crc no importan
		SPI0_writeByte(IDLE_MOSI);
	}
	else if(count > 1)
	{
		u8Resp = SDHC_writeCommand(18,sectorAdr,0x01);
		if(u8Resp != 0x00)
			return RES_ERROR;
		SPIO_csCtrl(csS_Low);
		do
		{
			do  //Espera a que la tarjeta salga de estado busy y proporcione el token de inicio de datos
			{
				u8Resp = SPI0_writeByte(IDLE_MOSI);
			}while((u8Resp == 0xFF) && (u8timeOut++ < 254));
			if(u8Resp != 0xFE) //First byte start bloc token.(p281)
				return RES_ERROR;
			while (SPI0_writeByte(IDLE_MOSI) != 0xFE);
			for(i = 0; i < 512; i++)                 //Lee los bytes
			{
				*buffer++ = SPI0_writeByte(IDLE_MOSI);
			}
			SPI0_writeByte(IDLE_MOSI);  //Los bytes de crc no importan
			SPI0_writeByte(IDLE_MOSI);
			u8timeOut = 0;
			count--;
		}while(count != 0);
		SPIO_csCtrl(csS_High);
		u8Resp = SDHC_writeCommand(12,0x00,0x01); //La respuesta a este comando es R1b(posiblemente quieras recogerla)
		SPI0_writeByte(IDLE_MOSI);
	}
	else
	{
		return RES_PARERR; // Error de parametros
	}
	SPIO_csCtrl(csS_High);
	return RES_OK;
}
/*********************************************************************************************+
 * Nombre: SDHC_readRegisterCSD
 * Descripcion: Lee el registro CSD (ver 2.0) de una tarjeta SDHC. Guarda los valores
 * del registro en el buffer proporcionado.
 * Parametros: *u8buff; punero al buffer en el que se almacenara el valor del registro,
 * este buffer debe tener una longitud de 16 bytes como minimo.
 * Return: DRESULT
 * */
uint8_t SDHC_readRegisterCSD(uint8_t *u8buff)
{
	uint8_t u8resp, i = 0;
	uint8_t u8timeOut = 0;
	u8resp = SDHC_writeCommand(9,0x00000000,0x95);
	if(u8resp != 0x00)
	{
		return RES_ERROR;
	}

	do  //Espera a que la tarjeta salga de estado busy y proporcione el token de inicio de datos
	{
		u8resp = SPI0_writeByte(IDLE_MOSI);
	}while((u8resp == 0xFF) && (u8timeOut++ < 254));
	if(u8resp != 0xFE)
		return RES_ERROR;

	while(i < 16)
	{
		u8buff[i++] = SPI0_writeByte(IDLE_MOSI);
	}
	u8resp = SPI0_writeByte(IDLE_MOSI);   //Leer los 2 bytes de CRC.
	u8resp = SPI0_writeByte(IDLE_MOSI);
	SPI0_writeByte(IDLE_MOSI);
	SPI0_writeByte(IDLE_MOSI);
	return RES_OK;
}
/*********************************************************************************************
 *Nombre: SDHC_readRegisterSDStatus
 *Descripcion: Lee el registo SD status y lo almacena en en el buffer de 64 bits proporcionado
 *Patametros: u8buff; puntero de 8 bits al buffer de 64 elementos  en el que se almacenara el
 *contenido del registro.
 *Return: DRESULT
 * */
uint8_t SDHC_readRegisterSDStatus(uint8_t *u8buff)
{
	uint8_t u8resp, i = 0;
	uint8_t u8timeOut = 0;
	SDHC_writeCommand(55, 0, 0xff );
	u8resp = SDHC_writeCommand(13, 0, 0xFF);
	if(u8resp != 0x00)
	{
		return RES_ERROR;
	}
	if(SPI0_writeByte(IDLE_MOSI) != 0x00) //Lee la segunda parte de la respuesta R2
	{
		return RES_ERROR;
	}
	do  //Espera a que la tarjeta salga de estado busy y proporcione el token de inicio de datos
	{
		u8resp = SPI0_writeByte(IDLE_MOSI);
	}while((u8resp == 0xFF) && (u8timeOut++ < 254));
	if(u8resp != 0xFE)
		return RES_ERROR;
	while(i < 64)
	{
		u8buff[i++] = SPI0_writeByte(IDLE_MOSI);
	}
	u8resp = SPI0_writeByte(IDLE_MOSI);   //Leer los 2 bytes de CRC.
	u8resp = SPI0_writeByte(IDLE_MOSI);
	return RES_OK;
}
/***************************************************************************
 *Nombre: SDHC_sectorCount
 *Descripcion: Regresa el conteo de sectores disponibles de la tatjeta. para
 *esto se extrae el parametro c_size y se multiplica por 512.
 *Parametros: *c_size; Puntero a la variable de 32 bits donde se almacenera
 *el valor asociado a la cantidad de sectores.
 *Return: 0 = la funcion concluyo con exito, 1 = error al leer el registro csd.
 * */
uint8_t SDHC_sectorCount(uint32_t *c_size)
{
	uint8_t u8buff[16];
	uint8_t u8resp;
	uint32_t temp;
	u8resp = SDHC_readRegisterCSD(u8buff);
	if(u8resp != 0)
		return 1;
	*c_size = (uint32_t)(((uint32_t)u8buff[8] << 8) + (u8buff[9] + 1));
	temp = *c_size;
	temp = temp << 10;
	*c_size = temp;
	return RES_OK;
}
/*********************************************************************************
 *Nombre: SDHC_readBlockSize.
 *Descripcion: Lee el registro SD Status y extrae el valor del sector AU_SIZE
 *Parametros: au_size; Puntero a la variable de 32 bits (DWORD) que contendra
 *            el valor de au_size. Recordar que el valor se regresa en forma
 *            de la potencia de 2 (2^n) que mas se acerca al valor indicado en
 *            la tabla 4.10.2.4. dividido entre 512 (bytes por sector).
 *Return: DRESULT
 **/
uint8_t SDHC_readBlockSize(uint32_t *u32AUsize)
{
	uint8_t u8resp, u8buff[64];
	static const uint32_t au_size[] = {1, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 24576, 32768, 49152, 65536, 131072};
	u8resp = SDHC_readRegisterSDStatus(u8buff);
	if(u8resp != 0x00)
		return RES_ERROR;
	*u32AUsize = au_size[u8buff[10] >> 4];
	return RES_OK;
}
/*********************************************************************************
 * Nombre: eraseSectorBlock
 * Descripcion: Borra un conjunto de bloques acotado entre 2 direcciones.
 * Paramtros: AddrBuf; buffer de 32 bits que contiene en su primer elemento
 *            a la direccion inicial del bloque de sectores a borrar y en su
 *            segundo elemento a la direccion final.
 *Return: DRESULT
 * */
uint8_t SDHC_eraseSectorBlock(uint32_t *AddrBuff)
{
	uint8_t u8resp;
    uint32_t startAdr,endAdr;
    startAdr = AddrBuff[0];
    endAdr = AddrBuff[1];
	if(sdType != 1)
	{
		startAdr *= 512; endAdr *= 512;
	}
	u8resp = SDHC_writeCommand(32,startAdr,0x01);
	if(u8resp != 0x00)
		return 1;
	SPI0_writeByte(IDLE_MOSI);
	u8resp = SDHC_writeCommand(33,endAdr,0x01);
	if(u8resp != 0x00)
		return RES_ERROR;
	SPI0_writeByte(IDLE_MOSI);
	u8resp = SDHC_writeCommand(38,0x00,0x01);
	if(u8resp != 0x00)
		return RES_ERROR;
	if(SPI0_writeByte(IDLE_MOSI) != 0x00)
		return 2;
	SPI0_writeByte(IDLE_MOSI);
	return RES_OK;
}

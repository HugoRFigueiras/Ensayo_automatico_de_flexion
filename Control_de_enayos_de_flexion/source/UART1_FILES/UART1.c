/*
 * UART1.c
 *
 *  Created on: 20 nov. 2021
 *      Author: hugoj
 */


#include "UART1.h"
#include <stdio.h>
#include <string.h>

sUart1TxRx sTxRxStrucU1;
uUART1_Banderas uBanderasUart1;

void UART1_Configuracion(void)
{
	SIM->SCGC4 |= SIM_SCGC4_UART1(1);   //Habilita Clock a UART1
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[1] |= PORT_PCR_MUX(3);  //Puerto E pin 1 Rx
	PORTE->PCR[0] |= PORT_PCR_MUX(3);  //Puerto E pin 0 Tx
    //Deshabilita Rx y Tx para configurar BDR
	UART1->C2  = UART_C2_RIE(0) & UART_C2_TE(0)
	&  UART_C2_RE(0);                  //Deshabilita interrupciones
	UART1->C1      = 0;

	//                 ((UarttModuleClock)/2)
	//UART baud rate = ----------------------
	//                        ((16)xBR).
	//UarttModuleClock = Core Clock =  20.97152MHz.
    UART1->BDH = 0x00;
    UART1->BDL = 0x44;
    uBanderasUart1.u8Todas = 0x00;
	sTxRxStrucU1.u8BfrTxIndex1 = 0;
	sTxRxStrucU1.u8BfrTxIndex2 = 0;
	sTxRxStrucU1.u8BfrRxIndex1 = 0;
	sTxRxStrucU1.u8BfrRxIndex2 = 0;

	UART1->C2 |= UART_C2_RIE(1)	| UART_C2_TE(1) | UART_C2_RE(1);
	NVIC_EnableIRQ(UART1_IRQn);      //Habilita interrupciones
}

/********
 *Nombre:      UART1_Transmitir_peso
 *Descripcion: Transmite un byte mediante el Tx de UART0
 *Parametros:  byteSaliente
 *Returns:     Ninguno
 *******/
void UART1_TransmitirByte(uint8_t* byteSaliente)
{
	//Espera a que se libere el pin Tx
	while((UART1->S1 & UART_S1_TDRE_MASK) == 0);
    UART1->D = *byteSaliente;
	if(sTxRxStrucU1.u8BfrTxIndex1 > u8TxSize)
		sTxRxStrucU1.u8BfrTxIndex1 = 0;
}

/**********
 * Nombre:      UART1_TransmiteCadena
 * Descripcion: Gestiona la transmicion de los bytes contenidos en el
 *              buffer "BfrTx"
 * Parametros:
 * Rerurn:
 *********/
void UART1_TransmiteCadena(void)
{
	while((sTxRxStrucU1.u8BfrTxIndex1 <= (sTxRxStrucU1.u8BfrTxIndex2)))
	{
		UART1_TransmitirByte(& sTxRxStrucU1.u8BfrTx[sTxRxStrucU1.u8BfrTxIndex1]);
		sTxRxStrucU1.u8BfrTxIndex1++;
	}
    //UART1_TransmitirByte(& sTxRxStrucU1.u8BfrTx[sTxRxStrucU1.u8BfrTxIndex1]);
	uBanderasUart1.bitBandera.DetenerMedicion = 1;
	sTxRxStrucU1.u8BfrTxIndex2 = 0;
	sTxRxStrucU1.u8BfrTxIndex1 = 0;
}

void UART1_TransmiteTest(uint8_t Dato)
{
	//Espera a que se libere el pin Tx
	 while((UART1->S1 & UART_S1_TDRE_MASK) == 0);
	 UART1->D = Dato;
}


/*********
 * Nombre:      UART1_LlenarBfrTx
 * Descripcion: Recibe un puntero a un array y lo almacena en "u8BfrTxIndex1"
 * Parametros:
 * Return:
 ********/
void UART1_LlenarBfrTx(float fPesoSimulado)
{
	int  u8ParEntera;
	uint16_t u16Multiplos = 10;
	double   tempDecimal = fPesoSimulado;
	uint8_t  u8TempStr[10];
	uint8_t  TempIndex = 0,TempIndexInt = 0,TempLen;

	u8ParEntera  = (int)(fPesoSimulado);
	sprintf(u8TempStr,"%d.",u8ParEntera);
	TempIndexInt = strlen(u8TempStr);
	TempLen = TempIndexInt + 5;


	while(TempIndex <= TempLen)
	{
		if(TempIndex < TempIndexInt)
		{
			sTxRxStrucU1.u8BfrTx[sTxRxStrucU1.u8BfrTxIndex2] = u8TempStr[TempIndex];
		}
		else
		{
			tempDecimal = (tempDecimal - u8ParEntera) * u16Multiplos;
			u8ParEntera = (int)(tempDecimal);
			sTxRxStrucU1.u8BfrTx[sTxRxStrucU1.u8BfrTxIndex2] = ((char)(u8ParEntera ) + '0');
		}
		TempIndex++;
		sTxRxStrucU1.u8BfrTxIndex2++;
		if(sTxRxStrucU1.u8BfrTxIndex2 >= u8TxSize)
			sTxRxStrucU1.u8BfrTxIndex2 = 0;
	}
	sTxRxStrucU1.u8BfrTx[sTxRxStrucU1.u8BfrTxIndex2] = '\n';
}


void UART1_IRQHandler(void)
{
	uint8_t DatoTemp = 0;
	//Consulta la badera RDRF(Paso 1 pra borrar bandera RDRF)
	if(UART1->S1 & UART_S1_RDRF_MASK)
	{
		DatoTemp = UART1->D;
		if(DatoTemp == 0x02)
			uBanderasUart1.bitBandera.DatoRecibido = 1;
		else if(DatoTemp == 0x01)
			uBanderasUart1.bitBandera.IniciarPrueba = 1;
		else if(DatoTemp == 0x03)
		{
			uBanderasUart1.bitBandera.DetenerMedicion = 1;
			uBanderasUart1.bitBandera.CancelarPrueba = 1;
		}
		else
		{
			sTxRxStrucU1.u8BfrRx[sTxRxStrucU1.u8BfrRxIndex2++] = DatoTemp;
			if(sTxRxStrucU1.u8BfrRxIndex2 >= (u8RxSize-1))
				sTxRxStrucU1.u8BfrRxIndex2 = 0;
		}
	}
}

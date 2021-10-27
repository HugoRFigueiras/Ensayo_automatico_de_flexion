/*
 * UART0.c
 *
 *  Created on: 19 oct. 2021
 *      Author: hugoj
 */

#include "UART0.h"
#include <stdio.h>
#include <string.h>

sUart0TxRx sTxRxStruc;
uUART0_Banderas uBanderasUart0;

/*****
 * Nombre: UART0_Configuracion
 * Funcon: Establecer los valores iniciales del modulo UART0
 * Parametros: Ninguno
 * Return: void
 ****/
void UART0_Configuracion(void)
{
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); //Selecciona MCGFLLCLK (20.97152MHz)como fuente de reloj

	SIM->SCGC4 |= SIM_SCGC4_UART0(1);   //Habilita Clock a UART0
//	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
//
//	PORTA->PCR[1] |= PORT_PCR_MUX(2);  //Puerto A pin 1 Rx
//	PORTA->PCR[2] |= PORT_PCR_MUX(2);  //Puerto A pin 2 Tx
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[21] |= PORT_PCR_MUX(4);  //Puerto A pin 1 Rx
	PORTE->PCR[20] |= PORT_PCR_MUX(4);  //Puerto A pin 2 Tx
	   //Dehabilita las interuupciones


	UART0->C2  = UART_C2_RIE(0) & UART_C2_TE(0) //Deshabilita Rx y Tx para configurar BDR
	&  UART_C2_RE(0);                           //Deshabilita interrupciones


	UART0->C3      = 0;
	UART0->C1      = 0;
		//                    UarttModuleClock   UarttModuleClock = Core Clock =  20.97152MHz.
		//UART baud rate = ----------------------
		//                   ((OSR+1)xBR).
		UART0->C4  = UART0_C4_OSR (7);
		UART0->BDH = UART0_BDH_SBR(0x01);
		UART0->BDL = UART0_BDL_SBR(0x11);
		uBanderasUart0.u8Todas = 0x00;
		sTxRxStruc.u8BfrTxIndex1 = 0;
		sTxRxStruc.u8BfrTxIndex2 = 0;
		sTxRxStruc.u8BfrRxIndex1 = 0;
		sTxRxStruc.u8BfrRxIndex2 = 0;


		UART0->C2 |= UART_C2_RIE(1)	| UART_C2_TE(1) | UART_C2_RE(1);

		UART0->C5 |= UART0_C5_RDMAE(1);
		NVIC_EnableIRQ(UART0_IRQn);      //Habilita interrupciones
}

/********
 *Nombre:      UART0_Transmitir_peso
 *Descripcion: Transmite un byte mediante el Tx de UART0
 *Parametros:  byteSaliente
 *Returns:     Ninguno
 *******/
void UART0_TransmitirByte(uint8_t* byteSaliente)
{
    UART0->D = *byteSaliente;
	if(sTxRxStruc.u8BfrTxIndex1 > u8TxSize)
		sTxRxStruc.u8BfrTxIndex1 = 0;
}

/**********
 * Nombre:      UART0_TransmiteCadena
 * Descripcion: Gestiona la transmicion de los bytes contenidos en el buffer "BfrTx"
 * Parametros:
 * Rerurn:
 *********/
void UART0_TransmiteCadena(void)
{
	while((sTxRxStruc.u8BfrTxIndex1 != sTxRxStruc.u8BfrTxIndex2))
	{
		UART0_TransmitirByte(& sTxRxStruc.u8BfrTx[sTxRxStruc.u8BfrTxIndex1]);
		sTxRxStruc.u8BfrTxIndex1++;
	}

}

void UART0_TransmiteTest(uint8_t Dato)
{
	 UART0->D = Dato;

}


/*********
 * Nombre:      UART0_LlenarBfrTx
 * Descripcion: Recibe un puntero a un array y lo almacena en "u8BfrTxIndex1"
 * Parametros:
 * Return:
 ********/
void UART0_LlenarBfrTx(float fPesoSimulado)
{
	uint8_t  u8ParEntera, u8Cont = 0;
//	uint16_t u16Multiplos = 10;
//	double   tempDecimal;
	uint8_t  u8TempStr[10];
	uint8_t  TempIndex = 0,TempIndex2 = 0,TempLen =4;

	u8ParEntera  = (uint8_t)(fPesoSimulado);
	sprintf(u8TempStr,"%c.",u8ParEntera);
	//TempLen = strlen(u8TempStr) + 1;
	/*
	tempDecimal = u8ParEntera - fPesoSimulado;

	u8TempStr[TempLen] = '.';
	while(u8Cont < 4)
	{
		u8TempStr[TempLen] = (tempDecimal * u16Multiplos) + '0';
		tempDecimal -= tempDecimal;
		TempLen++;
		u8Cont++;
		u16Multiplos *= 10;
	}
*/
	while(TempIndex <= TempLen)
	{
		sTxRxStruc.u8BfrTx[sTxRxStruc.u8BfrTxIndex2] = u8TempStr[TempIndex2];
		TempIndex++;
		TempIndex2++;
		sTxRxStruc.u8BfrTxIndex2++;
		if(sTxRxStruc.u8BfrTxIndex2 >= u8TxSize)
			sTxRxStruc.u8BfrTxIndex2 = 0;
	}
	UART0_TransmiteCadena();
	uBanderasUart0.bitBandera.DatoRecibido = 0;

}

void UART0_IRQHandler(void)
{
	uint8_t DatoTemp = 0;
	if(UART0->S1 & UART_S1_RDRF_MASK)  //Consulta la badera RDRF(Paso 1 pra borrar bandera RDRF)
	{
		DatoTemp = UART0->D;
		sTxRxStruc.u8BfrRx[sTxRxStruc.u8BfrRxIndex1] = DatoTemp;
		if(DatoTemp == 'P')
			uBanderasUart0.bitBandera.DatoRecibido = 1;
	}

}

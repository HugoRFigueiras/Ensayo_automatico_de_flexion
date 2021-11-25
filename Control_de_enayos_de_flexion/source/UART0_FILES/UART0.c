/*
 * UART0.c
 *
 *  Created on: 19 oct. 2021
 *      Author: hugoj
 */

#include "UART0.h"

uUART0_Banderas uUart0Status;


uint8_t u8Indice1Rx;
uint8_t u8Indice2Rx;
uint8_t u8BufferRxUart0[20];

/*****
 * Nombre: UART0_Configuracion
 * Funcon: Establecer los valores iniciales del modulo UART0
 * Parametros: Ninguno
 * Return: void
 ****/
void UART0_Configuracion(void)
{
	//Selecciona MCGFLLCLK (20.97152MHz)como fuente de reloj
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SCGC4 |= SIM_SCGC4_UART0(1);   //Habilita Clock a UART0
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	PORTE->PCR[21] |= PORT_PCR_MUX(4);  //Puerto E pin 21 Rx
	PORTE->PCR[20] |= PORT_PCR_MUX(4);  //Puerto E pin 20 Tx

    //Deshabilita Rx y Tx para configurar BDR
	UART0->C2  = UART_C2_RIE(0) & UART_C2_TE(0)
	&  UART_C2_RE(0);                   //Deshabilita interrupciones
	UART0->C3      = 0;
	UART0->C1      = 0;

	//                    UarttModuleClock
	//UART baud rate = ----------------------
	//                   ((OSR+1)xBR).
	//UarttModuleClock = Core Clock =  20.97152MHz.
	UART0->C4  = UART0_C4_OSR (7);
	UART0->BDH = UART0_BDH_SBR(0x01);
	UART0->BDL = UART0_BDL_SBR(0x11);
	u8Indice1Rx = 0;
	u8Indice2Rx = 0;

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
void UART0_TransmitirByte(uint8_t byteSaliente)
{
	while((UART0->S1 & UART_S1_TDRE_MASK) == 0);
    UART0->D = byteSaliente;
}

void UART0_IRQHandler(void)
{
	uint8_t TempChar;
	//Consulta la badera RDRF(Paso 1 pra borrar bandera RDRF)
	if(UART0->S1 & UART_S1_RDRF_MASK)
	{
		TempChar = UART0->D;
		if((TempChar != ' ') && uUart0Status.bitU0Bandera.ValorInvalido != 1)
		{
			u8BufferRxUart0[u8Indice1Rx] = TempChar;
			u8Indice1Rx++;
		}
		if((TempChar == 'k'))
		{
			u8BufferRxUart0[u8Indice1Rx - 1] = '\0';
		}
		if((TempChar == 'N'))
		{
			uUart0Status.bitU0Bandera.ValorInvalido = 1;
			u8Indice1Rx = 0;
		}
		if((TempChar == '\r') || (u8Indice1Rx >= 10))
		{
			uUart0Status.bitU0Bandera.MedicionExitosa = 1;
			u8BufferRxUart0[u8Indice1Rx] = '\0';
			u8Indice1Rx = 0;

		}

	}
}

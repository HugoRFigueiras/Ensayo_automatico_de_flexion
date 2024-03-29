/*
 * UART1.h
 *
 *  Created on: 20 nov. 2021
 *      Author: hugoj
 */

#ifndef UART1_FILES_UART1_H_
#define UART1_FILES_UART1_H_

#include "MKL25Z4.h"

#define u8TxSize 20
#define u8RxSize 500

typedef union
{
	uint8_t u8Todas;
	struct
	{
		uint8_t PruebaTipo1      :1,//B0
		        PruebaTipo2      :1,//B1
			    PruebaTipo3      :1,//B2
		        DatoDisponible   :1,//B3
		        DetenerMedicion  :1,//B4
				IniciarPrueba    :1,//B5
				DatoRecibido     :1,//B6
				CancelarPrueba   :1;//B7


	}bitBandera;
}uUART1_Banderas;

typedef struct
{
	//uUART1_Banderas uBanderasUart0;
	uint8_t *up8BfrTx;
	uint8_t u8BfrTx[u8TxSize];     //Array que contiene los datos a transmitir.
	uint8_t u8BfrRx[u8RxSize];     //Array que contiene los datos recibidos.
	uint8_t u8BfrTxIndex1;   //Posicion del ultimo dato transmitido.
	uint8_t u8BfrTxIndex2;   //Posicion del ultimo dato aun por transmitir.
	uint16_t u8BfrRxIndex1; //Posicion del ultimo dato recibido
	uint16_t u8BfrRxIndex2; //Posicion del ultimo dato utilizado (extraido)
}sUart1TxRx;

extern sUart1TxRx sTxRxStrucU1;
extern uUART1_Banderas uBanderasUart1;

void UART1_Configuracion(void);
void UART1_TransmitirByte(uint8_t* byteSaliente);
void UART1_TransmiteCadena(void);
void UART1_TransmiteTest(uint8_t Dato);
void UART1_LlenarBfrTx(float fPesoSimulado);
void UART1_Control(void);

#endif /* UART1_FILES_UART1_H_ */

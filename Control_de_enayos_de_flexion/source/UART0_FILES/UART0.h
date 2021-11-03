/*
 * UART0.h
 *
 *  Created on: 19 oct. 2021
 *      Author: hugoj
 */

#ifndef UART0_FILES_UART0_H_
#define UART0_FILES_UART0_H_
#include "MKL25Z4.h"
                     /*** VARIABLES GLOBALES ***/
#define u8TxSize 20
#define u8RxSize 50

typedef union
{
	uint8_t u8Todas;
	struct
	{
		uint8_t Configurando     :1,//B0
		        Ocupado          :1,//B1
			    DatoRecibido     :1,//B2
		        DatoDisponible   :1,//B3
		        DetenerMedicion  :1,//B4
				SiguienteByte    :1,//B5
		        BitsRestantes    :2;//B6,B7


	}bitBandera;
}uUART0_Banderas;



typedef struct
{
	//uUART0_Banderas uBanderasUart0;
	uint8_t *up8BfrTx;
	uint8_t u8BfrTx[u8TxSize];     //Array que contiene los datos a transmitir.
	uint8_t u8BfrRx[u8RxSize];     //Array que contiene los datos recibidos.
	uint8_t u8BfrTxIndex1;   //Posicion del ultimo dato transmitido.
	uint8_t u8BfrTxIndex2;   //Posicion del ultimo dato aun por transmitir.
	uint8_t u8BfrRxIndex1; //Posicion del ultimo dato recibido
	uint8_t u8BfrRxIndex2; //Posicion del ultimo dato utilizado (extraido)
}sUart0TxRx;

extern uUART0_Banderas uBanderasUart0;

                     /*** ENCABEZADOS DE FUNCIONES ***/

void UART0_Configuracion   (void);
void UART0_TransmitirByte  (uint8_t* byteSaliente);
void UART0_TransmiteCadena (void);
void UART0_LlenarBfrTx     (float fPesoSimulado);
void UART0_TransmiteTest   (uint8_t Dato);


#endif /* UART0_FILES_UART0_H_ */

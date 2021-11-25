/*
 * UART0.h
 *
 *  Created on: 19 oct. 2021
 *      Author: hugoj
 */

#ifndef UART0_FILES_UART0_H_
#define UART0_FILES_UART0_H_

#include "MKL25Z4.h"

typedef union
{
	uint8_t uU08Todas;
	struct
	{
		uint8_t Configurando     :1,//B0
		        Ocupado          :1,//B1
			    DatoRecibido     :1,//B2
		        DatoDisponible   :1,//B3
		        DetenerMedicion  :1,//B4
				SiguienteByte    :1,//B5
		        MedicionExitosa  :1,//B6
				ValorInvalido    :1;//B7


	}bitU0Bandera;
}uUART0_Banderas;

extern uUART0_Banderas uUart0Status;
extern uint8_t u8BufferRxUart0[20];
void UART0_Configuracion   (void);
void UART0_TransmitirByte  (uint8_t byteSaliente);


#endif /* UART0_FILES_UART0_H_ */

/*
 * UART0.h
 *
 *  Created on: 19 oct. 2021
 *      Author: hugoj
 */

#ifndef UART0_FILES_UART0_H_
#define UART0_FILES_UART0_H_

#include "MKL25Z4.h"


extern uint8_t DatoX;

void UART0_Configuracion   (void);
void UART0_TransmitirByte  (uint8_t byteSaliente);


#endif /* UART0_FILES_UART0_H_ */

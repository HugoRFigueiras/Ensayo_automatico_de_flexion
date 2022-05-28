/*
 * SPI.h
 *
 *  Created on: 12 abr. 2022
 *      Author: hugoj
 */

#ifndef SPI_H_
#define SPI_H_

#include "MKL25Z4.h"

typedef enum
{
	csS_Low = 0,
	csS_High,
}u8cs_state;

void    SPIO_csCtrl        (uint8_t CsState);
void    SPI0_initialize    (void);
uint8_t SPI0_writeByte (uint8_t data);



#endif /* SPI_H_ */

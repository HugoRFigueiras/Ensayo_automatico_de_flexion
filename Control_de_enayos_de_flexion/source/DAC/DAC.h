/*
 * DAC.h
 *
 *  Created on: 2 mar. 2022
 *      Author: hugoj
 */

#ifndef DAC_DAC_H_
#define DAC_DAC_H_

#include "MKL25Z4.h"

void Dac0_Init(void);
void Dac0_SetValHex(uint8_t *up8Cont);

#endif /* DAC_DAC_H_ */

/*
 * valvula.h
 *
 *  Created on: 25 oct. 2022
 *      Author: hugoj
 */

#ifndef VALVULAS_VALVULA_H_
#define VALVULAS_VALVULA_H_

#include "MKL25Z4.h"

#define SET_ON 0x00
#define SET_OFF 0x01

void ctrlPins(void);
void valPosX(uint8_t sol1, uint8_t sol2);

#endif /* VALVULAS_VALVULA_H_ */

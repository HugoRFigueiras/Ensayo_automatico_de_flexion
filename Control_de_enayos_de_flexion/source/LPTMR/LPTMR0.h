/*
 * LPTMR0.h
 *
 *  Created on: 12 abr. 2022
 *      Author: hugoj
 */

#ifndef LPTMR0_H_
#define LPTMR0_H_

/*** INCLUDES ***/
#include "MKL25Z4.h"

/*** Globals ***/


/*** Function Definitions ***/
uint8_t timeout(void);
void LPTMR0_Init(void);
void LPTMR0_Waitms(uint8_t msCount);




#endif /* LPTMR0_H_ */

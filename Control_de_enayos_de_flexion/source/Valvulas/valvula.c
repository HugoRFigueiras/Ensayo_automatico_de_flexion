/*
 * valvula.c
 *
 *  Created on: 25 oct. 2022
 *      Author: hugoj
 */

#include "valvula.h"

void ctrlPins(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //| SIM_SCGC5_PORTB_MASK;
	PORTE->PCR[30] |= PORT_PCR_MUX(1); //Control Rele 1
	PORTE->PCR[29] |= PORT_PCR_MUX(1); //Control Rele 2
	//PORTB->PCR[19] |= PORT_PCR_MUX(1);
	//GPIOB->PDDR |= (1 << 19);
	GPIOE->PDDR |= (1 << 29) | (1 << 30);
	GPIOE->PSOR |= (1 << 29) | (1 << 30);
}
/************************************
 * sol1 es controlado por gpioe 29 y sol2 es controlado por gpioe30
 * */
void valPosX(uint8_t sol1, uint8_t sol2)
{
	if(sol1 == SET_ON)
		GPIOE->PCOR |= (1<< 29);
	else if (sol1 == SET_OFF)
		GPIOE->PSOR |= (1 << 29);
	if(sol2 == SET_ON)
		GPIOE->PCOR |= (1 << 30);
	else if (sol2 == SET_OFF)
		GPIOE->PSOR |= (1 << 30);
	return;
}

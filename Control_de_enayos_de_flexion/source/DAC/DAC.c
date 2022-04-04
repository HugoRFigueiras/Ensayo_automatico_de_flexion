/*
 * DAC.c
 *
 *  Created on: 2 mar. 2022
 *      Author: hugoj
 */

#include "DAC.h"

void Dac0_Init(void)
{
	SIM->SCGC6  |= SIM_SCGC6_DAC0(1);
	SIM->SCGC5  |= SIM_SCGC5_PORTE(1);
	PORTE->PCR[30] = (PORT_PCR_MUX(0x000));

	DAC0->C1 = DAC_C1_DACBFEN(0);
	DAC0->C0 = DAC_C0_DACTRGSEL(1);
	DAC0->C0 |= DAC_C0_DACEN(1);
	//NVIC_EnableIRQ(LPTMR0_IRQn);
}

void Dac0_SetValHex(uint8_t *up8Cont)
{
    static uint16_t v1 = 0x0000;
    uint8_t mvDac = 124;
    if(*up8Cont < 33)
    {
        DAC0->DAT->DATL = (v1 & 0xFF);
        DAC0->DAT->DATH = ((v1 >> 8) & 0x0F);
        v1 += mvDac;
    }
    else if(*up8Cont >= 33)
    {
    	v1 = 0x0000;
        DAC0->DAT->DATL = (v1 & 0xFF);
        DAC0->DAT->DATH = ((v1 >> 8) & 0x0F);
    }
}


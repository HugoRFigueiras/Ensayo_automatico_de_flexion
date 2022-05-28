/*
 * LPTMR0.c
 *
 *  Created on: 12 abr. 2022
 *      Author: hugoj
 */

#include "LPTMR0.h"
#include "SDHC.h"

uint8_t bandera = 0;
uint8_t timeout(void)
{
	if(bandera == 0x01)
		return 0x01;
	else
		return 0x00;
}

void LPTMR0_Init(void)
{
	SIM->SCGC5  |= SIM_SCGC5_LPTMR(1);   //Clock gatinng
	LPTMR0->CSR  = LPTMR_CSR_TMS(0);
	LPTMR0->PSR |= LPTMR_PSR_PBYP(1) | LPTMR_PSR_PCS(0x01);
	LPTMR0->CMR  = (LPTMR_CMR_COMPARE(10));
	LPTMR0->CSR |= (LPTMR_CSR_TIE(1) | LPTMR_CSR_TEN(1));
	NVIC_EnableIRQ(LPTMR0_IRQn);
}

void LPTMR0_IRQHandler(void)
{
	//uint8_t temp;
	if(LPTMR0->CSR & LPTMR_CSR_TCF_MASK )
	{
		LPTMR0->CSR |= LPTMR_CSR_TCF(1);
		//GPIOE->PTOR = (1 << 30);
	    LPTMR0->CSR &= !LPTMR_CSR_TEN(0);

        //temp = SPI0_wrtOneByte(0x00);
        //temp = SPI0_wrtOneByte(0x00);
		bandera = 0x01;
		//SDHC_writeCommand(0x40,0x00000000,0x95);
	}
}

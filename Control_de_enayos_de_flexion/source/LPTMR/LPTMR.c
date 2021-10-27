/*
 * LPTMR.c
 *
 *  Created on: 25 oct. 2021
 *      Author: hugoj
 */


#include "LPTMR.h"
#include "MKL25Z4.h"

uint8_t Bandera;

void Init_LPTMR(unsigned int Time_ms)
{
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;
	LPTMR0->CSR &= ~LPTMR_CSR_TMS_MASK;

	/*
	 * Prescaler divides the clock by 2
	 * Select the the LPO as clock source
	 * The prescaler is bypassed
	 * */
	LPTMR0->PSR |= LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK ;

	/*The compare value is Time_ms*/
	LPTMR0->CMR =LPTMR_CMR_COMPARE(Time_ms);

	/*
	 * Enable the interrupt for the LPTMR
	 * LPTMR is reset on overflow
	 * LPTMR in counter mode
	 * Enable the LPTMR
	*/
	LPTMR0->CSR |= LPTMR_CSR_TIE_MASK | LPTMR_CSR_TEN_MASK;
	NVIC_EnableIRQ(LPTMR0_IRQn);

}

void LPTimer_IRQHandler(void)
{
	Bandera = 1;
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
}

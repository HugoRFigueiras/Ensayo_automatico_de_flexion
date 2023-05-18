/*
 * SPI.c
 *
 *  Created on: 12 abr. 2022
 *      Author: hugoj
 */

#include "SPI.h"

void SPIO_csCtrl(uint8_t CsState)
{
	if(CsState == 1)
		GPIOC->PSOR   |= (1<<4);
	    //GPIO_PCOR_PTSO(x)
	if(CsState == 0)
		GPIOC->PCOR   |= (1<<4);
	    //GPIO_PSOR_PTSO(x)
}


uint8_t SPI0_writeByte(uint8_t data)
{
	uint8_t datRead;
	while((SPI1->S & SPI_S_SPTEF_MASK) != SPI_S_SPTEF_MASK);
	SPI1->D = data;
	while((SPI1->S & SPI_S_SPRF_MASK) != SPI_S_SPRF_MASK);
	datRead = SPI1->D;
	datRead = datRead;
	return datRead;
}

void SPI0_initialize(void)
{
	SIM->SCGC4 |= SIM_SCGC4_SPI1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTC_MASK ;

	PORTC->PCR[4] |= PORT_PCR_MUX(1);//SPI CS
	GPIOC->PDDR    = (1<<4);
	GPIOC->PSOR    = (1<<4);

	PORTD->PCR[5] |= PORT_PCR_MUX(2);//SPI SCL0
	PORTD->PCR[6] |= PORT_PCR_MUX(2); //SPI MOSI
	PORTD->PCR[7] |= PORT_PCR_MUX(2); //SPI MISO

	SPI1->C1 |= SPI_C1_CPOL(0) | SPI_C1_CPHA(0);
	SPI1->C1 &= ~(SPI_C1_CPHA_MASK);
	//SPI0->C1 |= SPI_C1_SPIE(1);
	SPI1->C1 |= SPI_C1_MSTR(1);

	SPI1->BR |= SPI_BR_SPPR(7) | SPI_BR_SPR(2);
	SPI1->C1 |= SPI_C1_SPE_MASK;
    //NVIC_EnableIRQ(SPI0_IRQn);
}

//void SPI0_IRQHandler(void)
//{
//	uint8_t temp;
//	while(!(SPI0->S & SPI_S_SPRF_MASK));
//	temp = SPI0->D;
//
//
//}

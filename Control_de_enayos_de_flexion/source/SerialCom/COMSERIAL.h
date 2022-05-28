/*
 * COMSERIAL.h
 *
 *  Created on: 23 nov. 2021
 *      Author: hugoj
 */

#ifndef SERIALCOM_COMSERIAL_H_
#define SERIALCOM_COMSERIAL_H_

#include "UART0.h"
#include "UART1.h"

typedef enum
{
	eU1C_NuevaPrueba = 0,
	eUIC_EsperaSolicitud,
	eU1C_MidiendoPeso,
	eUIC_IncrementaPresion,
	eUIC_PesoARaspberry,
	eUIC_RegistraPeso,
	eU1C_Reset,
}eU1C_MedirPeso;

void COMSERIAL_Inicializa(void);
void COMSERIAL_Control(void);

#endif /* SERIALCOM_COMSERIAL_H_ */

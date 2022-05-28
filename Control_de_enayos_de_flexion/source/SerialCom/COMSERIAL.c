/*
 * COMSERIAL.c
 *
 *  Created on: 23 nov. 2021
 *      Author: hugoj
 */

#include "COMSERIAL.h"
#include <string.h>
#include "DAC.h"
#include "UAPP.h"

void COMSERIAL_Inicializa(void)
{
	UART0_Configuracion();
	UART1_Configuracion();
	Dac0_Init();
}

void COMSERIAL_Control(void)
{
	static uint8_t U1C_Status = eU1C_NuevaPrueba;
	static uint8_t cont = 0;
	uint8_t u8resp,sizeDat;
	uint16_t buffsize;
	switch (U1C_Status)
	{
	case eU1C_NuevaPrueba:
		if(uBanderasUart1.bitBandera.IniciarPrueba == 1)
		{
			if(uBanderasUart1.bitBandera.DatoRecibido == 1)
			{
				buffsize =sTxRxStrucU1.u8BfrRxIndex2 - sTxRxStrucU1.u8BfrRxIndex1;
				u8resp = UAPP_newFile(sTxRxStrucU1.u8BfrRx,buffsize);
				if(u8resp != 0)
				{
					UART1_TransmiteTest('I');
					U1C_Status = eUIC_EsperaSolicitud;
					uBanderasUart1.bitBandera.CancelarPrueba = 1;
				}
				sTxRxStrucU1.u8BfrRxIndex2 = 0;
				uBanderasUart1.bitBandera.IniciarPrueba = 0;
				uBanderasUart1.bitBandera.DatoRecibido = 0;
				UART1_TransmiteTest('\n');
				U1C_Status = eUIC_EsperaSolicitud;
			}
		}
		break;
	case eUIC_EsperaSolicitud:
			if(uBanderasUart1.bitBandera.DatoRecibido == 1)
			{
				U1C_Status = eU1C_MidiendoPeso;
				uBanderasUart1.bitBandera.DatoRecibido = 0;
				UART0_TransmitirByte('P');

			}
			if((uBanderasUart1.bitBandera.CancelarPrueba == 1)||(uBanderasUart1.bitBandera.DetenerMedicion == 1))
				U1C_Status = eU1C_Reset;
		break;
	case eU1C_MidiendoPeso:
		if(uUart0Status.bitU0Bandera.MedicionExitosa == 1)
		{
			if(uUart0Status.bitU0Bandera.ValorInvalido == 1)
				strcpy(sTxRxStrucU1.u8BfrTx,"0.000");
			else
				strcpy(sTxRxStrucU1.u8BfrTx,u8BufferRxUart0);
			sTxRxStrucU1.u8BfrTxIndex2 = strlen(sTxRxStrucU1.u8BfrTx);
			sTxRxStrucU1.u8BfrTx[sTxRxStrucU1.u8BfrTxIndex2] = '\n';
			U1C_Status = eUIC_IncrementaPresion;
		}
		break;

	case eUIC_IncrementaPresion:
	    Dac0_SetValHex(&cont);
	    if(cont >= 33)
	    {
	    	UART1_TransmiteTest('I');
	    	Dac0_SetValHex(&cont);
	    	cont = 0;
	    }
	    else
	    {
	    	cont++;
	    }
	    U1C_Status = eUIC_PesoARaspberry;
		break;
	case eUIC_PesoARaspberry:
		UART1_TransmiteCadena();
		U1C_Status = eUIC_RegistraPeso;
		break;
case eUIC_RegistraPeso:
		if(uBanderasUart1.bitBandera.DatoRecibido == 1)
		{
			sizeDat =sTxRxStrucU1.u8BfrRxIndex2 - sTxRxStrucU1.u8BfrRxIndex1;
			u8resp = UAPP_writeAppend(sTxRxStrucU1.u8BfrRx,sizeDat);
			if(u8resp != 0)
			{
				UART1_TransmiteTest('I');
				U1C_Status = eU1C_Reset;
				uBanderasUart1.bitBandera.CancelarPrueba = 1;
			}
			sTxRxStrucU1.u8BfrRxIndex2 = 0;
			uBanderasUart1.bitBandera.DatoRecibido = 0;
			U1C_Status = eU1C_Reset;
		}
		break;
	case eU1C_Reset:
		uBanderasUart1.bitBandera.DetenerMedicion = 0;
		uBanderasUart1.bitBandera.DatoRecibido = 0;
		uBanderasUart1.bitBandera.DetenerMedicion = 0;
		uUart0Status.bitU0Bandera.ValorInvalido = 0;
		uUart0Status.bitU0Bandera.MedicionExitosa = 0;
		U1C_Status = eUIC_EsperaSolicitud;
		if((uBanderasUart1.bitBandera.CancelarPrueba == 1)||(uBanderasUart1.bitBandera.DetenerMedicion == 1))
		{
			//PesoSim = 0;
			cont = 'S';
			Dac0_SetValHex(&cont);
			ap_cfName();
			U1C_Status = eU1C_NuevaPrueba;
			uBanderasUart1.bitBandera.CancelarPrueba = 0;
			uBanderasUart1.bitBandera.DetenerMedicion = 0;
		}
	break;
	}

}

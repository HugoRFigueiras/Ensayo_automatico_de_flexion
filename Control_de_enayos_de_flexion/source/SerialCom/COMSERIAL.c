/*
 * COMSERIAL.c
 *
 *  Created on: 23 nov. 2021
 *      Author: hugoj
 */

#include "COMSERIAL.h"
#include <string.h>
#include "DAC.h"

void COMSERIAL_Inicializa(void)
{
	UART0_Configuracion();
	UART1_Configuracion();
	Dac0_Init();
}

void COMSERIAL_Control(void)
{
	static uint8_t U1C_Status = eU1C_AtencionPeso;
	static uint8_t cont = 0;
	switch (U1C_Status)
	{
	case eU1C_AtencionPeso:
		if(uBanderasUart1.bitBandera.DatoRecibido == 1)
		{
			U1C_Status = eU1C_MidiendoPeso;
			UART0_TransmitirByte('P');
		}
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
		U1C_Status = eU1C_Reset;
		break;
	case eU1C_Reset:
		uBanderasUart1.bitBandera.DetenerMedicion = 0;
		uBanderasUart1.bitBandera.DatoRecibido = 0;
		uBanderasUart1.bitBandera.DetenerMedicion = 0;
		uUart0Status.bitU0Bandera.ValorInvalido = 0;
		uUart0Status.bitU0Bandera.MedicionExitosa = 0;
		U1C_Status = eU1C_AtencionPeso;
		if(uBanderasUart1.bitBandera.CancelarPrueba == 1)
		{
			//PesoSim = 0;
			uBanderasUart1.bitBandera.CancelarPrueba = 0;
		}
	break;
	}

}

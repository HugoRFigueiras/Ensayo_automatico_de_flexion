/*
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    Control_de_enayos_de_flexion.c
 * @brief   Application entry point.
 */

#include "MKL25Z4.h"
#include "UART1.h"
#include "UART0.h"
//#include "LPTMR.h"

/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */

float PesoSim = 0.0;
uint8_t count = 0;
uint8_t Pregunta = '0';
int main(void)
{
UART0_Configuracion();
UART1_Configuracion();
//UART0_TransmiteTest('L');
//UART0_TransmiteTest('L');

//Init_LPTMR(5000);
   for(;;)
    {
    	if(uBanderasUart1.bitBandera.DatoRecibido == 1)
    	{
    		if(count <= 0)
    		{
    			UART1_LlenarBfrTx(PesoSim);
    			count++;
    		}
    		if(uBanderasUart1.bitBandera.SiguienteByte == 1)
    		{
    			count++;
    			UART1_TransmiteCadena();
    			uBanderasUart1.bitBandera.SiguienteByte = 0;
    		}
    		if((count >= u8TxSize) || (uBanderasUart1.bitBandera.DetenerMedicion == 1) )
    		{
    			PesoSim += 0.345;
    			uBanderasUart1.bitBandera.DetenerMedicion = 0;
    			uBanderasUart1.bitBandera.DatoRecibido = 0;
    			uBanderasUart1.bitBandera.DetenerMedicion = 0;
    			count = 0;
    			if(uBanderasUart1.bitBandera.CancelarPrueba == 1)
    			{
    				PesoSim = 0;
    				uBanderasUart1.bitBandera.CancelarPrueba = 0;
    			}
    		}
    	}
        if(Pregunta == 'P')
        {
        	Pregunta = '0';
        	UART0_TransmitirByte('P');

        }

    }
    //return 0 ;
}

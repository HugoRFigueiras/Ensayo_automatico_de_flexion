/*
 * SDHC.h
 *
 *  Created on: 11 abr. 2022
 *      Author: hugoj
 */

#ifndef SDHC_H_
#define SDHC_H_

/***
*    Includes
***/
#include "MKL25Z4.h"
#include "SPI.h"
#include "diskio.h"

/***
*    Golbals
***/


/***
 *   Function Declarations
***/
void    SDHC_setCs                (uint8_t select);
uint8_t SDHC_writeCommand         (uint8_t cmdn,uint32_t arg,uint8_t crc);
uint8_t SDHC_waitReady            (void);
uint8_t SDHC_initialize           (void);
uint8_t SDHC_diskStat             (void);
uint8_t SDHC_writeBlock           (const uint8_t *buffer,uint8_t token);
uint8_t SDHC_dataWrite            (const uint8_t *buffer, uint32_t sectorAdr, uint8_t count);
uint8_t SDHC_dataRead             (uint8_t *buffer, uint32_t sectorAdr, uint8_t count);
uint8_t SDHC_readRegisterCSD      (uint8_t *u8buff);
uint8_t SDHC_readRegisterSDStatus (uint8_t *u8buff);
uint8_t SDHC_sectorCount          (uint32_t *c_size);
uint8_t SDHC_readBlockSize        (uint32_t * u32AUsize);
uint8_t SDHC_eraseSectorBlock     (uint32_t *AddrBuf);





#endif /* SDHC_H_ */

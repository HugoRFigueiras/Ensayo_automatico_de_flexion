/*
 * UAPP.h
 *
 *  Created on: 16 may. 2022
 *      Author: hugoj
 */

#ifndef FFS_UAPP_H_
#define FFS_UAPP_H_

#include "ff.h"
#include <string.h>

#define NFSIZE 12
typedef enum
{
	sDoc_mountDisk = 0,
	sDoc_FilStat,
	sDoc_NewFilName,
	sDoc_openFil,
	sDoc_writeDat,
}statDoc;



FRESULT open_append (FIL* fp,const char* path);
FRESULT ap_cfName(void);
FRESULT UAPP_newFile(uint8_t *buff,uint16_t textSize);
FRESULT UAPP_writeAppend(uint8_t *buff, uint8_t size);
FRESULT UAPP_closeFile(void);
FRESULT ap_states(void);


#endif /* FFS_UAPP_H_ */

/*
 * UAPP.c
 *
 *  Created on: 16 may. 2022
 *      Author: hugoj
 */

#include "UAPP.h"

FATFS fs;
FIL fil;
UINT bw;
FILINFO flno;
char file1[NFSIZE] = "Test1.txt";

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

FRESULT ap_cfName(void)
{
	uint8_t i = 0,mult = 1,filNum = 0,u8Index;
	char temp2[NFSIZE] = ".txt",*u8pIn;

	if(sizeof(file1) > NFSIZE)
		return FR_INT_ERR;
	u8pIn = strchr(file1,'.');
	if(u8pIn == 0)
		return FR_INT_ERR;
	u8Index = (uint8_t)(u8pIn-file1);
	u8Index--;
	while((u8Index > 2) && ((file1[u8Index] >='0') && (file1[u8Index] <= '9')))
	{
		filNum += (file1[u8Index]-0x30)*mult;
		mult *= 10;
		u8Index--;
	}
	filNum++; u8Index++;  //Incrementa el numero de archivo actual.
	if(filNum > 99)       //No mas de 99 archivos.
		filNum = 1;
	if(filNum > 9)
	{
		file1[u8Index++] = (filNum /10) +0x30; file1[u8Index++] = (filNum % 10) + 0x30;
	}
	else
	{
		file1[u8Index++] = filNum + 0x30;
	}
	i = 0;
	while(u8Index < sizeof(file1)) //Anade el formato .txt al nombre.
	{
		file1[u8Index++] = temp2[i++];
	}
	return 0;
}

FRESULT UAPP_newFile(uint8_t *buff,uint16_t textSize)
{
	uint8_t fr;
	fr = f_mount(&fs,file1,0);
	if(fr != FR_OK)
		return fr;
	fr = f_open(&fil, file1, FA_WRITE | FA_OPEN_ALWAYS);
	if(fr != FR_OK)
		return fr;
	fr = f_write(&fil,buff , textSize, &bw); //±íÍ·
	if(fr != FR_OK)
		return fr;
	fr = f_close(&fil);
	if(fr != FR_OK)
		return fr;
//	fr = ap_cfName();
//	if(fr != FR_OK)
//		return fr;
	return FR_OK;
}
FRESULT UAPP_writeAppend(uint8_t *buff, uint8_t size)
{
	uint8_t fr;
//    fr = f_lseek(&fil, f_size(file1));
//    if(fr != FR_OK)
//    {
//    	f_close(fp);
//    	return fr;
//    }
	fr = open_append(&fil,file1);
	if(fr != FR_OK)
		return fr;
	fr = f_write(&fil,buff,size, &bw);
	if(fr != FR_OK)
    {
    	f_close(&fil);
    	return fr;
    }
	fr = f_close(&fil);
	if(fr != FR_OK)
		return fr;
	return fr;
}
FRESULT UAPP_closeFile(void)
{
	uint8_t fr;
	fr = f_close(&fil);
	return fr;
}
FRESULT ap_states(void)
{
	FRESULT fr;
	static uint8_t sDoc = sDoc_mountDisk;
	switch (sDoc)
	{
	case sDoc_mountDisk:
		fr = f_mount(&fs,file1,0);
		if(fr != FR_OK)
			return fr;
		sDoc = sDoc_FilStat;
		break;
	case sDoc_FilStat:
		fr = f_stat(file1,&flno);
		if(fr == FR_OK)
			sDoc = sDoc_NewFilName;
		else if(fr == FR_NO_FILE)
			sDoc = sDoc_openFil;
		else
			return fr;
		break;
	case sDoc_NewFilName:
		ap_cfName();
		sDoc = sDoc_openFil;
		break;
	}
	return fr;
}

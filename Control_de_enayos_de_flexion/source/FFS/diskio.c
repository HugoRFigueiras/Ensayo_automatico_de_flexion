/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

//#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "SDHC.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	uint8_t Stat;
	if(pdrv)
		return STA_NOINIT;
	Stat = SDHC_diskStat();
	return Stat;
}


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;result = SDHC_initialize();
	if(result == 0)// translate the reslut code here
	{
		stat = RES_OK;
	}
	else if(result == 1)
	{
		stat = STA_NODISK;
	}
	else
	{
		stat = STA_NOINIT;
	}
	return stat;
}
/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	if(count <= 0 || pdrv != 0)
	{
		return RES_PARERR;
	}
	result = SDHC_dataRead(buff, sector, count);
	if(result == 0)
	{
		res = RES_OK;
	}
	else if(result == 1)
	{
		res = RES_ERROR;
	}
	else if(result == 4)
	{
		res = RES_PARERR;
	}
	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;


	if(count <= 0 || pdrv != 0)
	{
		return RES_PARERR;
	}
	result = SDHC_dataWrite(buff, sector, count);
	if(result == 0)
	{
		res = RES_OK;
	}
	else if(result == 1)
	{
		res = RES_ERROR;
	}
	else if(result == 4)
	{
		res = RES_PARERR;
	}
	return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	DWORD c_size, au_size;
	uint8_t u8resp;
	//int result;

	if(pdrv != 0)
	{
		return RES_PARERR;
	}

	switch (cmd)
	{
	case CTRL_SYNC:
		res = RES_OK; //Nothing to do for this command if each write operation to the medium is completed in the disk_write function.
		break;

	case GET_SECTOR_COUNT:
		u8resp = SDHC_sectorCount(&c_size);
		if(u8resp != 0)
			return RES_ERROR;
		*(DWORD*)buff = c_size;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:
        *(WORD*)buff = 512;
        res = RES_OK;
		break;

	case GET_BLOCK_SIZE:
		u8resp = SDHC_readBlockSize(&au_size);
		if(u8resp != 0x00)
			return RES_ERROR;
		*(DWORD*)buff = au_size;
		res = RES_OK;
		break;

	case CTRL_TRIM:
		u8resp = SDHC_eraseSectorBlock(buff);
		if(u8resp != 0x00)
			return RES_ERROR;
		res = RES_OK;
		break;

	default :
		res = RES_PARERR;
		break;
	}
	return res;
}


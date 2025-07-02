/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include "internalFlash.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void InternalFlashHandle::SaveData (ui32 address, ui32 data)
{
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address, (ui64)data);
	qmDelayMs(50);  // todo reduce
}


ui32 InternalFlashHandle::ReadWord (ui32 address)
{
	__IO ui32 word = *(__IO ui32*)address;
	return (ui32)word;
}


void InternalFlashHandle::EraseSector (ui32 sector, ui8 sectorsNumber)
{
	FLASH_EraseInitTypeDef EraseInit = {0};

	HAL_FLASH_Unlock();
	qmDelayMs(1);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGSERR);

	EraseInit.TypeErase    = FLASH_TYPEERASE_SECTORS;
	EraseInit.Sector       = sector;
	EraseInit.NbSectors    = sectorsNumber;

	HAL_FLASHEx_Erase(&EraseInit, &sectorError);

	/* FLASH_Erase_Sector(sector, voltageRange); */
	qmDelayMs(50);

	HAL_FLASH_Lock();
}

/** ____________________________________________________________________
 *
 * 	@file		internalFlash.h
 *
 *	@author		qsivey
 *
 *	VK / TG:	@qsivey
 *	mail:		qsivey@gmail.com
 *
 *	K-Lab		Laboratory of robotics and mechatronics
 *	____________________________________________________________________
 */

#ifndef		_INTERNAL_FLASH_H_
#define		_INTERNAL_FLASH_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#include	"periph.h"

/* Can be referenced from stm32f7xx_hal_flash_ex.h */
#define		HW_FLASH_VOLTAGE_RANGE	FLASH_VOLTAGE_RANGE_3


class InternalFlashHandle
{
	private:

		ui32				voltageRange;
		ui32				sectorError;

	public :

							InternalFlashHandle(ui32 voltageRange)
							{
								this->voltageRange = voltageRange;
							}


		void				SaveData (ui32 address, ui32 data);
		ui32				ReadWord (ui32 address);
		void				EraseSector (ui32 sector, ui8 sectorsNumber);

		ui32				GetSectorError (void) { return sectorError; }

};


extern InternalFlashHandle InternalFlash;

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* _INTERNAL_FLASH_H_ */

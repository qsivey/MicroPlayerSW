/** ____________________________________________________________________
 *
 *	@file		analogInput.h
 *
 *				BaseCamElectronics Team
 *
 *				Mini-Remote Controller project
 *
 *				https://www.basecamelectronics.com
 *  ____________________________________________________________________
 */

#ifndef		_ANALOG_INPUTS_H_
#define		_ANALOG_INPUTS_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */


#include	"periph.h"

#define		ANALOG_VALUES_AVERAGE	16


class qcAnalogChannels : virtual public qcPeripheral
{
	private :

		ui16				analogValues [ANALOG_VALUES_AVERAGE];


	public :

		ui16				batteryLevel;

		void				ADC_Init (void);
		void				ReadBatteryLevel (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* _ANALOG_INPUTS_H_ */

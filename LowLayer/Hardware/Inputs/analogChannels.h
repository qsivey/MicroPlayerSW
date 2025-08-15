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


class qcAnalogChannels : virtual public qcPeripheral
{
	public :

		//ui16				batteryLevel;

		void				ADC_Init (void);
		ui16				ReadBatteryLevel (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* _ANALOG_INPUTS_H_ */

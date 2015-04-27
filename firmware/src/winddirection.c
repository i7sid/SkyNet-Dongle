/*
 * winddirection.c
 *
 *  Created on: 26.04.2015
 *      Author: christoph
 */

#include "winddirection.h"
#include "./misc/misc.h"
#include "./periph/adc.h"
#include "time.h"

#define WINDVANE	ADC_CHANNEL_EXT

RTC_TIME_T curTimedir;
RTC_TIME_T bufferedTimedir;

/*
 * you want to use this!
 */

int getWindDirection(){
	uint32_t ret = 0;
	unsigned int cnt = 0;
	Chip_RTC_GetFullTime(LPC_RTC, &curTimedir);
	// activate adc
	PINSET(ADC_PWR_PORT, ADC_PWR_PIN);
	msDelayActive(50);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CHANNEL, DISABLE);//disable charger channel
	Chip_ADC_EnableChannel(LPC_ADC, WINDVANE, ENABLE);

	int buftime = timeAdd(&curTimedir);
	int now = timeAdd(&curTimedir);

	while ((now - buftime) <= 5 ){


		Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
		// Waiting for A/D conversion complete
		while (Chip_ADC_ReadStatus(LPC_ADC, WINDVANE, ADC_DR_DONE_STAT) != SET) {
			//DBG("Waiting for ADC val \n");
		}

		// Read ADC value
		uint16_t tmp = 0;
		Chip_ADC_ReadValue(LPC_ADC, WINDVANE, &tmp);

		ret += tmp; //overvlow !!
		cnt++;

		Chip_RTC_GetFullTime(LPC_RTC, &curTimedir);
		now = timeAdd(&curTimedir);

	}
	// deactivate
	Chip_ADC_EnableChannel(LPC_ADC, WINDVANE, DISABLE);
	PINCLR(ADC_PWR_PORT, ADC_PWR_PIN);

	int dir = (ret/cnt)/11.37; //adc val to deg
	return dir;
}

/*
 * Wind Direction translation: 0-20kOhm, 10kOhm = 180deg = south
 *
 */
void setupadc(){
	DBG("Initialize Wind Vane...\n");
	//nothing to do already done in adc.h and called in main
	Chip_RTC_GetFullTime(LPC_RTC, &bufferedTimedir);
	DBG("Initialize Wind Vane complete...\n");
}


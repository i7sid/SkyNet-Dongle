/** @file
 *
 *  @date 06.06.2015
 *  @author Christoph Kohl
 *
 *  @brief Functionality of the GPS Module
 */

#include "../tools/bitprint.h"
#include "../../misc/debug.h"
#include "gpsmodule.h"
#include "../../misc/misc.h"
#include "ubx_nmea_parser.h"
#include "stdlib.h"
#include "string.h"

#define GPS_UART_INT_MASK 	(UART_IER_RBRINT)

struct gps_data data;

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 128	/* Send */
#define UART_RRB_SIZE 128	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];




bool gps_init(){

	Chip_IOCON_PinMux(LPC_IOCON, 0, 2, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON, 0, 3, IOCON_MODE_INACT, IOCON_FUNC1);

	Chip_UART_Init(GPS_UART);
	Chip_UART_SetBaud(GPS_UART, 9600);
	Chip_UART_ConfigData(GPS_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(GPS_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(GPS_UART);

	/* Before using the ring buffers, initialize them using the ring
		   buffer init function */
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(GPS_UART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(GPS_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(GPS_IRQn, 1);
	NVIC_EnableIRQ(GPS_IRQn);

	config_ublox();
	poll_one_message();

	return true;
}

/**
 * disabling all messages except fix data
 */

void config_ublox(){
	char* config1 = ("$PUBX,40,GLL,0,0,0,0*5C\r\n");
	char* config2 = ("$PUBX,40,ZDA,0,0,0,0*44\r\n");
	char* config3 = ("$PUBX,40,VTG,0,0,0,0*5E\r\n");
	char* config4 = ("$PUBX,40,GSV,0,0,0,0*59\r\n");
	char* config5 = ("$PUBX,40,GSA,0,0,0,0*4E\r\n");
	char* config6 = ("$PUBX,40,RMC,0,0,0,0*47\r\n");

	Chip_UART_SendRB(GPS_UART,&txring,(void*)config1,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config2,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config3,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config4,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config5,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config6,25);

}

void poll_messages(){
	char message [81];
	char get = 0;
	int count = 0;
	while (true){
		int read = Chip_UART_ReadRB(GPS_UART, &rxring, &get, 1);
		if(read > 0){
			message[count] = get;
			count++;
			if(get == '\n'){
				message[count -2] = '\0';
				DBG("%s\n",message);
				count = 0;
			}
		}
	}
}

void poll_one_message(){
	char message [81];
	char get = 0;
	int count = 0;
	while (true){
		int read = Chip_UART_ReadRB(GPS_UART, &rxring, &get, 1);
		if(read > 0){
			message[count] = get;
			count++;
			if(get == '\n'){
				message[count -2] = '\0';
				parse_message(message);//TODO checksum and fix
				return;
			}
		}
	}
}

bool parse_message(char * message){
	char * str = strtok(message, ",");
	if(strcmp(str,"$GPGGA")!=0)return false;
	str = strtok(NULL, ",");
	memcpy(data.utc,str,11);
	str = strtok(NULL, ",");
	memcpy(data.lat,str,10);
	str = strtok(NULL, ",");
	data.north = str[1];
	str = strtok(NULL, ",");
	memcpy(data.lon,str,11);
	str = strtok(NULL, ",");
	data.east=str[1];
	str = strtok(NULL, ",");
	data.status=(uint8_t)str[1];
	return true;
}

struct gps_data* get_gps_data(){
	return &data;
}

void GPS_IRQHandler (){
	Chip_UART_IRQRBHandler(GPS_UART, &rxring, &txring);
}

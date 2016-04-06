/** @file
 *
 *  @date 06.06.2015
 *  @author Christoph Kohl
 *
 *  @brief Functionality of the GPS Module
 */

#include "../../basestation_old/gps/gpsmodule.h"

#include "../../misc/debug.h"
#include "../../misc/misc.h"
#include "stdlib.h"
#include "string.h"

#include "../../basestation_old/config.h"
#include "../../basestation_old/gps/ubx_nmea_parser.h"
#include "../../basestation_old/tools/bitprint.h"


#define GPS_UART 			LPC_UART1
#define GPS_IRQn			UART1_IRQn
#define GPS_IRQ_Handler	UART1_IRQHandler

#define GPS_UART_INT_MASK 	(UART_IER_RBRINT)

struct gps_data data;

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 160	/* Send */
#define UART_RRB_SIZE 512	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];




bool gps_init(){

	Chip_IOCON_PinMux(LPC_IOCON, 2, 0, IOCON_MODE_INACT, IOCON_FUNC2);
	Chip_IOCON_PinMux(LPC_IOCON, 2, 1, IOCON_MODE_INACT, IOCON_FUNC2);

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

	while(!poll_one_message()){}; //wait for valid gps data
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
	char* config6 = ("$PUBX,40,GSA,0,0,0,0*4E\r\n");//Fixme sending configuration in different orders fixes messages overlapping
	char* config5 = ("$PUBX,40,RMC,0,0,0,0*47\r\n");

	Chip_UART_SendRB(GPS_UART,&txring,(void*)config1,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config2,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config3,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config4,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config5,25);
	Chip_UART_SendRB(GPS_UART,&txring,(void*)config6,25);

}

void shutdown_GPS(){
	//TODO set gps module in idle mode
}

bool poll_one_message(){
	char message [81];
	char get = 0;
	int count = 0;
	bool start = false;
	while (true){
		int read = Chip_UART_ReadRB(GPS_UART, &rxring, &get, 1);
		if(read > 0){
			if(get == '$'){ //found start
				start = true;
			}
			if(start){ //found start symbol read until end symbol
				message[count] = get;
				count++;
				if(get == '\n'){
					message[count -2] = '\0';
					DBG("%s\n",message);
					if(parse_message(message)){
						return true;
						//return false;
					}else{
						return false;
					}
				}
			}
		}
	}
}

void poll_messages(){
	/*
	char message [81];
	char get = 0;
	int count = 0;
	while (true){
		int read = Chip_UART_ReadRB(GPS_UART, &rxring, &get, 1);
		if(read > 0){
			message[count] = get;
			count++;
			if(count >= 80){
				DBG("Something is very wrong!\n");
				return false;
			}
			if(get == '\n'){
				message[count -2] = '\0';
				DBG("%s\n",message);
				if(!parse_message(message)){
					count = 0;

				}
				if(data.status == 0){ //satellite fix failed
					return false;
				}
				return true;
			}
		}
	}
	 */
	return;
}

bool parse_message(char * message){
	char buf[81];
	strcpy(buf,message);
	char * str = strtok(buf, ",");
	if (str == NULL)return false;
	if(strcmp(str,"$GPGGA")!=0)return false;
	if(checksum(message))return false;
	str = strtok(NULL, ",");
	if (str == NULL)return false;
	memcpy(data.utc,str,10);
	str = strtok(NULL, ",");
	if (str == NULL)return false;
	memcpy(data.lat,str,9);
	str = strtok(NULL, ",");
	if (str == NULL)return false;
	memcpy(data.north,str,1);
	str = strtok(NULL, ",");
	if (str == NULL)return false;
	memcpy(data.lon,str,10);
	str = strtok(NULL, ",");
	if (str == NULL)return false;
	memcpy(data.east,str,1);
	str = strtok(NULL, ",");
	if (str == NULL)return false;
	memcpy(data.status,str,1);
	return true;
}

struct gps_data* get_gps_data(){
	return &data;
}

void GPS_IRQ_Handler (){
	Chip_UART_IRQRBHandler(GPS_UART, &rxring, &txring);
}

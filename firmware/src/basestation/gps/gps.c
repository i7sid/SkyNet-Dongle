/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "gps.h"

gps_pubx_data current_data;

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T txring, rxring;

extern RTC_TIME_T FullTime;

/* Transmit and receive ring buffer sizes */
#define UART_SRB_SIZE 64	/* Send */
#define UART_RRB_SIZE 128	/* Receive */

/* Transmit and receive buffers */
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


int skynetbase_gps_init(void) {
	DBG("begin init GPS...\n");

	NVIC_DisableIRQ(GPS_IRQn);
	Chip_UART_IntDisable(GPS_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	Chip_UART_Init(GPS_UART);
	Chip_UART_SetBaud(GPS_UART, 9600);
	Chip_UART_ConfigData(GPS_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(GPS_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(GPS_UART);

	// Before using the ring buffers, initialize them using the ring buffer init function
	RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	// Reset and enable FIFOs, FIFO trigger level 3 (14 chars)
	Chip_UART_SetupFIFOS(GPS_UART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
			UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(GPS_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(GPS_IRQn, 1);
	NVIC_EnableIRQ(GPS_IRQn);


	DBG("GPS up.\n");

	skynetbase_gps_config("$PUBX,40,GLL,0,0,0,0*5C\r\n");
	msDelayActive(150);
	skynetbase_gps_config("$PUBX,40,ZDA,0,0,0,0*44\r\n");
	msDelayActive(150);
	skynetbase_gps_config("$PUBX,40,VTG,0,0,0,0*5E\r\n");
	msDelayActive(150);
	skynetbase_gps_config("$PUBX,40,GSV,0,0,0,0*59\r\n");
	msDelayActive(150);
	skynetbase_gps_config("$PUBX,40,RMC,0,0,0,0*47\r\n");
	msDelayActive(150);
	skynetbase_gps_config("$PUBX,40,GSA,0,0,0,0*4E\r\n");
	msDelayActive(150);
	skynetbase_gps_config("$PUBX,40,GGA,0,0,0,0*5A\r\n");
	msDelayActive(150);

	DBG("GPS configured.\n");

	// TODO
	return 1;
}

void skynetbase_gps_query(void) {
	char tmp[] = "$PUBX,00*33\r\n";
	Chip_UART_SendRB(GPS_UART, &txring, (void*)tmp, sizeof(tmp) - 1);
}

void skynetbase_gps_config(char msg[]) {
	Chip_UART_SendRB(GPS_UART, &txring, (void*)msg, strlen(msg));
}

gps_pubx_data* skynetbase_gps_get_data(void) {
	return &current_data;
}

char gpsout[128];
uint8_t gpsoutpos = 0;

void skynetbase_gps_receive_data(void) {
	char c;
	int bytes = Chip_UART_ReadRB(GPS_UART, &rxring, &c, 1);
	if (bytes < 1) return;

	if (c == '$') { // beginning
		gpsoutpos = 0;
	}
	else if (c == '*' && gpsoutpos > 0) { // ending, for now ignore checksum
		gpsout[gpsoutpos] = 0;
		/*
		char *data = malloc(gpsoutpos+1);
		memcpy(data, gpsout, gpsoutpos);
		data[gpsoutpos] = 0;
		events_enqueue(EVENT_GPS_MESSAGE, data);
		*/
		skynetbase_gps_received_data(gpsout, gpsoutpos);
		return;
	}

	gpsout[gpsoutpos++] = c;
}

void skynetbase_gps_received_data(char gpsout[], uint8_t gpsoutpos) {
	gpsout[gpsoutpos] = 0;
	DBG("%s\n", gpsout);

	char delim[] = ",";

	// parse message ID
	char *str = strtok(gpsout, delim);
	if (str == NULL) return;			// answer was too short
	if (strcmp(str, "$PUBX")) return;	// wrong answer type

	// parse propietary message identifier
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	if (strcmp(str, "00")) return;		// wrong answer type

	// parse time
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	if (strlen(str) < 6) return;		// wrong time format
	memcpy(current_data.time, str, 6);
	current_data.time[6] = 0;

	// parse latitude
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.lat = atof(str);

	// parse latitude direction
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.lat_dir = str[0];

	// parse longitude
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.lon = atof(str);

	// parse longitude direction
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.lon_dir = str[0];

	// parse altitude direction
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.alt = atof(str);

	// parse navigation status
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.status = '0';
	if (!(strcmp(str, "G3"))) current_data.status = 'G';
	else if (!(strcmp(str, "NF"))) current_data.status = 'N';

	// parse horizontal accuracy
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.accuracy_horizontal = atof(str);

	// parse vertical accuracy
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.accuracy_vertical = atof(str);

	/*
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	// ignore some data
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short


	// parse number of gps satellites // just works if no empty fields
	str = strtok(NULL, delim);
	if (str == NULL) return;			// answer was too short
	current_data.satellites = atoi(str);
	 */

	current_data.satellites = 0;

	events_enqueue(EVENT_GPS_DATA_AVAILABLE, NULL);

	// update RTC
	Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
	sscanf(current_data.time, "%02d%02d%02d", &(FullTime.time[RTC_TIMETYPE_HOUR]),
			&(FullTime.time[RTC_TIMETYPE_MINUTE]), &(FullTime.time[RTC_TIMETYPE_SECOND]));
	Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

	//DBG("Lat: %f (%c)\n", current_data.lat, current_data.lat_dir);
	//DBG("Lon: %f (%c)\n", current_data.lon, current_data.lon_dir);
}

void GPS_IRQ_Handler() {
	Chip_UART_IRQRBHandler(GPS_UART, &rxring, &txring);
}

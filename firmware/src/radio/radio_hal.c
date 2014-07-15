#include "radio_hal.h"
#include "radio.h"
#include "../spi/spi.h"
#include "../misc/misc.h"

void radio_hal_AssertShutdown(void)
{
	DBG("AssertShutdown: State of sdn pin: %d\n", PINGET(SI_LIB_SDN_PORT, SI_LIB_SDN_PIN));
	PINSET(SI_LIB_SDN_PORT, SI_LIB_SDN_PIN);
	DBG("AssertShutdown: State of sdn pin: %d\n", PINGET(SI_LIB_SDN_PORT, SI_LIB_SDN_PIN));
}

void radio_hal_DeassertShutdown(void)
{
	DBG("DeassertShutdown: State of sdn pin: %d\n", PINGET(SI_LIB_SDN_PORT, SI_LIB_SDN_PIN));
	PINCLR(SI_LIB_SDN_PORT, SI_LIB_SDN_PIN);
	DBG("DeassertShutdown: State of sdn pin: %d\n", PINGET(SI_LIB_SDN_PORT, SI_LIB_SDN_PIN));
}

void radio_hal_ClearNsel(void)
{
	SPI_AssertSSEL(); // TODO: board-unabhängig
	// Not neccessary, chip SPI driver does it
	//PINCLR(SPI_nCS);
}

void radio_hal_SetNsel(void)
{
	SPI_DeassertSSEL(); // TODO: board-unabhängig
	// Not neccessary, chip SPI driver does it
	//PINSET(SPI_nCS);
}

uint8_t radio_hal_NirqLevel(void)
{
	//TODO: stimmt so? Vorher war es:
	// return GET(SPI_nCS)
    return PINGET(SI_LIB_nIRQ_PORT, SI_LIB_nIRQ_PIN);
}

void radio_hal_SpiWriteByte(uint8_t byteToWrite)
{
	SPI_Trans(byteToWrite);
}

uint8_t radio_hal_SpiReadByte(void)
{

  return SPI_Trans(0xFF);
}

void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{
	SPI_TransBatchWrite(pData, byteCount);
}

void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{
	SPI_TransBatchRead(pData, byteCount);
}

#ifdef RADIO_DRIVER_EXTENDED_SUPPORT
BIT radio_hal_Gpio0Level(void)
{
#ifdef SILABS_PLATFORM_DKMB
  return 0;
#endif
#ifdef SILABS_PLATFORM_UDP
  return EZRP_RX_DOUT;
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB)
  return RF_GPIO0;
#endif
#if (defined SILABS_PLATFORM_WMB)
  return 0;
#endif
}

BIT radio_hal_Gpio1Level(void)
{
#ifdef SILABS_PLATFORM_DKMB
  return FSK_CLK_OUT;
#endif
#ifdef SILABS_PLATFORM_UDP
  return 0; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  return RF_GPIO1;
#endif
}

BIT radio_hal_Gpio2Level(void)
{
#ifdef SILABS_PLATFORM_DKMB
  return DATA_NFFS;
#endif
#ifdef SILABS_PLATFORM_UDP
  return 0; //No Pop
#endif
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  return RF_GPIO2;
#endif
}

BIT radio_hal_Gpio3Level(void)
{
#if (defined SILABS_PLATFORM_RFSTICK) || (defined SILABS_PLATFORM_LCDBB) || (defined SILABS_PLATFORM_WMB)
  return RF_GPIO3;
#else
  return 0;
#endif
}

#endif

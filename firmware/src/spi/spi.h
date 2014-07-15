/**
 * @file
 *
 * @date	26.04.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SPI_H_
#define SPI_H_

/// @brief	Initialize SPI port pins and SPI module.
void SPI_Init(void);

/**
 * @brief			Sends a single byte and receives the immediate answer byte.
 * @param	data	Byte to be sent.
 * @return			Received answer byte.
 */
uint8_t SPI_Trans(uint8_t data);

/**
 * @brief			Sends several bytes from SPI discarding the immediate response.
 * @param	data	Bytes to be sent.
 * @param	length	Amount (number) of bytes to be sent.
 */
void SPI_TransBatchWrite(uint8_t *data, uint8_t length);

/**
 * @brief			Reads several bytes from SPI without sending specific content.
 * @param	data	Return bytes are placed in data.
 * @param	length	Amount (number) of bytes to be read.
 */
void SPI_TransBatchRead(uint8_t *data, uint8_t length);

/// @brief	Assure that nSEL is set.
void SPI_AssertSSEL(void);

/// @brief	Assure that nSEL is not set.
void SPI_DeassertSSEL(void);


#endif /* SPI_H_ */

/*
 * SPI.h - dummy implementation of SPI class from Arduino for STM32F10X devices
 *
 *  Created on: Feb 13, 2021
 *      Author: tchv
 */

#ifndef SPI_H_
#define SPI_H_
#include "wirish_types.h"

enum BitOrder {
  LSBFIRST = 0,
  MSBFIRST = 1
};
#define SPI_MODE3 3
#define DATA_SIZE_8BIT  SPI_DATASIZE_8BIT
#define DATA_SIZE_16BIT SPI_DATASIZE_16BIT

class SPISettings
{
public:
  SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode, uint32_t dataSize)
  {
  }
};

class SPIClass
{
public:
  void beginTransaction(SPISettings settings);
  void setDataSize(uint32 ds);
  void write(uint16 data);
  /**
   * @brief Transmit multiple bytes/words.
   * @param buffer Bytes/words to transmit.
   * @param length Number of bytes/words in buffer to transmit.
   */
  void write(const void * buffer, uint32 length);
  uint8 dmaSend(const void * transmitBuf, uint16 length, bool minc = 1);
  /**
   * @brief Transmit one byte/word a specified number of times.
   * @param data to transmit.
   */
  void write(uint16 data, uint32 n);
};

extern SPIClass SPI;

#endif /* SPI_H_ */

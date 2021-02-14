/*
 * SPI.cpp
 *
 *  Created on: Feb 13, 2021
 *      Author: tchv
 */
#include <stddef.h>
#include <stm32f1xx.h>
#include "SPI.h"

#if 0
extern SPI_HandleTypeDef hspi1;

void SPIClass::beginTransaction (SPISettings settings)
{
}
void SPIClass::setDataSize (uint32 ds)
{
  hspi1.Init.DataSize = ds;
  HAL_SPI_Init(&hspi1);
  __HAL_SPI_ENABLE(&hspi1);
}
void SPIClass::write (uint16 data)
{
  uint16 buffer[1] =
    { data };
  write(buffer, 1);
}
void SPIClass::write (const void *buffer, uint32 length)
{
  HAL_SPI_Transmit(&hspi1, (uint8_t*) buffer, length, HAL_MAX_DELAY);
}

uint8 SPIClass::dmaSend (const void *transmitBuf, uint16 length, bool minc = 1)
{
  write((uint16*) transmitBuf, length);
  return 0;
}
void SPIClass::write (uint16 data, uint32 n)
{
  while (n-- != 0)
    write(data);
}
#else
extern "C"
{
//#include "spi_c.h"
#include "st7789.h"
}

void SPIClass::beginTransaction (SPISettings settings)
{
  //vInit_SPI1();
  st7789_GPIOInit();
}
void SPIClass::setDataSize (uint32 ds)
{
  uint32 cr1 = ST7789_SPI->CR1 & ~(SPI_CR1_DFF);
  ST7789_SPI->CR1 = cr1 | (ds & SPI_CR1_DFF);
}
void SPIClass::write (uint16 data)
{
  for (int32_t i = 0; i < 10000; i++)
  {
    if (ST7789_SPI->SR & SPI_SR_TXE)
      break;
  }
  ST7789_SPI->DR = data;
  while (ST7789_SPI->SR & SPI_SR_BSY)
    ;
}
/**
 * @brief Transmit multiple bytes/words.
 * @param buffer Bytes/words to transmit.
 * @param length Number of bytes/words in buffer to transmit.
 */
void SPIClass::write (const void *buffer, uint32 length)
{
#if 0
  for (uint32_t count = 0; count < length; count++)
  {
    while ((ST7789_SPI->SR & SPI_SR_TXE) == (uint16_t) RESET) ;
    ST7789_SPI->DR = ((uint16_t*) buffer)[count];
  }
  while (ST7789_SPI->SR & SPI_SR_BSY) ;
#else
  st7789_WriteDMA16((void *)buffer, length);
  st7789_WaitForDMA();

#endif
}

uint8 SPIClass::dmaSend (const void *transmitBuf, uint16 length, bool minc)
{
  //write(*(uint16*) transmitBuf, length);
  if (!minc)
    st7789_WriteDMA16Repeat((void *)transmitBuf, length);
  else
    st7789_WriteDMA16((void *)transmitBuf, length);

  st7789_WaitForDMA();
  return 0;
}
/**
 * @brief Transmit one byte/word a specified number of times.
 * @param data to transmit.
 */
void SPIClass::write (uint16 data, uint32 n)
{
  for (uint32_t count = 0; count < n; count++)
  {
    while ((ST7789_SPI->SR & SPI_SR_TXE) == (uint16_t) RESET) ;
    ST7789_SPI->DR = data;
  }
  while (ST7789_SPI->SR & SPI_SR_BSY) ;
}
#endif

SPIClass SPI;


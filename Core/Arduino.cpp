/*
 * Arduino.cpp
 *
 *  Created on: Feb 11, 2021
 *      Author: tchv
 */

#include "Arduino.h"

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] =
  {

    { GPIOA, GPIO_PIN_0 }, /* PA0 */
    { GPIOA, GPIO_PIN_1 }, /* PA1 */
    { GPIOA, GPIO_PIN_2 }, /* PA2 */
    { GPIOA, GPIO_PIN_3 }, /* PA3 */
    { GPIOA, GPIO_PIN_4 }, /* PA4 */
    { GPIOA, GPIO_PIN_5 }, /* PA5 */
    { GPIOA, GPIO_PIN_6 }, /* PA6 */
    { GPIOA, GPIO_PIN_7 }, /* PA7 */
    { GPIOA, GPIO_PIN_8 }, /* PA8 */
    { GPIOA, GPIO_PIN_9 }, /* PA9 */
    { GPIOA, GPIO_PIN_10 }, /* PA10 */
    { GPIOA, GPIO_PIN_11 }, /* PA11 */
    { GPIOA, GPIO_PIN_12 }, /* PA12 */
    { GPIOA, GPIO_PIN_13 }, /* PA13 */
    { GPIOA, GPIO_PIN_14 }, /* PA14 */
    { GPIOA, GPIO_PIN_15 }, /* PA15 */

    { GPIOB, GPIO_PIN_0 }, /* PB0 */
    { GPIOB, GPIO_PIN_1 }, /* PB1 */
    { GPIOB, GPIO_PIN_2 }, /* PB2 */
    { GPIOB, GPIO_PIN_3 }, /* PB3 */
    { GPIOB, GPIO_PIN_4 }, /* PB4 */
    { GPIOB, GPIO_PIN_5 }, /* PB5 */
    { GPIOB, GPIO_PIN_6 }, /* PB6 */
    { GPIOB, GPIO_PIN_7 }, /* PB7 */
    { GPIOB, GPIO_PIN_8 }, /* PB8 */
    { GPIOB, GPIO_PIN_9 }, /* PB9 */
    { GPIOB, GPIO_PIN_10 }, /* PB10 */
    { GPIOB, GPIO_PIN_11 }, /* PB11 */
    { GPIOB, GPIO_PIN_12 }, /* PB12 */
    { GPIOB, GPIO_PIN_13 }, /* PB13 */
    { GPIOB, GPIO_PIN_14 }, /* PB14 */
    { GPIOB, GPIO_PIN_15 }, /* PB15 */

    { GPIOC, GPIO_PIN_13 }, /* PC13 */
    { GPIOC, GPIO_PIN_14 }, /* PC14 */
    { GPIOC, GPIO_PIN_15 }, /* PC15 */

  };


/*
 * Arduino.h
 *
 *  Created on: Feb 11, 2021
 *      Author: tchv
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include "board.h"
#include "wirish_types.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"

#define millis HAL_GetTick
#define delay  HAL_Delay
/**
 * @brief Stores STM32-specific information related to a given Maple pin.
 * @see PIN_MAP
 */
typedef struct stm32_pin_info {
  GPIO_TypeDef * gpio_device;    /**< HAL pin's GPIO device */
  uint16_t gpio_bit;             /**< Pin's GPIO port bit. */
} stm32_pin_info;

extern const stm32_pin_info PIN_MAP[];

inline uint32 digitalRead (uint8 pin)
{
  return HAL_GPIO_ReadPin(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit);
}

inline void digitalWrite(uint8 pin, uint8 val)
{
  return HAL_GPIO_WritePin(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit, (GPIO_PinState)val);
}

static const int HIGH=1;
static const int LOW=0;

/**
 * Specifies a GPIO pin behavior.
 * @see pinMode()
 */
typedef enum WiringPinMode {
    OUTPUT, /**< Basic digital output: when the pin is HIGH, the
               voltage is held at +3.3v (Vcc) and when it is LOW, it
               is pulled down to ground. */

    OUTPUT_OPEN_DRAIN, /**< In open drain mode, the pin indicates
                          "low" by accepting current flow to ground
                          and "high" by providing increased
                          impedance. An example use would be to
                          connect a pin to a bus line (which is pulled
                          up to a positive voltage by a separate
                          supply through a large resistor). When the
                          pin is high, not much current flows through
                          to ground and the line stays at positive
                          voltage; when the pin is low, the bus
                          "drains" to ground with a small amount of
                          current constantly flowing through the large
                          resistor from the external supply. In this
                          mode, no current is ever actually sourced
                          from the pin. */

    INPUT, /**< Basic digital input. The pin voltage is sampled; when
              it is closer to 3.3v (Vcc) the pin status is high, and
              when it is closer to 0v (ground) it is low. If no
              external circuit is pulling the pin voltage to high or
              low, it will tend to randomly oscillate and be very
              sensitive to noise (e.g., a breath of air across the pin
              might cause the state to flip). */

    INPUT_ANALOG, /**< This is a special mode for when the pin will be
                     used for analog (not digital) reads.  Enables ADC
                     conversion to be performed on the voltage at the
                     pin. */

    INPUT_PULLUP, /**< The state of the pin in this mode is reported
                     the same way as with INPUT, but the pin voltage
                     is gently "pulled up" towards +3.3v. This means
                     the state will be high unless an external device
                     is specifically pulling the pin down to ground,
                     in which case the "gentle" pull up will not
                     affect the state of the input. */

    INPUT_PULLDOWN, /**< The state of the pin in this mode is reported
                       the same way as with INPUT, but the pin voltage
                       is gently "pulled down" towards 0v. This means
                       the state will be low unless an external device
                       is specifically pulling the pin up to 3.3v, in
                       which case the "gentle" pull down will not
                       affect the state of the input. */

    INPUT_FLOATING, /**< Synonym for INPUT. */

    PWM, /**< This is a special mode for when the pin will be used for
            PWM output (a special case of digital output). */

    PWM_OPEN_DRAIN, /**< Like PWM, except that instead of alternating
                       cycles of LOW and HIGH, the voltage on the pin
                       consists of alternating cycles of LOW and
                       floating (disconnected). */
} WiringPinMode;

/**
 * Configure behavior of a GPIO pin.
 *
 * @param pin Number of pin to configure.
 * @param mode Mode corresponding to desired pin behavior.
 * @see WiringPinMode
 */
inline void pinMode(uint8 pin, WiringPinMode mode)
{}

#endif /* ARDUINO_H_ */

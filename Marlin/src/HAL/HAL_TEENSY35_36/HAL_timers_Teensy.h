/* **************************************************************************

 Marlin 3D Printer Firmware
 Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 Copyright (c) 2016 Bob Cousins bobcousins42@googlemail.com
 Copyright (c) 2015-2016 Nico Tonnhofer wurstnase.reprap@gmail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

/**
 * Description: HAL for
 * Teensy3.5 (__MK64FX512__)
 * Teensy3.6 (__MK66FX1M0__)
 */

#ifndef _HAL_TIMERS_TEENSY_H
#define _HAL_TIMERS_TEENSY_H

// --------------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------------

#include <stdint.h>

// --------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------

#define FORCE_INLINE __attribute__((always_inline)) inline

typedef uint32_t hal_timer_t;
#define HAL_TIMER_TYPE_MAX 0xFFFFFFFF

#define STEP_TIMER_NUM 0
#define TEMP_TIMER_NUM 1

#define FTM0_TIMER_PRESCALE 8
#define FTM1_TIMER_PRESCALE 4
#define FTM0_TIMER_PRESCALE_BITS 0b011
#define FTM1_TIMER_PRESCALE_BITS 0b010

#define FTM0_TIMER_RATE (F_BUS / FTM0_TIMER_PRESCALE) // 60MHz / 8 = 7500kHz
#define FTM1_TIMER_RATE (F_BUS / FTM1_TIMER_PRESCALE) // 60MHz / 4 = 15MHz

#define STEPPER_TIMER STEP_TIMER_NUM // Alias?
#define STEPPER_TIMER_PRESCALE (CYCLES_PER_MICROSECOND / HAL_TICKS_PER_US)

#define PULSE_TIMER_NUM STEP_TIMER_NUM
#define PULSE_TIMER_PRESCALE STEPPER_TIMER_PRESCALE

#define HAL_TIMER_RATE         (FTM0_TIMER_RATE)
#define HAL_STEPPER_TIMER_RATE HAL_TIMER_RATE
#define HAL_TICKS_PER_US       ((HAL_STEPPER_TIMER_RATE) / 1000000)

#define TEMP_TIMER_FREQUENCY   1000

#define STEP_TIMER_MIN_INTERVAL   8 // minimum time in µs between stepper interrupts

#define ENABLE_STEPPER_DRIVER_INTERRUPT() HAL_timer_enable_interrupt(STEP_TIMER_NUM)
#define DISABLE_STEPPER_DRIVER_INTERRUPT() HAL_timer_disable_interrupt(STEP_TIMER_NUM)
#define STEPPER_ISR_ENABLED() HAL_timer_interrupt_enabled(STEP_TIMER_NUM)

#define ENABLE_TEMPERATURE_INTERRUPT() HAL_timer_enable_interrupt(TEMP_TIMER_NUM)
#define DISABLE_TEMPERATURE_INTERRUPT() HAL_timer_disable_interrupt(TEMP_TIMER_NUM)

#define HAL_STEP_TIMER_ISR  extern "C" void ftm0_isr(void) //void TC3_Handler()
#define HAL_TEMP_TIMER_ISR  extern "C" void ftm1_isr(void) //void TC4_Handler()

#define HAL_ENABLE_ISRs() do { if (thermalManager.in_temp_isr) DISABLE_TEMPERATURE_INTERRUPT(); else ENABLE_TEMPERATURE_INTERRUPT(); ENABLE_STEPPER_DRIVER_INTERRUPT(); } while(0)

void HAL_timer_start(const uint8_t timer_num, const uint32_t frequency);

FORCE_INLINE static void HAL_timer_set_compare(const uint8_t timer_num, const hal_timer_t compare) {
  switch (timer_num) {
    case 0: FTM0_C0V = compare; break;
    case 1: FTM1_C0V = compare; break;
  }
}

FORCE_INLINE static hal_timer_t HAL_timer_get_compare(const uint8_t timer_num) {
  switch (timer_num) {
    case 0: return FTM0_C0V;
    case 1: return FTM1_C0V;
  }
  return 0;
}

FORCE_INLINE static hal_timer_t HAL_timer_get_count(const uint8_t timer_num) {
  switch (timer_num) {
    case 0: return FTM0_CNT;
    case 1: return FTM1_CNT;
  }
  return 0;
}

FORCE_INLINE static void HAL_timer_restrain(const uint8_t timer_num, const uint16_t interval_ticks) {
  const hal_timer_t mincmp = HAL_timer_get_count(timer_num) + interval_ticks;
  if (HAL_timer_get_compare(timer_num) < mincmp) HAL_timer_set_compare(timer_num, mincmp);
}

void HAL_timer_enable_interrupt(const uint8_t timer_num);
void HAL_timer_disable_interrupt(const uint8_t timer_num);
bool HAL_timer_interrupt_enabled(const uint8_t timer_num);

void HAL_timer_isr_prologue(const uint8_t timer_num);

#endif // _HAL_TIMERS_TEENSY_H

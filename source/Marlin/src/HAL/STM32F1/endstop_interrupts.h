/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (c) 2017 Victor Perez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

/**
 * Endstop interrupts for Libmaple STM32F1 based targets.
 *
 * On STM32F, all pins support external interrupt capability.
 * Any pin can be used for external interrupts, but there are some restrictions.
 * At most 16 different external interrupts can be used at one time.
 * Further, you can’t just pick any 16 pins to use. This is because every pin on the STM32
 * connects to what is called an EXTI line, and only one pin per EXTI line can be used for external interrupts at a time
 * Check the Reference Manual of the MCU to confirm which line is used by each pin
 */

/**
 * Endstop Interrupts
 *
 * Without endstop interrupts the endstop pins must be polled continually in
 * the temperature-ISR via endstops.update(), most of the time finding no change.
 * With this feature endstops.update() is called only when we know that at
 * least one endstop has changed state, saving valuable CPU cycles.
 *
 * This feature only works when all used endstop pins can generate an 'external interrupt'.
 *
 * Test whether pins issue interrupts on your board by flashing 'pin_interrupt_test.ino'.
 * (Located in Marlin/buildroot/share/pin_interrupt_test/pin_interrupt_test.ino)
 */

#include "../../module/endstops.h"

// One ISR for all EXT-Interrupts
inline void endstop_ISR() { endstops.update(); }

inline void setup_endstop_interrupts() {
  #if HAS_X_MIN
    attachInterrupt(X_MIN_PIN, ExtInt_X_MIN_Callback, 0, CHANGE);
  #endif
    #if HAS_X_MAX
    attachInterrupt(X_MAX_PIN, ExtInt_X_MAX_Callback, 1, CHANGE);
    #endif
  #if HAS_Y_MIN
    attachInterrupt(Y_MIN_PIN, ExtInt_Y_MIN_Callback, 2, CHANGE);
  #endif
  #if HAS_Z_MIN
    attachInterrupt(Z_MIN_PIN, ExtInt_Z_MIN_Callback, 3, CHANGE);
  #endif
  #if HAS_Z2_MIN
    attachInterrupt(Z_MAX_PIN, ExtInt_Z_MAX_Callback, 4, CHANGE);
  #endif
}

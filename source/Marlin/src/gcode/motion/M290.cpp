/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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

#include "../../inc/MarlinConfig.h"

#if ENABLED(BABYSTEPPING)

#include "../gcode.h"
#include "../../feature/babystep.h"
#include "../../module/probe.h"
#include "../../module/planner.h"

#if ENABLED(BABYSTEP_ZPROBE_OFFSET)
  #include "../../core/serial.h"
#endif

#if ENABLED(MESH_BED_LEVELING)
  #include "../../feature/bedlevel/bedlevel.h"
#endif

#if ENABLED(BABYSTEP_ZPROBE_OFFSET)

  FORCE_INLINE void mod_probe_offset(const_float_t offs) {
    if (TERN1(BABYSTEP_HOTEND_Z_OFFSET, active_extruder == 0)) {
      probe.offset.z += offs;
      SERIAL_ECHO_MSG(STR_PROBE_OFFSET " " STR_Z, probe.offset.z);
    }
    else {
      #if ENABLED(BABYSTEP_HOTEND_Z_OFFSET)
        hotend_offset[active_extruder].z -= offs;
        SERIAL_ECHO_MSG(STR_PROBE_OFFSET STR_Z ": ", hotend_offset[active_extruder].z);
      #endif
    }
  }

#endif



/**
 * M290: Babystepping
 *
 * Send 'R' or no parameters for a report.
 *
 *  X<linear> - Distance to step X
 *  Y<linear> - Distance to step Y
 *  Z<linear> - Distance to step Z
 *  S<linear> - Distance to step Z (alias for Z)
 *
 * With BABYSTEP_ZPROBE_OFFSET:
 *  P0 - Don't adjust the Z probe offset
 */
void GcodeSuite::M290() {
  #if ENABLED(BABYSTEP_XY)
    LOOP_XYZ(a)
      if (parser.seenval(XYZ_CHAR(a)) || (a == Z_AXIS && parser.seenval('S'))) {
        const float offs = constrain(parser.value_axis_units((AxisEnum)a), -2, 2);
        babystep.add_mm((AxisEnum)a, offs);
        #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
          if (a == Z_AXIS && (!parser.seen('P') || parser.value_bool())) mod_probe_offset(offs);
        #endif
      }
  #else
    if (parser.seenval('Z') || parser.seenval('S')) {
      const float offs = constrain(parser.value_axis_units(Z_AXIS), -2, 2);
      
      babystep.add_mm(Z_AXIS, offs);
      #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
        if (!parser.seen('P') || parser.value_bool()) mod_probe_offset(offs);
      #endif

      #if ENABLED(TENLOG_TOUCH_LCD)
        char cmd[32];
        tl_Z_HOME_POS -= offs;
        int32_t zoffset = tl_Z_HOME_POS * 1000;
        sprintf_P(cmd, PSTR("setting.xZOffset.val=%d"), -zoffset);
        TLSTJC_println(cmd);
        sprintf_P(cmd, PSTR("babystep.xBaby.val=%d"), -zoffset);
        TLSTJC_println(cmd);
        EXECUTE_GCODE(PSTR("M500"));
      #endif
    }
  #endif

  if (!parser.seen("XYZ") || parser.seen('R')) {
    SERIAL_ECHO_START();

    #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
      SERIAL_ECHOLNPAIR(STR_PROBE_OFFSET " " STR_Z, probe.offset.z);
    #endif

    #if ENABLED(BABYSTEP_HOTEND_Z_OFFSET)
    {
      SERIAL_ECHOLNPAIR_P(
        PSTR("Hotend "), active_extruder
        #if ENABLED(BABYSTEP_XY)
          , PSTR("Offset X"), hotend_offset[active_extruder].x
          , SP_Y_STR, hotend_offset[active_extruder].y
          , SP_Z_STR
        #else
          , PSTR("Offset Z")
        #endif
        , hotend_offset[active_extruder].z
      );
    }
    #endif

    #if ENABLED(MESH_BED_LEVELING)
      SERIAL_ECHOLNPAIR("MBL Adjust Z", mbl.z_offset);
    #endif

    #if ENABLED(BABYSTEP_DISPLAY_TOTAL)
    {
      SERIAL_ECHOLNPAIR_P(
        #if ENABLED(BABYSTEP_XY)
            PSTR("Babystep X"), babystep.axis_total[X_AXIS]
          , SP_Y_STR, babystep.axis_total[Y_AXIS]
          , SP_Z_STR
        #else
          PSTR("Babystep Z")
        #endif
        , babystep.axis_total[BS_TOTAL_IND(Z_AXIS)]
      );
    }
    #endif
  }
}

#endif // BABYSTEPPING

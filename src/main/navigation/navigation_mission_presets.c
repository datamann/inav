/*
 * This file is part of INAV Project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public License Version 3, as described below:
 *
 * This file is free software: you may copy, redistribute and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "platform.h"

#include "build/debug.h"

#include "common/axis.h"
#include "common/filter.h"
#include "common/maths.h"
#include "common/utils.h"

#include "config/parameter_group.h"
#include "config/parameter_group_ids.h"

#include "drivers/time.h"

#include "fc/rc_modes.h"
#include "fc/runtime_config.h"

#include "io/beeper.h"

#include "navigation/navigation.h"
#include "navigation/navigation_private.h"
#include "navigation/navigation_mission_presets.h"

#if defined(USE_NAV_MISSION_PRESETS)

PG_REGISTER_WITH_RESET_TEMPLATE(missionPresetsConfig_t, missionPresetsConfig, PG_MISSION_PRESETS_CONFIG, 0);

PG_RESET_TEMPLATE(missionPresetsConfig_t, missionPresetsConfig,
    .missionPresetType = {
        [0] = MISSION_PRESET_VECTOR,
        [1] = MISSION_PRESET_VECTOR,
        [2] = MISSION_PRESET_VECTOR
    },

    .missionAltitudeCm = 1000,
    .missionDistanceCm = 5000,
);

static bool canUpdateMissionFromPreset = false;

static void setWaypointRelative(int wpNumber, navWaypointActions_e action, navWaypointFlags_e flag, float x, float y, float z)
{
    navWaypoint_t wpData;
    gpsLocation_t wpLLH;
    fpVector3_t wpLocal;

    wpData.action = (uint8_t)action;
    wpData.flag = (uint8_t)flag;
    wpData.p1 = 0;
    wpData.p2 = 0;
    wpData.p3 = 0;

    if (action == NAV_WP_ACTION_RTH) {
        wpData.lat = 0;
        wpData.lon = 0;
        wpData.alt = 0;
    }
    else {
        // Prepare local coordinates
        wpLocal.x = posControl.actualState.pos.x + x;
        wpLocal.y = posControl.actualState.pos.y + y;
        wpLocal.z = posControl.actualState.pos.z + z;

        // Convert to LLH
        geoConvertLocalToGeodetic(&posControl.gpsOrigin, &wpLocal, &wpLLH);

        // Prepare waypoint data
        wpData.lat = wpLLH.lat;
        wpData.lon = wpLLH.lon;
        wpData.alt = wpLLH.alt;
    }

    // Update waypoint
    posControl.waypointList[wpNumber - 1] = wpData;
    posControl.waypointCount = wpNumber;
    posControl.waypointListValid = (wpData.flag == NAV_WP_FLAG_LAST);
}

static bool loadMissionPresetVectir(void)
{
    // Reset waypoint list
    resetWaypointList();

    // Climb to current position 
    setWaypointRelative(1, NAV_WP_ACTION_WAYPOINT, NAV_WP_FLAG_NONE, 0, 0, missionPresetsConfig()->missionAltitudeCm);

    setWaypointRelative(2, NAV_WP_ACTION_WAYPOINT, NAV_WP_FLAG_NONE, 
                           missionPresetsConfig()->missionDistanceCm * posControl.actualState.cosYaw - missionPresetsConfig()->missionDistanceCm * posControl.actualState.sinYaw,
                           missionPresetsConfig()->missionDistanceCm * posControl.actualState.cosYaw - missionPresetsConfig()->missionDistanceCm * posControl.actualState.sinYaw,
                           missionPresetsConfig()->missionAltitudeCm);

    setWaypointRelative(3, NAV_WP_ACTION_RTH, NAV_WP_FLAG_LAST, 0, 0, 0);

    return true;
}

static void loadMissionPreset(missionPresetType_e type)
{
    bool result = false;

    switch (type) {
        case MISSION_PRESET_VECTOR:
            result = loadMissionPresetVectir();
            break;
        default:
            // Unknown type, do nothing
            break;
    }

    // Beep out operation status
    beeper(result ? BEEPER_ACTION_SUCCESS : BEEPER_ACTION_FAIL);
}

void updateMissionPresets(void)
{
    // Can't change mission preset when armed
    if (ARMING_FLAG(ARMED)) {
        return;
    }

    if (IS_RC_MODE_ACTIVE(BOXMISSION1) || IS_RC_MODE_ACTIVE(BOXMISSION2) || IS_RC_MODE_ACTIVE(BOXMISSION3)) {
        if (canUpdateMissionFromPreset) {
            if (IS_RC_MODE_ACTIVE(BOXMISSION1)) {
                // Mission preset 1
                loadMissionPreset((missionPresetType_e)missionPresetsConfig()->missionPresetType[0]);
            } 
            else if (IS_RC_MODE_ACTIVE(BOXMISSION2)) {
                // Mission preset 2
                loadMissionPreset((missionPresetType_e)missionPresetsConfig()->missionPresetType[1]);
            }
            else if (IS_RC_MODE_ACTIVE(BOXMISSION3)) {
                // Mission preset 3
                loadMissionPreset((missionPresetType_e)missionPresetsConfig()->missionPresetType[2]);
            }

            // Reset flag
            canUpdateMissionFromPreset = false;
        }
    }
    else {
        // None of the mission preset modes is enabled, now we can check the prerequisites for mission presets
        canUpdateMissionFromPreset = canActivateAltHoldMode()           // Have altitude sensor
                                  && canActivatePosHoldMode()           // Have Position/Heading sensor and it's valid and locked
                                  && posEstimationHasGlobalReference()  // Have global position data (GPS)
                                  && posControl.gpsOrigin.valid;        // Have valid GPS origin
    }

}
















#endif

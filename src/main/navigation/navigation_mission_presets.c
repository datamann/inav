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

#if defined(USE_NAV_MISSION_PRESETS)

static bool canUpdateMissionFromPreset = false;

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
            } 
            else if (IS_RC_MODE_ACTIVE(BOXMISSION2)) {
                // Mission preset 2
            }
            else if (IS_RC_MODE_ACTIVE(BOXMISSION3)) {
                // Mission preset 3
            }

            // Reset flag
            canUpdateMissionFromPreset = false;
        }
    }
    else {
        // None of the mission preset modes is enabled, now we can check the prerequisites for mission presets
        canUpdateMissionFromPreset = canActivateAltHoldMode()           // Have altitude sensor
                                  && canActivatePosHoldMode()           // Have Position/Heading sensor and it's valid and locked
                                  && posEstimationHasGlobalReference(); // Have global position data (GPS)
    }

}
















#endif

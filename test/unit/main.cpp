/*
 * Copyright (c) 2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed-drivers/mbed.h"

#include "wrd-rgb-led/RGBLEDImplementation.h"

#if (YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_RGB_LED_PRESENT == 0)
#error Platform not supported
#endif

/*****************************************************************************/
/* App start                                                                 */
/*****************************************************************************/

RGBLEDImplementation rgb;

uint8_t value = 0;
uint8_t color = 0;
bool up = true;

void updateTask(void)
{
    switch (color & 0x03)
    {
        case 0:
                rgb.set(value, 0, 0);
                break;
        case 1:
                rgb.set(0, value, 0);
                break;
        case 2:
                rgb.set(0, 0, value);
                break;
        case 3:
                rgb.set(value, value, value);
                break;
    }

    if (up)
    {
        if (value == 0xFF)
        {
            up = false;
        }
        else
        {
            value++;
        }
    }
    else
    {
        if (value == 0x01)
        {
            up = true;
            color++;
        }
        else
        {
            value--;
        }
    }
}

void app_start(int, char *[])
{
    minar::Scheduler::postCallback(updateTask)
        .period(minar::milliseconds(10))
        .tolerance(1);
}

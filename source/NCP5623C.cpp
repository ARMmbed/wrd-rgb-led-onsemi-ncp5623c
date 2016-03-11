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

#include "wrd-rgb-led/NCP5623C.h"

NCP5623C::NCP5623C(PinName sda, PinName scl)
    : i2c(sda, scl),
      state(STATE_IDLE)
{
    i2c.frequency(400000);
}

void NCP5623C::set(uint8_t new_red,
                   uint8_t new_green,
                   uint8_t new_blue,
                   FunctionPointer0<void> callback)
{
    /* For each color, round up values between 0 and 8 to lowest value. */
    if (new_red > 0)
    {
        red = (new_red < 9) ? 1 : (new_red >> 3) & 0x1F;
    }
    else
    {
        red = 0;
    }

    if (new_green > 0)
    {
        green = (new_green < 9) ? 1 : (new_green >> 3) & 0x1F;
    }
    else
    {
        green = 0;
    }

    if (new_blue > 0)
    {
        blue = (new_blue < 9) ? 1 : (new_blue >> 3) & 0x1F;
    }
    else
    {
        blue = 0;
    }

    setDoneHandle = callback;

    state = STATE_SET_RED;

    i2c.write(PRIMARY_ADDRESS,
              SET_CURRENT | 0x01,
              &memoryWrite, 0,
              this, &NCP5623C::writeDone);
}

void NCP5623C::writeDone(void)
{
    /* Use state machine to set each color sequentially.
       Call set done handler when last color has been set.
    */
    switch(state)
    {
        case STATE_SET_RED:
                        i2c.write(PRIMARY_ADDRESS,
                                  PWM1_CONTROL | red,
                                  &memoryWrite, 0,
                                  this, &NCP5623C::writeDone);

                        state = STATE_SET_GREEN;
                        break;

        case STATE_SET_GREEN:
                        i2c.write(PRIMARY_ADDRESS,
                                  PWM2_CONTROL | green,
                                  &memoryWrite, 0,
                                  this, &NCP5623C::writeDone);
                        state = STATE_SET_BLUE;
                        break;

        case STATE_SET_BLUE:
                        i2c.write(PRIMARY_ADDRESS,
                                  PWM3_CONTROL | blue,
                                  &memoryWrite, 0,
                                  this, &NCP5623C::writeDone);
                        state = STATE_SET_DONE;
                        break;

        case STATE_SET_DONE:
                        if (setDoneHandle)
                        {
                            minar::Scheduler::postCallback(setDoneHandle);
                        }
                        state = STATE_IDLE;
                        break;
        case STATE_IDLE:
        default:
                        break;
    }
}

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
        red = (new_red < 9) ? 1 : new_red >> 3;
    }
    else
    {
        red = 0;
    }

    if (new_green > 0)
    {
        green = (new_green < 9) ? 1 : new_green >> 3;
    }
    else
    {
        green = 0;
    }

    if (new_blue > 0)
    {
        blue = (new_blue < 9) ? 1 : new_blue >> 3;
    }
    else
    {
        blue = 0;
    }

    setDoneHandle = callback;

    state = STATE_SET_RED;

    setRegister(NCP5623C::SET_CURRENT, 0x01);
}

int NCP5623C::setRegister(register_t reg, uint8_t value)
{
    memoryWrite = reg | (value & 0x1F);

    I2C::event_callback_t fp(this, &NCP5623C::setRegisterDone);

    return i2c.transfer(PRIMARY_ADDRESS, &memoryWrite, 1, &memoryRead, 0, fp);
}

void NCP5623C::setRegisterDone(Buffer txBuffer, Buffer rxBuffer, int code)
{
    (void) txBuffer;
    (void) rxBuffer;
    (void) code;

    /* Use state machine to set each color sequentially.
       Call set done handler when last color has been set.
    */
    switch(state)
    {
        case STATE_SET_RED:
                        setRegister(NCP5623C::PWM1_CONTROL, red);
                        state = STATE_SET_GREEN;
                        break;

        case STATE_SET_GREEN:
                        setRegister(NCP5623C::PWM2_CONTROL, green);
                        state = STATE_SET_BLUE;
                        break;

        case STATE_SET_BLUE:
                        setRegister(NCP5623C::PWM3_CONTROL, blue);
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

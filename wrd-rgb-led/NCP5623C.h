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

#ifndef __ONSEMI_NCP5623C_H__
#define __ONSEMI_NCP5623C_H__

#include "mbed-drivers/mbed.h"

using namespace mbed::util;

class NCP5623C {
public:
    typedef enum {
        PRIMARY_ADDRESS = 0x72
    } address_t;

    typedef enum {
        SHUTDOWN      = 0,
        SET_CURRENT   = 1 << 5,
        PWM1_CONTROL  = 2 << 5,
        PWM2_CONTROL  = 3 << 5,
        PWM3_CONTROL  = 4 << 5,
        UPWARD_LEND   = 5 << 5,
        DOWNWARD_LEND = 6 << 5,
        DIMMING_STEPS = 7 << 5
    } register_t;

    /**
     * @brief Constructor. RGB driver for the NCP5623C.
     *
     * @param sda I2C data line.
     * @param scl I2C clock line.
     */
    NCP5623C(PinName sda, PinName scl);

    /**
     * @brief Set RGB color.
     * @details The NCP5623C only has 31 distinct levels for each power setting.
     *          This driver keeps the power level constant and ignores the lowest
     *          3 bit of the 8 bit input. Value between 1-8 will be rounded up to
     *          the lowest color setting, because a user might want to put in 1 to
     *          get the lowest color possible.
     *
     * @param red uint8_t red intensity.
     * @param green uint8_t green intensity.
     * @param blue uint8_t blue intensity.
     * @param callback FunctionPointer callback for when setting has been received.
     */
    void set(uint8_t red, uint8_t green, uint8_t blue, FunctionPointer0<void> callback);

private:
    int setRegister(register_t reg, uint8_t value);
    void setRegisterDone(Buffer txBuffer, Buffer rxBuffer, int code);

    typedef enum {
        STATE_IDLE,
        STATE_SET_RED,
        STATE_SET_GREEN,
        STATE_SET_BLUE,
        STATE_SET_DONE
    } state_t;

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    FunctionPointer0<void> setDoneHandle;

    I2C i2c;
    char memoryWrite;
    char memoryRead;

    state_t state;
};
#endif // __ONSEMI_NCP5623C_H__

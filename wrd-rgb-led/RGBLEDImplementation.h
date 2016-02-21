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

#ifndef __WRD_RGB_LED_ONSEMI_H__
#define __WRD_RGB_LED_ONSEMI_H__

#include "wrd-rgb-led/NCP5623C.h"
#include "gpio-pcal64/PCAL64.h"

class RGBLEDImplementation
{
public:
    /**
     * @brief Constructor. Implementation of the WRD RGB API.
     * @details Pin configurations are pulled from Yotta config.
     */
    RGBLEDImplementation()
        :   led(YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_RGB_LED_I2C_NAME),
            ioexpander(YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_EXTERNAL_GPIO_I2C_NAME,
                       PCAL64::SECONDARY_ADDRESS,
                       YOTTA_CFG_HARDWARE_WEARABLE_REFERENCE_DESIGN_EXTERNAL_GPIO_PIN_IRQ2),
            red(0),
            green(0),
            blue(0)
    { }

    /**
     * @brief Set RGB color.
     *
     * @param red uint8_t red intensity.
     * @param green uint8_t green intensity.
     * @param blue uint8_t blue intensity.
     * @param callback FunctionPointer callback for when setting has been received.
     */
    void set(uint8_t new_red,
             uint8_t new_green,
             uint8_t new_blue,
             FunctionPointer0<void> callback = (void(*)(void)) NULL)
    {
        /* If all colors are zero, turn off RGB using the I/O expander. */
        if ((new_red == 0) && (new_green == 0) && (new_blue == 0))
        {
            ioexpander.set(PCAL64::P0_0, PCAL64::Output)
                      .set(PCAL64::P0_0, PCAL64::Low)
                      .callback(callback);
        }
        /* If the RGB was off before, turn on RGB before sending colors. */
        else if ((red == 0) && (green == 0) && (blue == 0))
        {
            FunctionPointer0<void> fp(this, &RGBLEDImplementation::internalDelay);

            ioexpander.set(PCAL64::P0_0, PCAL64::Output)
                      .set(PCAL64::P0_0, PCAL64::High)
                      .callback(fp);
        }
        /* RGB is already on, set new colors. */
        else
        {
            internalSet();
        }

        red = new_red;
        green = new_green;
        blue = new_blue;
        setDoneHandle = callback;
    }

private:
    void internalSet(void)
    {
        led.set(red, green, blue, setDoneHandle);
    }

    void internalSetDone(void)
    {
        if (setDoneHandle)
        {
            setDoneHandle.call();
            setDoneHandle.clear();
        }
    }

    /**
     * @brief Delay sending commands to the RGB chip.
     * @details Load switch ramp up time is 1.5 ms. RGB ramp up is 0.15 ms.
     */
    void internalDelay(void)
    {
        FunctionPointer0<void> fp(this, &RGBLEDImplementation::internalSet);

        minar::Scheduler::postCallback(fp)
            .delay(minar::milliseconds(3))
            .tolerance(1);
    }

    NCP5623C led;
    PCAL64 ioexpander;

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    FunctionPointer0<void> setDoneHandle;
};

#endif // __WRD_RGB_LED_ONSEMI_H__

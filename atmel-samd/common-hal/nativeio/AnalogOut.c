/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>

#include "py/runtime.h"

#include "shared-bindings/nativeio/AnalogOut.h"

#include "asf/sam0/drivers/dac/dac.h"

void common_hal_nativeio_analogout_construct(nativeio_analogout_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (pin->pin != PIN_PA02) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
            "AnalogOut only supported on pin PA02."));
        return;
    }
    struct dac_config config_dac;
    dac_get_config_defaults(&config_dac);
    config_dac.reference = DAC_REFERENCE_AVCC;
    enum status_code status = dac_init(&self->dac_instance, DAC, &config_dac);
    if (status != STATUS_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "DAC init failed."));
        return;
    }

    struct dac_chan_config config_analogout_chan;
    dac_chan_get_config_defaults(&config_analogout_chan);
    dac_chan_set_config(&self->dac_instance, DAC_CHANNEL_0, &config_analogout_chan);
    dac_chan_enable(&self->dac_instance, DAC_CHANNEL_0);

    dac_enable(&self->dac_instance);
}

void common_hal_nativeio_analogout_deinit(nativeio_analogout_obj_t *self) {
    dac_disable(&self->dac_instance);
    dac_chan_disable(&self->dac_instance, DAC_CHANNEL_0);
}

void common_hal_nativeio_analogout_set_value(nativeio_analogout_obj_t *self,
        uint16_t value) {
    dac_chan_write(&self->dac_instance, DAC_CHANNEL_0, value);
}

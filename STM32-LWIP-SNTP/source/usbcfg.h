/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Modified Jean-Michel 01/06/2019

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef USBCFG_H
#define USBCFG_H

#include "portab.h"
#include "chprintf.h"

extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver PORTAB_SDU1;

// for printing to console
#define CONSOLE_PRINTF(...) chprintf((BaseSequentialStream *) &PORTAB_SDU1, __VA_ARGS__ )

// to print messages for debugging
// #define DEBUG_PRINTF(...)
#define DEBUG_PRINTF(...) CONSOLE_PRINTF( __VA_ARGS__ )
#define DEBUG_ASSERT(x,...) do {if(!(x)) { DEBUG_PRINTF( __VA_ARGS__ );}} while(0)

#endif  /* USBCFG_H */

/** @} */

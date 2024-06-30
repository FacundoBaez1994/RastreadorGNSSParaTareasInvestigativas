/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"

#define BLINKING_RATE  500



nonBlockingDelay delayLatency (BLINKING_RATE );
char StringToSend [15] = "Hola Mundo!\n";
UnbufferedSerial uartUsb(USBTX, USBRX, 115200 ); // debug only

int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);
    DigitalOut led2 (PB_4);

    while (true) {
        if( delayLatency.read()) {
            led = !led;
            led2 = ! led2;
            uartUsb.write (StringToSend, strlen (StringToSend));  // debug only
            uartUsb.write ( "\r\n",  3 );  // debug only
        }
    }
}






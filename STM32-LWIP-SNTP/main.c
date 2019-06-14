/*
    Ntp client using ChibiOS, lwip and sntp
    Copyright (C) 2019 Jean-Michel Gallego

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "usbcfg.h"

#include "lwipthread.h"
#include "lwip/netif.h"
#include "lwip/apps/sntp.h"

//===========================================================================
// Green LED blinker thread
//===========================================================================

static THD_WORKING_AREA(waThread1, 128);

static THD_FUNCTION( Thread1, arg )
{
  (void) arg;

  chRegSetThreadName( "blinker" );
  while( true )
  {
    palClearLine( PORTAB_BLINK_LED1 );
    chThdSleepMilliseconds( 100 );     // times are in milliseconds.
    palSetLine( PORTAB_BLINK_LED1 );
    chThdSleepMilliseconds( 900 );
  }
}

//===========================================================================
// Lwip related.
//===========================================================================

const uint8_t localMACAddress[6] = { 0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x45 };

// local IP defined by DHCP
static struct lwipthread_opts netOptions = { (uint8_t *) localMACAddress,
                                             0, 0, 0, NET_ADDRESS_DHCP, NULL };

// static local IP
/*
const uint8_t localMACAddress[6] = { 0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46 };
static struct lwipthread_opts netOptions = { (uint8_t *) localMACAddress,
                                             IP4_ADDR_VALUE( 192, 168, 1, 20 ),
                                             IP4_ADDR_VALUE( 255, 255, 255, 0 ),
                                             IP4_ADDR_VALUE( 192, 168, 1, 1 ),
                                             NET_ADDRESS_STATIC, NULL };
*/

uint32_t getLocalIp( void )
{
  if( netif_default != NULL )
    return netif_default->ip_addr.addr;
  return 0;
}

/*===========================================================================*/
/* Main and generic code.                                                    */
/*===========================================================================*/

// integer to store time in seconds since 1980
//  updated by sntp
//  incremented every seconds by main() thread activity
uint32_t systemTime = 0;

//
// Application entry point.
//

int main( void )
{
  uint32_t   localIp = 0;
  systime_t  now;
  time_t     tim;
  struct tm  stm;
  char       str[20];

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   * - lwIP subsystem initialization using configuration defined in
   *   netOptions structure.
   */
  halInit();
  chSysInit();
  lwipInit( & netOptions );

  // Target-dependent setup code.
  portab_setup();

  // Initializes a serial-over-USB CDC driver.
  sduObjectInit( &PORTAB_SDU1 );
  sduStart( &PORTAB_SDU1, &serusbcfg );

  // Activates the USB driver and then the USB bus pull-up on D+.
  // Delays are inserted in order to not have to disconnect the cable after a reset.
  usbDisconnectBus( serusbcfg.usbp );
  chThdSleepMilliseconds( 1500 );
  usbStart( serusbcfg.usbp, &usbcfg );
  usbConnectBus( serusbcfg.usbp );
  chThdSleepMilliseconds( 1500 );

  // Creates the blinker thread.
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  // Waiting for obtaining an IP
  systime_t millis = chVTGetSystemTimeX();
  while( localIp == 0 )
  {
    millis += TIME_MS2I( 1000 );

    now = chVTGetSystemTimeX();
    DEBUG_PRINTF( "Waiting for local IP %d ms\r", TIME_I2MS( now ));

    localIp = netif_default->ip_addr.addr;

    chThdSleepUntil( millis );
  }
  DEBUG_PRINTF( "\r\nLocal IP is %d.%d.%d.%d\r\n",
                localIp & 0xff, ( localIp >> 8 ) & 0xff,
                ( localIp >> 16 ) & 0xff, ( localIp >> 24 ) & 0xff );

  // 3 ways to define NTP servers
  // ( LWIP_DHCP_MAX_NTP_SERVERS must be >= 3 in lwipopts.h):
  //  by numerical IP
  ip_addr_t serverAddr;
  serverAddr.addr = IP4_ADDR_VALUE( 212, 83, 145, 32 );
  sntp_setserver( 0, &serverAddr );
  //  IP as a string
  sntp_setservername( 1, "51.255.141.154" );
  //  by name
  sntp_setservername( 2, "fr.pool.ntp.org" );

  // Starts the SNTP client.
  sntp_init();

  // Normal main() thread activity.
  DEBUG_PRINTF( "Starting main loop\r\n" );

  while( true )
  {
    millis += TIME_MS2I( 1000 );

    tim = systemTime;
    gmtime_r( &tim, &stm );
    chsnprintf( str, 20, "%02u/%02u/%04u %02u:%02u:%02u",
                stm.tm_mday, stm.tm_mon + 1, stm.tm_year + 1900,
                stm.tm_hour, stm.tm_min, stm.tm_sec );
    DEBUG_PRINTF( "UTC time: %s\r", str );
    systemTime ++;

    chThdSleepUntil( millis );
  }
}

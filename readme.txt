*****************************************************************************
**          Demo of Ntp client using ChibiOS, lwip and sntp                **
**            ChibiOS/RT port for ARM-Cortex-M4 STM32F407.                 **
*****************************************************************************

** Target **

The demo has been tested on an Olimex STM32-E407 board.
Compiled but not tested for Goldbull STM32F107, Discovery STM32F746 and
 Discovery STM32F769

** The Demo **

The demo currently just flashes a LED using a thread and make periodical
requests to NTP servers.
The USB-FS port is used as USB-CDC for sending message to terminal.

** Build Procedure **

The demo has been tested by using the free Codesourcery GCC-based toolchain
and YAGARTO.
ChibiStudio 21
ChibiOS 19.1.2

You must modify in chibios191/ext/lwip:
 - in directory chibios191\ext\lwip\src\include\lwip\apps , rename or
    eliminate sntp_opts.h as it is moved to directory cfg of project
 - in file chibios191\ext\lwip\src\core\dns.c , line 100, replace
    #if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_XID) != 0)
    with
    #if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_XID) != 0) && defined(LWIP_RAND)
 - in file C:\ChibiStudio\chibios191\os\various\lwip_bindings\lwip.mk, line 13,
    replace
    LWSRC = $(COREFILES) $(CORE4FILES) $(APIFILES) $(LWBINDSRC) $(NETIFFILES) $(HTTPDFILES)
    with
    LWSRC = $(COREFILES) $(CORE4FILES) $(APIFILES) $(LWBINDSRC) $(NETIFFILES) $(SNTPFILES)

See cfg\lwipopts.h and cfg\sntp_opts.h for specific options for this demo
See comments in main.c

** Notes **

Some files used by the demo are not part of ChibiOS/RT but are copyright of
ST Microelectronics and are licensed under a different license.
Also note that not all the files present in the ST library are distributed
with ChibiOS/RT, you can find the whole library on the ST web site:

                             http://www.st.com

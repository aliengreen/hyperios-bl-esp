/*
 *	main.c
 *
 *  Copyright (c) 2018 Alien Green LLC
 *
 *  This file is part of Hyperios.
 *
 *  Hyperios is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Hyperios is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Hyperios.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <avr/interrupt.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "config.h"
#include "hos_util.h"

uint8_t buffer[SPM_PAGESIZE];
uint16_t crc = 0xFFFF;

static FILE mystdout = FDEV_SETUP_STREAM(HosSerialTX, HosSerialRX, _FDEV_SETUP_RW);

//------------------------------------------------------------------------------

uint16_t HosESP8266NeedUpdate(uint8_t force)
{
    uint8_t tmp[sizeof(uint16_t)];
    uint16_t rem_crc = 0;
    uint32_t i = 0;

    printf_P(PSTR("FCHK=%d\r\n"), force);

    /* Wait for 0xFD to receive checksum */
    wdt_enable(WDTO_8S);
    while(getchar() != 0xFD);
    // wdt_disable();

    /* Receive Checksum */
    for(uint16_t i = 0; i < sizeof(uint16_t); i++) {
        tmp[i] = getchar();
    }

    memcpy(&rem_crc, tmp, sizeof(uint16_t));

    return rem_crc;
}

//------------------------------------------------------------------------------

void HosESP8266ReadData(uint16_t page_number)
{
    /* Receive Page */
    printf_P(PSTR("FRPG=%d\r\n"), page_number);

    for(uint16_t i = 0; i < SPM_PAGESIZE; i++) {
        buffer[i] = getchar();
    }

    // hexDump("DMP", buffer, SPM_PAGESIZE);
    crc = HosUdsCRC16(buffer, SPM_PAGESIZE, crc);
}

//------------------------------------------------------------------------------

uint8_t HosLoadProgramFromESP8266(uint16_t rem_crc)
{
    uint16_t address = 0;

    wdt_disable();

    /* Erase application flash section */
    HosEspEraseProgramSpace();
    crc = 0xFFFF;
    /* ------------------------------- */

  
    for(uint16_t n = 0; n < HOS_FIRMWARE_TOTAL_IMAGE_SECTOR_COUNT; n++) {
      LED_OFF();
      _delay_ms(1);
      // wdt_enable(WDTO_8S);
      HosESP8266ReadData(n);
      address = HosEspWriteProgramPage(buffer, address);
      // wdt_disable();
      LED_ON();
      _delay_ms(1);
    }

    // printf_P(PSTR("%u=%u\r\n"), rem_crc, crc);
    if(rem_crc == crc) { /* We update success */
      HosLedBlink(2);

      /* Let notify master MCU that we updated firmware successfully */
      printf_P(PSTR("FCHK=%d\r\n"), 2);
      _delay_ms(2000);
      return TRUE;
    } else {
      /* Let notify master MCU that firmware update failed */
      printf_P(PSTR("FCHK=%d\r\n"), 3);
      _delay_ms(2000);
      HosLedBlink(5); /* Update faild */
    }

  return FALSE;
}

//------------------------------------------------------------------------------

uint8_t _HosMainRunUpdate()
{
    uint16_t rem_crc;

    if((rem_crc = HosESP8266NeedUpdate(HosEspIsEmptyApp())) > 0) {
      LED_OFF();
      if(HosLoadProgramFromESP8266(rem_crc)) {
        HosRebootCPU();
        asm volatile("jmp 0x0000" ::);
      }  else {
        // HosRebootCPU();
        _delay_ms(1000);
        return FALSE;
      }
   }

   return TRUE;
}

//------------------------------------------------------------------------------

/**
 * Initialize Device
 *
 */
void HosInitDevice(void)
{
    /* Disable global interrupt */
    cli();

    /* Reset watchdog */
    wdt_reset();

    /* Disable watchdog */
    /*     MCUSR  &= ~(1 << WDRF);  */
    MCUSR &= ~(1 << WDRF) & ~(1 << BORF) & ~(1 << EXTRF) & ~(1 << PORF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR  = 0x00;


#if defined(WIFI_RESET_PORT)
    /* Set 'Wireless' Reset pin as digital output */
    WIFI_RESET_DDR |= _BV (WIFI_RESET_PIN);

    // -- Config transmitter
    WIFI_RESET_PORT |= _BV(WIFI_RESET_PIN);  // Set high Reset pin
#endif

#if defined(LED_OUTPUT_PORT)
    /* Set 'LED' pin as digital output */
    LED_OUTPUT_DDR |= _BV(LED_OUTPUT_PIN);
    LED_OFF();
#endif

    // Serial Init
    HosSerialInit();
}

//------------------------------------------------------------------------------

int main(void)
{
 /*
  * Open STD for terminal output
  * Assign to default Standard IO facilities -> stdin,stdout,stderr
  */
  stdout = stdin = stderr = &mystdout;

  /* Initialize Device */
  HosInitDevice();

  /* Turn on on-board led */
  LED_ON();

  /* Wait for primary CPU to boot */
  _delay_ms(1800);

  /* Output in serial port bootloader version number */
  printf_P(PSTR("VER=%c.%c\r\n"), BOOTLOADER_VERSION_MAJOR, BOOTLOADER_VERSION_MINOR);

  while(TRUE) {

    /* Wait for about 3sec to check if there is any update. Otherwise run firmware */
    if(_HosMainRunUpdate()) {

      for(int i = 0; i < 3; i++) {

        LED_OFF();
        _delay_ms(100);
        LED_ON();
        _delay_ms(100);
      }

      asm volatile("jmp 0x0000" ::);
    }
  }

  return 0;
}

//------------------------------------------------------------------------------

/*
 *	hos_util.c
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
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "hos_util.h"


// ------------------------------------------------------------------

uint8_t HosEspIsEmptyApp()
{
  return (pgm_read_byte(0) == 0xFF);
}

// ------------------------------------------------------------------

void HosEspEraseProgramSpace(void)
{
    uint16_t address = 0;

    /* Erase application flash section */
    cli();
    while (address < APP_END) {
        boot_page_erase(address);    // perform page erase
        boot_spm_busy_wait();        // wait until the memory is erased.
        address += SPM_PAGESIZE;
    }
    sei();
}

// ------------------------------------------------------------------

uint16_t HosEspWriteProgramPage(uint8_t *buffer, uint16_t address)
{
   uint16_t mem_data;

   for(uint8_t i = 0; i < SPM_PAGESIZE;) {
      mem_data = buffer[i++];
      mem_data |= (buffer[i++] << 8);
      boot_page_fill(address, mem_data);
      address += 2;
   }

   boot_page_write(address - SPM_PAGESIZE);   // write to flash
   boot_spm_busy_wait();
   boot_rww_enable();         // we-enable the RWW section

   return address;
}

// ------------------------------------------------------------------

void HosSerialInit(void)
{
    myUBRRH = (F_CPU / (BAUDRATE * 16L) - 1) >> 8;       // calculate baudrate and set high byte
    myUBRRL = (uint8_t)(F_CPU / (BAUDRATE * 16L) - 1);   // and low byte
    myUCSRB = _BV(myTXEN) | _BV(myRXEN);                 // enable transmitter and receiver and receiver interrupt
    myUCSRC = myURSEL | _BV(myUCSZ1) | _BV(myUCSZ0);     // 8 bit character size, 1 stop bit, no parity
}

// ------------------------------------------------------------------

int HosSerialTX(char b, FILE *stream)
{
   while (!(myUCSRA & (1<<myUDRE)));   // Wait for empty transmit buffer
   myUDR = b;   // send byte

   return myUDR;
}

// ------------------------------------------------------------------

int HosSerialRX(FILE *stream)
{
  while (!(myUCSRA & (1<<myRXC)));
  return myUDR;                     // Get byte
}

// ------------------------------------------------------------------

void HosLedBlink(uint8_t count)
{
    for(uint8_t i = 0; i < count; i++) {
      LED_ON();
      _delay_ms(200);
      LED_OFF();
      _delay_ms(200);
    }
}

// ------------------------------------------------------------------

void HosRebootCPU()
{
  WIFI_RST_OFF();
  _delay_ms(100);
  WIFI_RST_ON();
  _delay_ms(3000);
}

// ------------------------------------------------------------------

/**
 * CRC16-CCITT checksum calculation
 *
 * @param data source buffer
 * @param length source buffer length
 * @param crc start rcr value
 */
uint16_t HosUdsCRC16(uint8_t *data, uint16_t length, uint16_t crc)
{
    uint8_t x;

    while(length--) {
        x = crc >> 8 ^ *data++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t) (x << 12)) ^ ((uint16_t) (x << 5)) ^ ((uint16_t) x);
    }

    return crc;
}

// ------------------------------------------------------------------

void hexDump (char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf_P (PSTR("%s:\n"), desc);

    if (len == 0) {
        printf_P(PSTR("  ZERO LENGTH\n"));
        return;
    }
    if (len < 0) {
        printf_P(PSTR("  NEGATIVE LENGTH: %i\n"),len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf_P (PSTR("  %s\n"), buff);

            // Output the offset.
            printf_P (PSTR("  %04x "), i);
        }

        // Now the hex code for the specific character.
        printf_P (PSTR(" %02x"), pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf_P (PSTR("   "));
        i++;
    }

    // And print the final ASCII bit.
    printf_P (PSTR("  %s\n"), buff);
}

// ------------------------------------------------------------------

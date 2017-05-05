/*
 *	hos_util.c
 *
 *	Copyright (c) 2017 Picktek LLC
 *
 *	This file is part of Hyperios.
 *
 *	Hyperios is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	Hyperios is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with Hyperios.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/interrupt.h>
#include <avr/boot.h> 
#include <util/delay.h>

#include "hos_util.h"


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

void HosSerialTX(uint8_t b) 
{ 
   while (!(myUCSRA & (1<<myUDRE)));   // Wait for empty transmit buffer 
   myUDR = b;   // send byte 
} 

// ------------------------------------------------------------------

uint8_t HosSerialRX(void) 
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


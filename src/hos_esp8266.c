/*
 *	hos_esp8266.c
 *
 *	Copyright (c) 2015 Picktek LLC
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


#include <stdlib.h>
#include <ctype.h>
#include <avr/boot.h> 
#include <util/delay.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "hos_esp8266.h"
#include "hos_util.h"
#include "config.h"

extern hos_config_perm conf;
uint16_t checksum; 
uint16_t indexa;
    
static inline unsigned char serialReceiveHexNibble(char *data)
{ 
   unsigned char in = data[indexa++]; 
   if (in >= 'A') 
      in -= 'A' - 10; 
   else 
      in -= '0'; 
   return in; 
} 

static inline unsigned char bootloaderGetData(char *data)
{ 
   unsigned char in; 

   in = serialReceiveHexNibble(data) << 4; 
   in |= serialReceiveHexNibble(data); 
   checksum += in; 
   return in; 
} 

void _HosEspConfigure(void) 
{   
    /* SoftAP  mode */
    printf_P(PSTR("AT+CWMODE_CUR=2\r\n")); 

    _delay_ms(1000);

    /* Create newtwork using SoftAP mode , 11 Channel, 3 - WPA2_PSK security password 'MOSTAT123' */
    printf_P(PSTR("AT+CWSAP_CUR=\"%s\",\"MOSTAT123\",11,3\r\n"), conf.ssid); 

    _delay_ms(1000);

    /* Set multiple connection mode */
    printf_P(PSTR("AT+CIPMUX=1\r\n"));

    _delay_ms(1000);
    
    /* Listen TCP Poer */
    printf_P(PSTR("AT+CIPSERVER=1,%d\r\n"), BOOTLOADER_PORT);

}

// ------------------------------------------------------------------

void HosEspWrite_P(const char *fmt, ...) {

    va_list ap;
    FILE f;

    static char s[64];// = HosMemAlloc(128);

    f.flags = __SWR | __SSTR | __SPGM;
    f.buf = s;
    f.size = INT16_MAX;
    va_start(ap, fmt);
    vfprintf(&f, fmt, ap);
    va_end(ap);
    s[f.len] = 0;

    printf_P(PSTR("AT+CIPSEND=0,%d\r\n"), f.len);
    _delay_ms(20);
    printf(s);
    _delay_ms(20);
}

// ------------------------------------------------------------------

static uint8_t _HosEspExecute(char *command)
{
    char *cmd = strtok_P(command, PSTR("="));
    
    uint8_t memory[SPM_PAGESIZE]; // create temporary page 
    
    uint8_t count, type, i; 
    uint16_t address = 0;
    uint8_t chkval;


    /* Erase flash section */
    if(strcasecmp_P(cmd, PSTR("AT+BLEF")) == 0) {
        HosEspEraseProgramSpace();
        address = 0;
        HosEspWrite_P(PSTR("\r\nOK\r\n"));
    } else 

    /* Upload page */
    if(command[0] == '>') {
    
            indexa = 1;
            checksum = 0;                  
            count = bootloaderGetData(command);     // get the packet size 
            address = bootloaderGetData(command);   // get the memory address 
            address <<= 8; 
            address |= bootloaderGetData(command); 
            type = bootloaderGetData(command);      // get the packet type 

            for (i = 0; i < count; i++)      // get the data 
               memory[i] = bootloaderGetData(command); 

			
			if(checksum > 0xFF) {
				chkval = 0x100 - (checksum & 0xFF);
			} else {
				chkval = 0x100 - checksum;
			}
			
			checksum = bootloaderGetData(command);
			
            if (checksum == chkval) { 
                
                if (type == 0) {
                   LED_ON(); 
                   while (count < SPM_PAGESIZE)   // fill rest of buffer 
                      memory[count++] = 0xFF; 
                  
                   address = HosEspWriteProgramPage(memory, address);
                   LED_OFF(); 
                }
                
                HosEspWrite_P(PSTR("\r\nOK\r\n"));
                
                if(type == 1) {
                    asm volatile("jmp 0x0000" ::);
                }
                 
            } else {
                HosEspWrite_P(PSTR("\r\nERR CHK\r\n"));
            }
    } else 
    
    /* Execute application */
    if(strcasecmp_P(cmd, PSTR("AT+BLX")) == 0) {
      HosEspWrite_P(PSTR("\r\nOK\r\n"));
      asm volatile("jmp 0x0000" ::);
    } else 
        
    /* Get bootloader version number */
    if(strcasecmp_P(cmd, PSTR("AT+BLV")) == 0) {
      HosEspWrite_P(PSTR("\r\n+BLV:%c.%c\r\n"), BOOTLOADER_VERSION_MAJOR, BOOTLOADER_VERSION_MINOR);
      HosEspWrite_P(PSTR("\r\nOK\r\n"));
    } else 
    
    if(strcmp_P(cmd, PSTR("WIFI GOT IP")) == 0) {
      _HosEspConfigure();
    } else {
        return 0;
    }
    
    return 1;
}

// ------------------------------------------------------------------

void HosEspRunLoop(void) 
{
  int16_t ret = 0;
  static char command[HOS_SHELL_MAX_CMD_LENGTH + 1];
  uint8_t i = 0;

   _HosEspConfigure();

  while(1) {
    ret = fgetc(stdin);
    if(ret == 0 || ret == '\r') continue;
    if(ret == ':') {
        i = 0; continue;
    }

    if(ret != '\n' && i < HOS_SHELL_MAX_CMD_LENGTH) {
      command[i++] = ret; 
    } else {
      command[i] = 0;
      i = 0;
      _HosEspExecute(command);
    }
  }
}

// ------------------------------------------------------------------

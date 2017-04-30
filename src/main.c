/*
 *	main.c
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
 *
 */

#include <avr/boot.h> 
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "config.h" 
#include "hos_esp8266.h"
#include "hos_i2c.h"
#include "hos_24cxx.h"


hos_config_perm conf;

hos_config_perm EEMEM eemem_conf = {
            0x00, /* MCUSR value */
            0x00, /* Number of booting */ 
            0x00, /* flash_lock bits */
            0x0000, /* EEPROM page count*/ 
            0x0000, /* EEPROM page offset */
            0x0000, /* EEPROM Recovery page count */
            0x0000, /* EEPROM Recovery page offset */
            {0x22,0xEF,0x61,0x5E,0x7B,0xCD,0x47,0xE0,0xAF,0xD1,0x8D,0x43,0xC8,0x66,0xDB,0xAB}, /* UUID */
            {'M','O','S','T','A','T','-','8','d','4','3','c','8','6','6','d','b','a','b',0}, /* SSID */
          };


static FILE mystdout = FDEV_SETUP_STREAM(HosSerialTX, HosSerialRX,
                                         _FDEV_SETUP_RW);
                                         
uint8_t buffer[SPM_PAGESIZE];

//------------------------------------------------------------------------------

void HosSerialTX(uint8_t b) 
{ 
   while (!(myUCSRA & (1<<myUDRE)));   // Wait for empty transmit buffer 
   myUDR = b;   // send byte 
} 

//------------------------------------------------------------------------------

uint8_t HosSerialRX(void) 
{ 
   while (!(myUCSRA & (1<<myRXC))); 
   return myUDR;                     // Get byte 
} 

//------------------------------------------------------------------------------

void _HosLedBlink(uint8_t count) {

    for(uint8_t i = 0; i < count; i++) {
      LED_ON();
      _delay_ms(200);
      LED_OFF();
      _delay_ms(200);
    }
}

//------------------------------------------------------------------------------
#if defined(ALLOW_24CXX)
void _HosLoadProgramFromEEPROM(uint16_t start_page, uint16_t page_count)
{
      uint16_t address = 0; 

      /* Erase application flash section */
      HosEspEraseProgramSpace();
      /* ------------------------------- */

      for(uint16_t n = start_page; n < page_count; n++) {
         Hos24cxxReadData(n * SPM_PAGESIZE, (unsigned char*)buffer, SPM_PAGESIZE);
         address = HosEspWriteProgramPage(buffer, address);
      }
      _HosLedBlink(5);
}
#endif

//------------------------------------------------------------------------------

void _HosSaveConfig(void)
{
  eeprom_busy_wait();
  eeprom_update_block(&conf, (void*)&eemem_conf, sizeof(hos_config_perm));
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
    
    /* Read config block from EEPROM */
    eeprom_read_block(&conf, (const void*)&eemem_conf, sizeof(hos_config_perm));
    
    if(conf.number_of_boot == 0xFF) {
       conf.number_of_boot = 0; 
    }
    
    conf.number_of_boot += 1;
    conf.mcusr = MCUSR;
    _HosSaveConfig();

		
    /* Disable watchdog */
    /*     MCUSR  &= ~(1 << WDRF);  */
    MCUSR &= ~(1 << WDRF) & ~(1 << BORF) & ~(1 << EXTRF) & ~(1 << PORF);
    WDTCSR |= (1 << WDCE) | (1 << WDE); 
    WDTCSR  = 0x00; 

    /* Set 'Button' pin as digital input */
    BUTTON_INPUT_DDR  |= _BV(BUTTON_INPUT_PIN);   

#if defined(WIRELESS_RESET_PORT) 
    /* Set 'Wireless' Reset pin as digital output */
    WIRELESS_RESET_DDR |= _BV (WIRELESS_RESET_PIN); 
    
    // -- Config transmitter
    WIRELESS_RESET_PORT |= _BV(WIRELESS_RESET_PIN);  // Set high Reset pin
#endif

#if defined(LED_OUTPUT_PORT) 
    /* Set 'LED' pin as digital output */
    LED_OUTPUT_DDR |= _BV(LED_OUTPUT_PIN);
    LED_OFF();
#endif    


    // Serial Init 
    myUBRRH = (F_CPU / (BAUDRATE * 16L) - 1) >> 8;       // calculate baudrate and set high byte
    myUBRRL = (uint8_t)(F_CPU / (BAUDRATE * 16L) - 1);   // and low byte
    myUCSRB = _BV(myTXEN) | _BV(myRXEN);                 // enable transmitter and receiver and receiver interrupt  
    myUCSRC = myURSEL | _BV(myUCSZ1) | _BV(myUCSZ0);     // 8 bit character size, 1 stop bit, no parity
}

//------------------------------------------------------------------------------

uint8_t _HosFactoryClicked(void)
{
    static uint32_t debon = 0;
    if (HosIoBitIsSet(BUTTON_INPUT_REG, BUTTON_INPUT_PIN)) {
        debon++;
        if(debon == 1000000) {
          _HosLedBlink(3);
          return 1;
        }
    } 

    return 0;
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
    
    /* Initialize I2C Bus for External EEPROM */
    HosI2CInit();

    /* Waiting about 3sec */
    for(uint32_t i = 0; i < 2000000; i++) {
      // Mannualy enter in DFU mode by clicking onboard Factory-Reset button 
      // Long click about 5-6 seconds to enter DFU mode
      if(_HosFactoryClicked()) {
        goto dfu;
      }
    }

    /* If bit is set start application otherwise start bootloader code */
    if(conf.flash_lock == BOOTLOADER_LOCK_VALUE) {
        asm volatile("jmp 0x0000" ::);
    } 
#if defined(ALLOW_24CXX)
    else if(conf.flash_lock == BOOTLOADER_EEPROM_LOAD_VALUE) {

        _HosLoadProgramFromEEPROM(conf.eeprom_page_offset, conf.eeprom_page_count);

        conf.flash_lock = BOOTLOADER_EEPROM_LOAD_COMPLETED_VALUE;
        _HosSaveConfig();
        asm volatile("jmp 0x0000" ::);

    } else if(conf.flash_lock == BOOTLOADER_EEPROM_RECOVERY_VALUE) {

        _HosLoadProgramFromEEPROM(conf.eeprom_recovery_page_offset, conf.eeprom_recovery_page_count);

        conf.flash_lock = BOOTLOADER_EEPROM_RECOVERY_COMPLETED_VALUE;
        _HosSaveConfig();
        asm volatile("jmp 0x0000" ::);
    }
#endif
    
dfu:
    LED_ON();

    /* Start bootloader */
    HosEspRunLoop();
    
    return 0; 
}

/*
 *	config.c
 *
 *	Copyright (c) 2013 Picktek LLC
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


#ifndef _HOS_CONFIG_H 
#define _HOS_CONFIG_H 



/* ------------------- START CONFIGURATION ------------------- */

/* Baud rate for ESP8266 Wi-Fi module */
#define BAUDRATE                    19200

/* 'WIFI 07/12 module' reset PIN */
#define WIRELESS_RESET_PORT         PORTD       
#define WIRELESS_RESET_REG          PIND        
#define WIRELESS_RESET_PIN          PIND2       
#define WIRELESS_RESET_DDR          DDRD    

/* On Board Relay */
#define WIFI_RST_ON()  WIRELESS_RESET_PORT |= (_BV(WIRELESS_RESET_PIN))
#define WIFI_RST_OFF() WIRELESS_RESET_PORT &= ~(_BV(WIRELESS_RESET_PIN))
#define WIFI_RST_ION() ((WIRELESS_RESET_PORT & (_BV(WIRELESS_RESET_PIN))) > 0)

/* 'On-board factory button' */
#define BUTTON_INPUT_PORT           PORTD        /* PORTx - register for 'Buttons' */
#define BUTTON_INPUT_REG            PIND         /* PINx - register for 'Buttons' */
#define BUTTON_INPUT_PIN            PIND3        /* bit for 'Buttons' for input/output */
#define BUTTON_INPUT_DDR            DDRD         /* 'Buttons' data direction register */

#if 'A' == BOARD_REV
    /* 'Light Status for old version mostat REVA' */
    #define LED_OUTPUT_PORT PORTC
    #define LED_OUTPUT_REG  PINC
    #define LED_OUTPUT_PIN  PINC1
    #define LED_OUTPUT_DDR  DDRC
#elif 'B' == BOARD_REV
    /* 'Light Status for new version mostat REVB' */
    #define LED_OUTPUT_PORT PORTB
    #define LED_OUTPUT_REG  PINB
    #define LED_OUTPUT_PIN  PINB1
    #define LED_OUTPUT_DDR  DDRB
#else 
    #error "Unknow revision !"
#endif

/* Using external EEPROM. If you don't need to update from EEPROM just comment */
// #define ALLOW_24CXX

/* Using WI-FI serial programming. If you don't need WI-FI serial programming just comment */
// #define ALLOW_WIFI

/* Using external ESP8266 Wi-fi chip SPI flash. If you don't need to update from this method just comment */
#define ALLOW_ESP8266_UART

/* ------------------- END CONFIGURATION ------------------- */


#endif /* _HOS_CONFIG_H */

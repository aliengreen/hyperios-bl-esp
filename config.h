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

/* 'On-board factory button' */
#define BUTTON_INPUT_PORT           PORTD        /* PORTx - register for 'Buttons' */
#define BUTTON_INPUT_REG            PIND         /* PINx - register for 'Buttons' */
#define BUTTON_INPUT_PIN            PIND3        /* bit for 'Buttons' for input/output */
#define BUTTON_INPUT_DDR            DDRD         /* 'Buttons' data direction register */

/* 'On-board LED' */
#define LED_OUTPUT_PORT             PORTC        /* PORTx - register for 'Buzzer' */
#define LED_OUTPUT_REG              PINC         /* PINx - register for 'Buzzer' */
#define LED_OUTPUT_PIN              PINC1        /* bit for 'Buzzer' for input/output */
#define LED_OUTPUT_DDR              DDRC         /* 'Buzzer' data direction register */

/* Using external EEPROM. If you don't need to update from EEPROM just comment */
#define ALLOW_24CXX

/* Using WI-FI serial programming. If you don't need WI-FI serial programming just comment */
#define ALLOW_WIFI

/* ------------------- END CONFIGURATION ------------------- */


#endif /* _HOS_CONFIG_H */

/*
 *	hos_i2c.h
 *
 *	Copyright (c) 2016 Picktek LLC
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

#ifndef _HOS_I2C_H
#define _HOS_I2C_H

#include <stdio.h>
#include <string.h>
#include <util/twi.h>

/* 
 *  On the AVRmega series, PA4 is the data line (SDA) and PA5 is the clock (SCL
 *  The standard clock rate is 100 KHz, and set by I2C_Init. It depends on the AVR osc. freq.
 */

#define F_SCL       	    200000L                 // I2C clock speed 100 KHz
#define READ  		        1
#define HOS_TW_START        0xA4                    // send start condition (TWINT,TWSTA,TWEN)
#define HOS_TW_STOP         0x94                    // send stop condition (TWINT,TWSTO,TWEN)   
#define HOS_TW_ACK          0xC4                    // return ACK to slave
#define HOS_TW_NACK         0x84                    // don't return ACK to slave
#define HOS_TW_SEND         0x84                    // send data (TWINT,TWEN)
#define HOS_TW_READY        (TWCR & 0x80)           // ready when TWINT returns to logic 1.
#define HOS_TW_STATUS       (TWSR & 0xF8)           // returns value of status register



#define HOS_PCF85263A			0xA2
#define HOS_24CXX				0xA0

void HosI2CInit(void);
uint8_t HosI2CFindDevice(uint8_t start);
void HosI2CWriteRegister(uint8_t busAddr, uint8_t deviceRegister, uint8_t data);
uint8_t HosI2CReadRegister(uint8_t busAddr, uint8_t deviceRegister);

uint8_t HosI2CDetect(uint8_t addr);
uint8_t HosI2CStart(uint8_t slaveAddr);
uint8_t HosI2CWrite(uint8_t data);
void HosI2CStop(void);
uint8_t HosI2CReadACK(void);
uint8_t HosI2CReadNACK(void);
void HosI2CWriteByte(uint8_t busAddr, uint8_t data);

#endif

/*
 *	hos_i2c.c
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

#include "hos_i2c.h"

// ------------------------------------------------------------------

// at 16 MHz, the SCL frequency will be 16/(16+2(TWBR)), assuming prescalar of 0. // so for 100KHz SCL, TWBR = ((F_CPU/F_SCL)-16)/2 = ((16/0.1)-16)/2 = 144/2 = 72. 
void HosI2CInit(void)
{
	TWSR = 0; // set prescalar to zero
	TWBR = ((F_CPU/F_SCL) - 16)/2; // set SCL frequency in TWI bit register 
}

// ------------------------------------------------------------------

// look for device at specified address; return 1=found, 0=not found
uint8_t HosI2CDetect(uint8_t addr)
{
	TWCR = HOS_TW_START;
	while (!HOS_TW_READY);
	TWDR = addr;
	TWCR = HOS_TW_SEND;
	while (!HOS_TW_READY);
	return (HOS_TW_STATUS == 0x18);
}

// ------------------------------------------------------------------

uint8_t HosI2CStart(uint8_t slaveAddr)
{
	return HosI2CDetect(slaveAddr); 
}

// ------------------------------------------------------------------

uint8_t HosI2CWrite(uint8_t data)
{
    TWDR = data;
    TWCR = HOS_TW_SEND;
    while (!HOS_TW_READY);
    return (HOS_TW_STATUS != 0x28);
}

// ------------------------------------------------------------------

uint8_t HosI2CReadACK(void)
{
    TWCR = HOS_TW_ACK;			// ack = will read more data
    while (!HOS_TW_READY);		// wait
    return TWDR;
}

// ------------------------------------------------------------------

uint8_t HosI2CReadNACK(void)
{
    TWCR = HOS_TW_NACK;
    while (!HOS_TW_READY);
    return TWDR;
    //return (TW_STATUS!=0x28);
}

// ------------------------------------------------------------------

void HosI2CWriteByte(uint8_t busAddr, uint8_t data) 
{
	HosI2CStart(busAddr); 
	HosI2CWrite(data); 
	HosI2CStop();
}

// ------------------------------------------------------------------

void HosI2CStop()
{
	TWCR = HOS_TW_STOP;
	while(TWCR & (1<<TWSTO));
}

// ------------------------------------------------------------------


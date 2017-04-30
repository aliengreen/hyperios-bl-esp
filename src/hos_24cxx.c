/*
 *  hos_24cxx.c
 *
 *  Copyright (c) 2016 Picktek LLC
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

#include "hos_24cxx.h"
#include <hos_i2c.h>

#define i2c_read(ack)  (ack) ? HosI2CReadACK() : HosI2CReadNACK(); 

/* Reads sequential data from the specified address */
void Hos24cxxReadData( int address, unsigned char *data, int len )
{
	int i;

	while(!HosI2CStart( EEPROM_ADDRESS ));
	HosI2CWrite( address >> 8 );	// MSB of address
	HosI2CWrite( address );		// LSB of address

	// Start reading
	HosI2CStart( EEPROM_ADDRESS + READ );
	for( i = 0; i < len; i++ )
		*data++ = i2c_read( (i < len-1) );	// Send ACK as long as read<len – i.e. we want another byte
	
	HosI2CStop();
}


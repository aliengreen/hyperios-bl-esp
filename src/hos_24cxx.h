/*
 *  hos_24c.h
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
 

#include <avr/io.h>

#ifndef _HOS_24CXX_H_
#define _HOS_24CXX_H_


/**
@defgroup 24C EEPROM library
 @brief Utility functions for using 24C series EEPROMs.
 
 @code #include "h" @endcode

 24C EEPROM Library
 
 The 24C EEPROM library contains helper functions for using the 24C (specifically, Microchip 24LC512) series EEPROMs.
 
@note
 This library requires I2C functions compatible with <a href="http://jump.to/fleury">Peter Fleury's I2C (TWI) Master Software Library</a>!

 @author Lasha Dolidze http://picktek.com
 
 */

/**@{*/

/** Defines the I2C device address of the EEPROM device. This address must already be shifted one byte to the left to make room for the read/write bit. So for a device where the upper four bits are configured as 1010 (e.g. M24C64) and the Chip Enable pins E2:E0 are all tied to ground, the final address is 0xA0. */
#define EEPROM_ADDRESS 0xA0

/**
 @brief Reads sequential data.
 @param address The 16 bit memory address to start reading from.
 @param data Pointer to a buffer to receive read data.
 @param len Number of bytes to read.
 
 This function reads `len` bytes from the specified address and forward.
 */
void Hos24cxxReadData( int address, unsigned char *data, int len );

/**@}*/

#endif

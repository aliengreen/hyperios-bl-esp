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

/** Writes a single byte to the specified destination address.
 @param address The 16 bit memory address to write to.
 @param data The 8 bit value to store.
 */
void Hos24cxxWriteByte( int address, uint8_t data );

/** Writes a 32 byte page to the specified address.
 @param address The 16 bit memory address to write to.
 @param data Pointer to the data to write.
 @param len Length of data to write. **NOTE: This should not exceed 128 bytes or weird stuff will happen**
 
 A page is 128 bytes long. When writing a page, an internal memory address pointer is increased for every byte. But only the last 7 bits are increased. So if the data is longer than 128 bytes or the address is not aligned to the start of a page, the address pointer will wrap around to the start of the page and continue writing.
 
 @warning To align the address to the start of a page, the last seven bits of the address _must_ be 0. If this is not the case, the address pointer will wrap around to the start of the page and continue so the data written will not be in the same sequence as the original data.
 */
void Hos24cxxWritePage( int address, unsigned char *data, uint8_t len);

/** Reads one byte from the specified address.
 @param address The 16 bit address to read from.
 @return The 8 bit value stored at the specified address.
 */
uint8_t Hos24cxxReadByte( int address );


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

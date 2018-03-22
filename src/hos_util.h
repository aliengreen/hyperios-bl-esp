/*
 *	hos_util.h
 *
 *  Copyright (c) 2018 Alien Green LLC
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

#ifndef _HOS_UTIL_H
#define _HOS_UTIL_H

#include <stdio.h>
#include <string.h>
#include <avr/boot.h> 

#include "config.h"


void HosSerialInit(void);
int HosSerialTX(char b, FILE *stream);
int HosSerialRX(FILE *stream);
void HosLedBlink(uint8_t count);
void HosRebootCPU();
void HosEspEraseProgramSpace(void);
uint16_t HosEspWriteProgramPage(uint8_t *buffer, uint16_t address);
uint16_t HosUdsCRC16(uint8_t *data, uint16_t length, uint16_t crc);
void hexDump (char *desc, void *addr, int len);


#endif

/*
 *	hos_esp8266.h
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
 
/** 
 *  @defgroup hyperios_os_esp8266 ESP8266 Wifi functionality
 * 
 *  @brief ESP8266 functionality
 *
 *  @author Lasha Dolidze lasha@picktek.com http://www.picktek.com
 */
 
/**@{*/

#ifndef _HOS_ESP8266_H
#define _HOS_ESP8266_H

#include <stdio.h>
#include <string.h>

#define HOS_SHELL_MAX_CMD_LENGTH            128

void HosEspRunLoop(void);
void HosEspEraseProgramSpace(void);
uint16_t HosEspWriteProgramPage(uint8_t *buffer, uint16_t address);

#endif /* _HOS_ESP8266_H */

/**@}*/
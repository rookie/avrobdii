/*
Copyright (C) Trampas Stern  name of author

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __FLASH_H
#define __FLASH_H

#include "datatypes.h"


#define FLASH_RD_STATUS	 0xD7
#define FLASH_MAX_PAGES	0x07FF00

void flash_init();

UBYTE flash_erase_page(UWORD page) ;

UBYTE flash_read(UDWORD addr);
UBYTE flash_erase();
UBYTE flash_put(UDWORD address, UBYTE data);
UBYTE flashRead(UBYTE *data, UBYTE count, UDWORD address);

UBYTE Flash_serial_program(UDWORD address);

#endif //__FLASH_H

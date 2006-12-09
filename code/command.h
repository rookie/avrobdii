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

#ifndef __COMMAND_H
#define __COMMAND_H

#include "system.h"
#include <avr/pgmspace.h>

#define MAX_CMD_LENGTH 30
#define MAX_ARGS 10
#define MAX_ARG_LENGTH 20
#define CMD_PROMPT "AVROBDII>"

#define CLEAR_SCREEN() {printf_P(PSTR("\e[2J\e[f"));} 

UINT CommandBegin();

#endif


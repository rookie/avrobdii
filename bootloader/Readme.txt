If a 12mhz ceramic resonator is loaded onto the ARC board, the external high
speed crystal enabled, Bootloader reset vector enabled for 512 word boot block
and the .HEX file from this directory loaded into the mega16, the ARC board
becomes and STK500 emulator and code can be directly loaded into the CPU from
STK500, Astudio, BASCOM or AvrDude without an additional programmer dongle.

The boot loader will wait for programming commands for 6 seconds after boot,
when a user program is loaded.  It flashes the program LED while waiting.  If no
user program is loaded, the bootloader will flash the LED three times and then
wait for programming commands.

Alternatively, if you short out PB6 ('A' on the programming header) and reset
the board, the bootloader will flash the LED three times and then wait for
programming commands.

If you want to change the bootloader for other baud rates or crystal
frequencies, or for different hardware (CPU, LED or SW) modify the file "ARC.MK"
and rebuild using WinAvr GCC 3.4.3 or higher.

Cheers!
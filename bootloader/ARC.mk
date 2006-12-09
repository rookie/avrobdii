MCU	= at90can128
BOOT_START = 0x1F800
#define Boot override & indicator LED ports.
DEFS	=  -DF_CPU=16000000	\
	  -DBAUD_RATE=115200 \
	  -DBOOT_START=$(BOOT_START)
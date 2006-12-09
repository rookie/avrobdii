# Makefile for BootLoader
# Larry Barello, 2005-05-21
# E.Lins, 2004-10-14
# Jason Kyle, ???

# enter the product name for which you want to build the bootloader

#TARGET = RS160D
#TARGET	= RS80D
TARGET	= ARC
#TARGET = WM11
#TARGET = WM1


############################################################
# You should not have to change anything below here.
############################################################

include $(TARGET).mk

PROGRAM    = Boot$(TARGET)
OBJ        = start.o boot.o
CC         = avr-gcc
SIZE = avr-size


MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:


override CFLAGS  = -g -Wall -Os -std=gnu99 -Wa,-adhlns=$(<:.c=.lst) \
		-Wstrict-prototypes -mmcu=$(MCU) -D$(TARGET) $(DEFS) \
		-gstabs -mshort-calls
override LDFLAGS  = -Wl,-Map,$(TARGET).map,--section-start=.text=$(BOOT_START),--cref,-e,boot, -nostdlib -gdwarf-2

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).elf $(PROGRAM).hex $(PROGRAM).lss sizeafter

free: free.pl
	@perl free.pl $(TARGET).map $(BOOT_START) '16*1024'

program: all
	avrdude -p $(MCU) -P com1 -c stk500 -V -u $(LOCK) $(EFUSE) $(FUSE_L) $(FUSE_H) -U flash:w:$(PROGRAM).hex

# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(PROGRAM).hex
ELFSIZE = $(SIZE) -x -A $(PROGRAM).elf
sizebefore:
	@if [ -f $(PROGRAM).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(PROGRAM).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi



$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o *.elf *.lst *.lss *.map

%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

boot.o:	boot.c makefile $(TARGET).mk
start.o: start.c makefile $(TARGET).mk

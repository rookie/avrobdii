#include <avr/io.h>

void boot_0(void);
void boot_1(void);

void start(void)
{
	asm volatile(
	"\trjmp	boot_0\n"	// Hardware reset entry point
	"\trjmp	boot_1\n"	// Entry point if called by application for direct com with stk500
	);
}

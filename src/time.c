#include "time.h"

int time_getTicks()
{
	asm("mov.l syscall_adress, r2\n"
	    "mov.l getTicks, r0\n"
	    "jmp @r2\n"
	    "nop\n"
	    "syscall_adress: .long 0x80020070\n"
	    "getTicks: .long 0x02C1");
}

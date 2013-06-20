/* 
 * $Id: ucos_timer.c,v 1.4 2002/02/07 01:08:53 linfusheng Exp $
 */

#include 	"includes.h"
#include	"ucos_timer.h"

void load_timer2(unsigned int ticks)
{
	/* Set up the timer gate, turn off the speaker */
	UCOS_OUTB((UCOS_INB(PPC_PORTB) & ~PPCB_SPKR) | PPCB_T2GATE, PPC_PORTB);
	UCOS_OUTB(TIMER2_SEL|WORD_ACCESS|MODE0|BINARY_COUNT, TIMER_MODE_PORT);
	UCOS_OUTB(ticks & 0xFF, TIMER2_PORT);
	UCOS_OUTB(ticks >> 8, TIMER2_PORT);
}

int timer2_running(void)
{
	return ((UCOS_INB(PPC_PORTB) & PPCB_T2OUT) == 0);
}

void waiton_timer2(unsigned int ticks)
{
	load_timer2(ticks);
	while ((UCOS_INB(PPC_PORTB) & PPCB_T2OUT) == 0)
		;
}
/* 
 * $Log: ucos_timer.c,v $
 * Revision 1.4  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 00:46:25  linfusheng
 * update
 *
 */




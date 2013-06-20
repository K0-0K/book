/*
 * $Id: interrupt.c,v 1.2 2002/02/07 01:08:53 linfusheng Exp $
 */
#include "includes.h"
#include "interrupt.h"

#define N_INTR	16
struct intrhand intrhand[N_INTR];



void 
init_intr_table(void)
{
	int i;
	for( i = 0; i < N_INTR ; i ++) {
		struct intrhand *ih = NULL;
		ih = &intrhand[i];
		ih->ih_fun = NULL;
		ih->ih_arg = NULL;
		ih->ih_count = 0;
		ih->ih_level = 0;
		ih->ih_irq = 0;
		ih->ih_what = NULL;		
	}
	return;
}

/*
 * Set up an interrupt handler to start being called.
 * XXX PRONE TO RACE CONDITIONS, UGLY, 'INTERESTING' INSERTION ALGORITHM.
 */
void *
intr_establish(irq, type, level, ih_fun, ih_arg, ih_what)
int irq;
int type;
int level;
int (*ih_fun) __P((void *));
void *ih_arg;
char *ih_what;
{
	struct intrhand *ih = NULL;

	if(irq >= N_INTR)
		return NULL;

	ih = &intrhand[irq];
	/*
	 * Poke the real handler in now.
	 */
	ih->ih_fun = ih_fun;
	ih->ih_arg = ih_arg;
	ih->ih_count = 0;
	ih->ih_level = level;
	ih->ih_irq = irq;
	ih->ih_what = ih_what;

	return ih;
}

/*
 * Deregister an interrupt handler.
 */
void
isa_intr_disestablish(irq)
int irq;
{
	struct intrhand *ih = NULL;

	if(irq >= N_INTR)
		return ;

	ih = &intrhand[irq];
	/*
	 * Poke the real handler in now.
	 */
	ih->ih_fun = NULL;
	ih->ih_arg = NULL;
	ih->ih_count = 0;
	ih->ih_level = 0;
	ih->ih_irq = 0;
	ih->ih_what = NULL;
	
}
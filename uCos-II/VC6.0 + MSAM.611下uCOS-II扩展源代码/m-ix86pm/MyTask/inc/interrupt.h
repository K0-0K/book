/* 
 * $Id: interrupt.h,v 1.1 2002/02/07 01:09:36 linfusheng Exp $
 */
#ifndef INTERRUPT_H
#define INTERRUPT_H
/*
 * Interrupt handler chains.  isa_intr_establish() inserts a handler into
 * the list.  The handler is called with its (single) argument.
 */

struct intrhand {
	int	(*ih_fun) __P((void *));
	void	*ih_arg;
	unsigned long	ih_count;
	int	ih_level;
	int	ih_irq;
	char	*ih_what;
};

extern struct intrhand intrhand[];

extern void init_intr_table(void);
/*
 * Set up an interrupt handler to start being called.
 * XXX PRONE TO RACE CONDITIONS, UGLY, 'INTERESTING' INSERTION ALGORITHM.
 */
extern void *intr_establish(int irq, int type, int level, int (*ih_fun) __P((void *)), void *ih_arg, char *ih_what);
#define IRQ_REQUEST(irq,func,arg)	\
	intr_establish(irq,0,0,func,arg,NULL)
#endif


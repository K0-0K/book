/* $Id: ucos_io.h,v 1.5 2002/02/07 10:25:41 linfusheng Exp $
 */
#ifndef UCOS_IO_H
#define UCOS_IO_H

#define readb(addr) (*(unsigned char *) (addr))
#define readw(addr) (*(unsigned short *) (addr))
#define readl(addr) (*(unsigned int *) (addr))

#define writeb(b,addr) (*(unsigned char *) (addr) = (b))
#define writew(b,addr) (*(unsigned short *) (addr) = (b))
#define writel(b,addr) (*(unsigned int *)(addr) = (b))



#ifdef PORT_BEYOND
	#define UCOS_OUTB(port,val)		_outp(port,val)			
	#define UCOS_OUTW(port,val)		_outpw(port,val)	
	#define UCOS_OUTD(port,val)		_outpd(port,val)
#else
	#define UCOS_OUTB(val,port)		_outp(port,val)	
	#define UCOS_OUTW(val,port)		_outpw(port,val)
	#define UCOS_OUTD(val,port)		_outpd(port,val) 		
#endif

#define UCOS_INB(port)		_inp(port)	
#define UCOS_INW(port)		_inpw(port)
#define UCOS_IND(port)		_inpd(port)

#endif
/* 
 * $Id: serial.c,v 1.5 2002/02/07 01:08:53 linfusheng Exp $
 */


#include "includes.h"
#include "serial.h"
#include "debug.h"
#include "interrupt.h"


/*
 * These are two static variables used in SERIAL.C.  pserial is
 * the pointer to the port that will be serviced by the
 * ISR.  The old break handler points to the CTRL-BREAK
 * handler that was in place before the port was opened.
 * It will be restored when the port is closed.
 */

static PORT serialport[4];
int serialport_found = 0;

/*
 * This is the interrupt service routine for the SERIAL port.
 * It sits in a loop reading the interrrupt ID register, then
 * servicing one of the four different types of interrupts.
 * Note that we shouldn't even get Modem Status and Line
 * interrupts in this implementation, but they are left
 * in for later enhancements.
 */
int OSSerial_interrupt(void *arg)
{
	unsigned char c;
	PORT *pserial = (PORT *)arg;
	switch (_inp( pserial->uart_base + IIR ) & IIR_INT_BITMASK ) {

/*
 * When a new character pseriales in and generates an
 * interrupt, it is read in.  If there is room in the input
 * buffer, it is stored, otherwise it is discarded.
 */
	case IIR_RX_ID :
		c = (unsigned char) _inp( pserial->uart_base+RXR );
		if ((pserial->in.write_index+1 ) != pserial->in.read_index)
			pserial->in.buffer[ pserial->in.write_index++ ] =  c ;
		break;

/*
 * If there are no valid interrupts left to service, an EOI
 * is written to the 8259 interrupt controller, and the
 * routine exits.
 */
	default :
		break;
	}


	return 0;
}


PORT *port_open( int address, int int_number )
{
	unsigned char temp;
	PORT *port = NULL;
	port = &serialport[serialport_found ++];
	port->in.write_index = port->in.read_index = 0;
	port->uart_base = address;
	port->irq_mask = (char) 1 << (int_number);
	port->interrupt_number = int_number;
//	SetIntVector( int_number+OSIRQ_OFFSET, OSSerialISR);
	IRQ_REQUEST(int_number,OSSerial_interrupt,port);
	
	/*
	 *The (IMR) tells the (PIC) to service an interrupt only if it
	 * is not masked (FALSE).
	 */
	// Turn on the interrupte bit 
	temp = (char) _inp( IMR_A );
	_outp( IMR_A, ~port->irq_mask & temp );

	return( port );
}


void port_set( PORT *port,
               long speed,
               char parity,
               int data,
               int stopbits )
{
	unsigned char lcr_out = 0;
	unsigned char mcr_out = 0;
	unsigned char low_divisor = 0 ;
	unsigned char high_divisor = 0;
/*
 * First disable all interrupts from the port.  I also read
 * RBR just in case their is a char sitting there ready to
 * generate an interupt.
 */
	_outp( port->uart_base + IER, 0 );
	_inp( port->uart_base );
/*
 * Writing the baud rate means first enabling the divisor
 * latch registers, then writing the 16 bit divisor int
 * two steps, then disabling the divisor latch so the other
 * registers can be accessed normally.
 */
	low_divisor = (char) (115200L / speed ) & 0xff;
	high_divisor = (char) ((115200L /  speed ) >> 8);
	_outp( port->uart_base + LCR, LCR_DLAB );
	_outp( port->uart_base + DLL, low_divisor );
	_outp( port->uart_base + DLH, high_divisor );
	_outp( port->uart_base + LCR, 0 );

	/* PARITY AND ODD/EVEN PARITY */
	switch(parity) {
	case 'O':
		lcr_out |= LCR_PARITY;
		break;
	case 'E':
		lcr_out |= LCR_PARITY;
		lcr_out |= LCR_PARITY_EVEN;
		break;
	default:
		break;
	}

	/* STOP BITS */	
	if (stopbits == 2 )
		lcr_out |= LCR_2_STOP;
	else 
		;	/* 1 stop bits */


	/* DATA BITS */
	switch(data) {
	case 5:
		lcr_out |= LCR_5_DATA_BITS;
		break;
	case 6:
		lcr_out |= LCR_6_DATA_BITS;
		break;
	case 7:
		lcr_out |= LCR_7_DATA_BITS;
		break;
	case 8:
		lcr_out |= LCR_8_DATA_BITS;
		break;
	default:
		break;
	}

	_outp( port->uart_base + LCR, lcr_out );

/*
 * I turn on RTS and DTR, as well as OUT2.  OUT2 is needed
 * to allow interrupts on PC pserialpatible cards.
 */
	mcr_out = MCR_RTS | MCR_DTR | MCR_OUT2 ;
	_outp( port->uart_base + MCR, mcr_out );

/*
 * Finally, restart receiver interrupts, and exit.
 */
	_outp( port->uart_base + IER, IER_RX_DATA);

}
/*
 * This routine is used to send a single character out to
 * the UART.  If there is room in the output buffer, the
 * character is inserted in the buffer.  Then the routine
 * checks to see if Transmit interrupts are presently
 * enabled for this UART.  If they aren't, they are turned
 * on so the ISR will see this new character.
 */
int port_putc( unsigned char c, PORT *port )
{
	unsigned long timeout = 0x0000ffff;
/*
	_outp( port->uart_base+MCR,MCR_DTR | MCR_RTS | MCR_OUT2);
	while((_inp(port->uart_base + MSR) & MSR_CTS) == 0)
		if(!(--timeout))
				return -1;
	timeout = 0x0000ffff;

*/
	while((_inp(port->uart_base + LSR) & LSR_XMTRDY) == 0)
		if(!(--timeout))
				return -1;

	_outp( port->uart_base+TXR,c);
	return( c );
}
/*
 * This routine checks to see if there is a character
 * available in the input buffer for the specified port.
 * If there is, it is pulled out and returned to the
 * caller.
 */
int port_getc(PORT *port)
{
	if ( port->in.write_index == port->in.read_index )
		return( -1 );
	else
		return( port->in.buffer[ port->in.read_index++ ] );
}


////////////////////////////////////////////////////////


void OSport_init(void)
{
	PORT *pserial;
	pserial = port_open(SERIAL1_UART,SERIAL1_INTERRUPT);
	if(pserial == NULL) {
		return;
	}
	port_set(pserial,9600,0,8,1);

	pserial = port_open(SERIAL2_UART,SERIAL2_INTERRUPT);
	if(pserial == NULL) {
		return;
	}
	port_set(pserial,9600,0,8,1);





}


int OSport_getc(int portnum,unsigned char flag)
{
	char  ch;

	PORT *pserial = &serialport[portnum];
	ch = port_getc(pserial);
	if(ch < 0)
		return -1;

	if(ch == '\r')
		OSport_putc(portnum,'\n');

	switch(flag){
	case SERIAL_NOECHO:	//没有回显
		break;
	case SERIAL_ECHOP:	//明文回显
		OSport_putc(portnum,ch);	
		break;
	case SERIAL_ECHOC:	//密文回显
		OSport_putc(portnum,'*');
		break;
	}
	return ch;
}


int OSport_gets(int portnum, unsigned char *buf,int max_get)
{
	int len = 0;
	while (1) {
		char  c = OSport_getc(portnum, SERIAL_NOECHO); /* We only receive the asicc char */
		if(c <= 0 ) 
			continue;
		switch(c) {
		case 0x08:
			if(len > 0) {
				buf[--len] = '\0';
				OSport_putc(portnum,0x08);
				OSport_putc(portnum,' ');
				OSport_putc(portnum,0x08);
			}
			break;
		case '\n':
		case '\r':
			buf[len] = '\0';
			OSport_putc(portnum,c);
			return len;
		default:
			buf[len++] = c;
			OSport_putc(portnum,c);
			break;
		}
		if(len == max_get)
			break;
	}
	return len;
}
int OSport_putc(int portnum, unsigned char ch)
{
	PORT *pserial = &serialport[portnum];
	return port_putc(ch,pserial);
}

int OSport_puts(int portnum,unsigned char *buf,int len)
{
	int i;
	for( i = 0 ; i < len ;i  ++)
		OSport_putc(portnum, buf[i]);
	return len;
}
/* 
 * $Log: serial.c,v $
 * Revision 1.5  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.4  2002/02/06 07:28:51  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 00:46:25  linfusheng
 * update
 *
 */
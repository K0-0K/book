
#define TXR             0       /*  Transmit register (WRITE) */
#define RXR             0       /*  Receive register  (READ)  */

#define IER             1       /*  Interrupt Enable          */
#define IIR             2       /*  Interrupt ID              */
#define LCR             3       /*  Line control              */
#define MCR             4       /*  Modem control             */
#define LSR             5       /*  Line Status               */
#define MSR             6       /*  Modem Status              */

#define DLL             0       /*  Divisor Latch Low         */
#define DLH             1       /*  Divisor latch High        */

/*------------------------------------------------------------------*
  Bit values held in the Interrupt Enable Register (IER).
	bit		meaning
	---		-------
	0		Interrupt when data received.
	1		Interrupt when transmitter holding reg. empty.
	2		Interrupt when data reception error.
	3		Interrupt when change in modem status register.
	4-7		Not used.
 *------------------------------------------------------------------*/
#define IER_RX_DATA         0x01
#define IER_THRE			0x02


/*------------------------------------------------------------------*
  Bit values held in the Interrupt Identification Register (IIR).
	bit		meaning
	---		-------
	0		Interrupt pending
	1-2             Interrupt ID code
			00=Change in modem status register,
			01=Transmitter holding register empty,
			10=Data received,
			11=reception error, or break encountered.
	3-7		Not used.
 *------------------------------------------------------------------*/
#define IIR_INT_BITMASK		0x06
#define IIR_MODEM_STATUS	0x00
#define IIR_TX_ID			0x01
#define IIR_RX_ID           0x04
#define	IIR_LINE_STATUS		0x06
 
 
/*-------------------------------------------------------------------*
  Bit values held in the Line Control Register (LCR).
	bit		meaning
	---		-------
	0-1		00=5 bits, 01=6 bits, 10=7 bits, 11=8 bits.
	2		Stop bits.
	3		0=parity off, 1=parity on.
	4		0=parity odd, 1=parity even.
	5		Sticky parity.
	6		Set break.
	7		Toggle port addresses.
 *-------------------------------------------------------------------*/

#define LCR_DATA_BITMASK	0x03
#define LCR_5_DATA_BITS		0x00
#define LCR_6_DATA_BITS		0x01
#define LCR_7_DATA_BITS		0x02
#define LCR_8_DATA_BITS		0x03		

#define LCR_2_STOP			0x04

#define LCR_PARITY			0x08
#define LCR_PARITY_EVEN		0x10
#define LCR_DLAB			0x80

/*-------------------------------------------------------------------*
  Bit values held in the Modem Output Control Register (MCR).
	bit     	meaning
	---		-------
	0		Data Terminal Ready. Computer ready to go.
	1		Request To Send. Computer wants to send data.
	2		auxillary output #1.
	3		auxillary output #2.(Note: This bit must be
			set to allow the communications card to send
			interrupts to the system)
	4		UART ouput looped back as input.
	5-7		not used.
 *------------------------------------------------------------------*/
#define MCR_DTR             0x01
#define MCR_RTS             0x02
#define MCR_OUT1			0x04
#define MCR_OUT2			0x08
#define MCR_INT				0x08



/*-------------------------------------------------------------------*
  Bit values held in the Line Status Register (LSR).
	bit		meaning
	---		-------
	0		Data ready.
	1		Overrun error - Data register overwritten.
	2		Parity error - bad transmission.
	3		Framing error - No stop bit was found.
	4		Break detect - End to transmission requested.
	5		Transmitter holding register is empty.
	6		Transmitter shift register is empty.
	7               Time out - off line.
 *-------------------------------------------------------------------*/
#define LSR_RCVRDY          0x01
#define LSR_OVRERR          0x02
#define LSR_PRTYERR         0x04
#define LSR_FRMERR          0x08
#define LSR_BRKERR          0x10
#define LSR_XMTRDY          0x20
#define LSR_XMTRSR          0x40
#define LSR_TIMEOUT			0x80



/*------------------------------------------------------------------*
  Bit values held in the Modem Input Status Register (MSR).
	bit		meaning
	---		-------
	0		delta Clear To Send.
	1		delta Data Set Ready.
	2		delta Ring Indicator.
	3		delta Data Carrier Detect.
	4		Clear To Send.
	5		Data Set Ready.
	6		Ring Indicator.
	7		Data Carrier Detect.
 *------------------------------------------------------------------*/
#define MSR_CTS             0x10
#define MSR_DSR             0x20


//////////////////////////////////////////////////////////////////////



/*
 * This structure defines the 256 byte buffer used for
 * I/O buffers by the SERIAL routines.  By using a buffer
 * size of 256 bytes, updating the indices is simplified.
 */
typedef struct {
                 char buffer[256];
                 unsigned char write_index;
                 unsigned char read_index;
               } BUFFER;
/*
 * This structure defines a SERIAL port.  It is initialized
 * when the port is opened with port_open().
 */
typedef struct {
                 int                   uart_base;
                 int                   irq_mask;
                 int                   interrupt_number;
                 BUFFER                in;
} PORT ;

/*
 * The fully qualified function prototypes.  All of these
 * routines actually reside in SERIAL.C
 */
PORT *port_open( int address, int interrupt_number );
void port_set( PORT *port,
               long speed,
               char parity,
               int data,
               int stopbits );
int port_putc( unsigned char c, PORT *port );
int port_getc( PORT *port );


/*
 * These are the standard UART addresses and interrupt
 * numbers for the two IBM serialpatible SERIAL ports.
 */
#define SERIAL1_UART		0x3f8
#define SERIAL1_INTERRUPT	4
#define	SERIAL_PORT_1		0 
#define SERIAL2_UART		0x2f8
#define SERIAL2_INTERRUPT	3
#define SERIAL_PORT_2		1



#define SERIAL_NOECHO		1
#define SERIAL_ECHOP		2
#define SERIAL_ECHOC		3

extern void OSport_init(void);
extern int OSport_getc(int portnum,unsigned char flag);
extern int OSport_gets(int portnum,unsigned char *buf,int max_input);
extern int OSport_putc(int portnum,unsigned char ch);
extern int OSport_puts(int portnum,unsigned char *buf,int len);

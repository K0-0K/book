/* 
 * $Id: debug.c,v 1.6 2002/02/07 01:08:53 linfusheng Exp $
 */
#include "includes.h"
#include "debug.h"
#include "serial.h"


char bigbuffer[8192];


void debug_print(const char *message,...)
{
	int len;
	va_list args;
	va_start(args,message);
	len = vsprintf(bigbuffer,message,args);
	va_end(args);
	OSport_puts(SERIAL_PORT_1,bigbuffer,len);
}

void debug_print_buf(char *msg,unsigned char *buf,int buflen)
{
	int i;
	debug_print(msg);
	debug_print("\r\n");
	for( i = 0 ; i < buflen; i  ++) {
		debug_print("%02x  ",buf[i]);
	}
	debug_print("\r\n");
}

/* 
 * $Log: debug.c,v $
 * Revision 1.6  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.5  2002/02/06 07:28:51  linfusheng
 * update
 *
 * Revision 1.4  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:00:21  linfusheng
 * update
 *
 */

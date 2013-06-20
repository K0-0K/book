/* $Id: swap.h,v 1.1 2002/02/06 02:17:07 linfusheng Exp $
 */
#ifndef __SYS_SWAP_BYTES_H
#define __SYS_SWAP_BYTES_H

#undef  ntohs
#undef  htons
#undef  ntohl
#undef  htonl
#define ntohs(x)  intel16(x)
#define htons(x)  intel16(x)
#define ntohl(x)  intel(x)
#define htonl(x)  intel(x)

extern 	unsigned long intel (unsigned long x);
extern 	unsigned short intel16 (unsigned short x);


#endif /* __SYS_SWAP_BYTES_H */

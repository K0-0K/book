/* 
 * $Id: ip.c,v 1.4 2002/02/07 01:08:53 linfusheng Exp $
 */

#include "includes.h"
#include "ether.h"
#include "nic.h"

#pragma warning (push)
#pragma warning (disable: 4761) // integral size mismatch in argument; conversion supplied
static int ip_fast_csum(struct iphdr *iph);

#define IP_DEBUG
int
iprecv(struct nbuf *nbuf)
{
	int n = nbuf->len;
	struct iphdr *iph = nbuf->nh.iph; 
	struct nic *d = nbuf->nic;
	
	errno = 0;	/* XXX */
	if (n < 0 || n < sizeof(struct iphdr)) {
		return (-1);
	}
	/*
	 *	RFC1122: 3.1.2.2 MUST silently discard any IP frame that fails the checksum.
	 *	RFC1122: 3.1.2.3 MUST discard a frame with invalid source address [NEEDS FIXING].
	 *
	 *	Is the datagram acceptable?
	 *
	 *	1.	Length at least the size of an ip header
	 *	2.	Version of 4
	 *	3.	Checksums correctly. [Speed optimisation for later, skip loopback checksums]
	 *	4.	Doesn't have a bogus length
	 *	(5.	We ought to check for IP multicast addresses and undefined types.. does this matter ?)
	 */

	if (n < sizeof(struct iphdr) || 
			  iph->ihl<5 || 
	  	   iph->version != 4 || 
	       ip_fast_csum(iph) !=0 || n < ntohs(iph->tot_len)) {
		return -1;
	}

	if(n > ntohs(iph->tot_len)) {
		n = ntohs(iph->tot_len);
	}

	if(iph->dest.s_addr == d->ip_addr.s_addr) {
		/* 生成处理 */
		switch(iph->protocol) {
		case IPPROTO_ICMP:
			nbuf->th.raw = nbuf->nh.raw + (iph->ihl << 2);
			nbuf->data += iph->ihl << 2;
			nbuf->len -= iph->ihl << 2;
			icmprecv(nbuf);
			break;
		case IPPROTO_UDP:
			nbuf->th.raw += iph->ihl << 2;
			nbuf->data += iph->ihl << 2;
			nbuf->len -= iph->ihl << 2;

		default:
			break;
		}
	}
	else if ( iph->dest.s_addr == IP_BROADCAST ){
		/*处理广播报文*/
	}
	else {
		/* 转发 */
	}



	return 0;
}


/* Check ip head sum */
int 
ip_fast_csum(struct iphdr *iph)
{
	return(0);
}

#pragma warning (pop)

/* 
 * $Log: ip.c,v $
 * Revision 1.4  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.3  2002/02/05 05:26:03  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.1  2002/01/28 00:57:30  linfusheng
 * update
 *
 */
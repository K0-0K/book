/*****************************************************************************
* neticmp.c - Network Internet Control Message Protocol program file.
*
* Copyright (c) 1997 by Global Election Systems Inc.  All rights reserved.
*
* The authors hereby grant permission to use, copy, modify, distribute,
* and license this software and its documentation for any purpose, provided
* that existing copyright notices are retained in all copies and that this
* notice and the following disclaimer are included verbatim in any 
* distributions. No written agreement, license, or royalty fee is required
* for any of the authorized uses.
*
* THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
* IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
* REVISION HISTORY
*
* 98-01-22 Guy Lancaster <lancasterg@acm.org>, Global Election Systems Inc.
*	Extracted from BSD's ip_icmp.c and icmp_var.h.
*****************************************************************************/
/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ip_icmp.c	8.2 (Berkeley) 1/4/94
 *
 *	@(#)icmp_var.h	8.1 (Berkeley) 6/10/93
 */
/*
 * ICMP routines: error generation, receive packet processing, and
 * routines to turnaround packets back to the originator, and
 * host table maintenance routines.
 */


#include "includes.h"


#include "ether.h"
#include "nic.h"
#include "neticmp.h"



#pragma warning (push)
#pragma warning (disable: 4761) // integral size mismatch in argument; conversion supplied
////////////////////////////////////////////////////////////////////////////////


/******************************/
/*** PUBLIC DATA STRUCTURES ***/
/******************************/
IcmpStats icmpStats;
static void icmpreply(struct nbuf *nbuf);
static void icmpsend(struct nbuf *nbuf);

/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/
void icmpInit(void)
{
	memset(&icmpStats, 0, sizeof(icmpStats));
}


int icmp_echo(struct nbuf *nbuf)
{
	struct icmphdr *icmph = nbuf->th.icmph;
	icmph->type = ICMP_ECHOREPLY;
	icmph->checksum = 0;

	return 0;
}

/*
 * Process a received ICMP message.
 */
int  icmprecv(struct nbuf *nbuf)
{

	struct icmphdr* icmph = nbuf->th.icmph;
	int n = nbuf->len;
	int code;
			
	/*
	 * Locate icmp structure in nBuf, and check
	 * that not corrupted and of at least minimum length.
	 */
	if (n < ICMP_MINLEN) {
		icmpStats.icps_tooshort++;
		return -1;
	}


	/* check sum */
	/* ??? */
	if(icmph->checksum) {
	}


	code = icmph->code;
	switch (icmph->type) {

	case ICMP_UNREACH:
		switch (code) {
			case ICMP_UNREACH_NET:
			case ICMP_UNREACH_HOST:
			case ICMP_UNREACH_PROTOCOL:
			case ICMP_UNREACH_PORT:
			case ICMP_UNREACH_SRCFAIL:
			case ICMP_UNREACH_NEEDFRAG:
			case ICMP_UNREACH_NET_UNKNOWN:
			case ICMP_UNREACH_NET_PROHIB:
			case ICMP_UNREACH_TOSNET:
			case ICMP_UNREACH_HOST_UNKNOWN:
			case ICMP_UNREACH_ISOLATED:
			case ICMP_UNREACH_HOST_PROHIB:
			case ICMP_UNREACH_TOSHOST:
			default:
				break;
		}
		break;	
	case ICMP_ECHO:
		icmp_echo(nbuf);
		break;

	/*
	 * No kernel processing for the following;
	 * just fall through to send to raw listener.
	 */
	case ICMP_ECHOREPLY:
		break;
	case ICMP_TIMXCEED:
	case ICMP_PARAMPROB:
	case ICMP_SOURCEQUENCH:
	case ICMP_ROUTERADVERT:
	case ICMP_ROUTERSOLICIT:
	case ICMP_TSTAMPREPLY:
	case ICMP_IREQREPLY:
	case ICMP_MASKREPLY:
	default:
		return -1;
	}


	icmpStats.icps_reflect++;
	icmpStats.icps_outhist[icmph->type]++;
	icmpreply(nbuf);

	return 0;
}


/**********************************/
/*** LOCAL FUNCTION DEFINITIONS ***/
/**********************************/
/*
 * Reflect the ip packet back to the source
 */
static void icmpreply(struct nbuf *nbuf)
{
	struct iphdr *iph = nbuf->nh.iph;
	struct nic *dev = nbuf->nic;

	
	/* Send back to source, use our address as new source. */
	iph->dest = iph->src;
	iph->src = dev->ip_addr;
	iph->ttl = 64;


	nbuf->data -= iph->ihl << 2;
	nbuf->len += iph->ihl << 2;

	icmpsend(nbuf);

}

/*
 * Send an icmp packet back to the ip level,
 * after supplying a checksum.
 */
static void icmpsend(struct nbuf *nbuf)
{

	struct ethhdr *eth_hdr = nbuf->lh.ethernet;
	struct nic *dev = nbuf->nic;

	memcpy(eth_hdr->ether_dhost,eth_hdr->ether_shost,ETH_ALEN);
	memcpy(eth_hdr->ether_shost,dev->node_addr,ETH_ALEN);

	nbuf->data -= ETH_HLEN;
	nbuf->len += ETH_HLEN;

	debug_print_buf("reply data: ",nbuf->data,nbuf->len);

	dev->transmit(dev,nbuf->data,nbuf->len);

	return ;
}





#pragma warning (pop)
////////////////////////////////////////////////////////////////////////////////

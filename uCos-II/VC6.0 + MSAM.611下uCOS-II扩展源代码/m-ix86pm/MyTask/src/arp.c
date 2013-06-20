/* 
 * $Id: arp.c,v 1.4 2002/02/07 01:08:53 linfusheng Exp $
 */
#include "includes.h"
#include "ether.h"
#include "nic.h"



#define ARP_DEBUG
int debug = 1;

/* Cache stuff */
#define ARP_NUM 8			/* need at most 3 arp entries */

#define bcopy(x,y,l)	memcpy(y,x,l)
#define bzero(x,l)	memset(x,0,l)
#define bcmp(x,y,l)	memcmp(x,y,l)
#define MACPY(s,d)	bcopy(s,d,ETH_ALEN)

struct arp_list {
	struct in_addr	addr;
	unsigned char	ea[6];
} arp_list[ARP_NUM] = {
	/* XXX - net order `INADDR_BROADCAST' must be a constant */
	{ {0xffffffff}, BA }
};
int arp_num = 1;


void arp_reply(struct nbuf *nbuf);

/* Broadcast an ARP packet, asking who has addr on interface d */
unsigned char *
arpwhohas(struct nic *d, struct in_addr addr)
{
	int i;
	struct ether_arp *ah;
	struct arp_list *al;
	struct {
		struct ethhdr eh;
		struct {
			struct ether_arp arp;
			unsigned char pad[18]; 	/* 60 - sizeof(...) */
		} data;
	} wbuf;
	struct {
		struct ethhdr eh;
		struct {
			struct ether_arp arp;
			unsigned char pad[24]; 	/* extra space */
		} data;
	} rbuf;

	/* Try for cached answer first */
	for (i = 0, al = arp_list; i < arp_num; ++i, ++al)
		if (addr.s_addr == al->addr.s_addr)
			return (al->ea);

	/* Don't overflow cache */
	if (arp_num > ARP_NUM - 1) {
		arp_num = 1;	/* recycle */
		debug_print("arpwhohas: overflowed arp_list!\r\n");
	}

#ifdef ARP_DEBUG
 	if (debug)
 	    debug_print("arpwhohas: send request for %08x\r\n", addr.s_addr);
#endif

	bzero((char*)&wbuf.data, sizeof(wbuf.data));
	ah = &wbuf.data.arp;
	ah->arp_hrd = htons(ARPHRD_ETHER);
	ah->arp_pro = htons(ETHERTYPE_IP);
	ah->arp_hln = sizeof(ah->arp_sha); /* hardware address length */
	ah->arp_pln = sizeof(ah->arp_spa); /* protocol address length */
	ah->arp_op = htons(ARPOP_REQUEST);
	MACPY(d->node_addr, ah->arp_sha);
	bcopy(&d->ip_addr, ah->arp_spa, sizeof(ah->arp_spa));
	/* Leave zeros in arp_tha */
	bcopy(&addr, ah->arp_tpa, sizeof(ah->arp_tpa));

	/* Store ip address in cache (incomplete entry). */
	al->addr = addr;



	/* send packet */


	/* Store ethernet address in cache */
	ah = &rbuf.data.arp;

	MACPY(ah->arp_sha, al->ea);
	++arp_num;

	return (al->ea);
}


static int
arpsend(struct nbuf *nbuf,unsigned char *dest_hw,unsigned short ptype)
{
	struct ethhdr *eh = nbuf->lh.ethernet;
	struct nic *d = nbuf->nic;

	nbuf->data -= ETH_HLEN;
	nbuf->len += ETH_HLEN;

	MACPY(d->node_addr, eh->ether_shost);		/* by byte */
	MACPY(dest_hw, eh->ether_dhost);			/* by byte */
	eh->ether_type = htons(ptype);

	
	if(d->transmit)
		return d->transmit(d, nbuf->data,nbuf->len);
	return 0;
}



/*
 * Returns 0 if this is the packet we're waiting for
 * else -1 (and errno == 0)
 */
int
arprecv(struct nbuf *nbuf)

{
	int n = nbuf->len;
	struct ether_arp *ah = nbuf->nh.arph;


	errno = 0;	/* XXX */
	if (n < 0 || n < sizeof(struct ether_arp)) {
		return (-1);
	}


	/* Ethernet address now checked in readether() */

	if (ah->arp_hrd != htons(ARPHRD_ETHER) ||
	    ah->arp_pro != htons(ETHERTYPE_IP) ||
	    ah->arp_hln != sizeof(ah->arp_sha) ||
	    ah->arp_pln != sizeof(ah->arp_spa) )
	{
		return (-1);
	}

	if (ah->arp_op == htons(ARPOP_REQUEST)) {
		arp_reply(nbuf);
		return (-1);
	}

	if (ah->arp_op != htons(ARPOP_REPLY)) {
		return (-1);
	}

	/* Is the reply from the source we want? */
	if (bcmp(&arp_list[arp_num].addr,
			 ah->arp_spa, sizeof(ah->arp_spa)))
	{
		return (-1);
	}

	/* We don't care who the reply was sent to. */

	/* We have our answer. */

	return (0);
}



/*
 * Convert an ARP request into a reply and send it.
 * Notes:  Re-uses buffer.  Pad to length = 46.
 */
void
arp_reply(struct nbuf *nbuf)
{
	struct ether_arp *arp = nbuf->nh.arph;
	struct nic *d = nbuf->nic;

	/* If we are not the target, ignore the request. */
	if (bcmp(arp->arp_tpa, &d->ip_addr, sizeof(arp->arp_tpa))) {
		return;
	}

	arp->arp_op = htons(ARPOP_REPLY);
	/* source becomes target */
	bcopy(arp->arp_sha, arp->arp_tha, sizeof(arp->arp_tha));
	bcopy(arp->arp_spa, arp->arp_tpa, sizeof(arp->arp_tpa));
	/* here becomes source */
	bcopy(d->node_addr,  arp->arp_sha, sizeof(arp->arp_sha));
	bcopy(&d->ip_addr, arp->arp_spa, sizeof(arp->arp_spa));

	/*
	 * No need to get fancy here.  If the send fails, the
	 * requestor will just ask again.
	*/
	(void) arpsend(nbuf,arp->arp_tha, ETHERTYPE_ARP);
}

/* 
 * $Log: arp.c,v $
 * Revision 1.4  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 00:58:45  linfusheng
 * update
 *
 */

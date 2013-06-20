 /*
  * $Id: NIC.H,v 1.6 2002/02/21 08:25:00 linfusheng Exp $
  */

#ifndef	NIC_H
#define NIC_H

//#define INCLUDE_3C90X
#define INCLUDE_RTL8139
#include "cards.h"


/*
 * Structure defining statistics and other data kept regarding a network
 * interface.
 */
struct	nic_data {
	/* generic interface information */
	unsigned char	nici_type;		/* ethernet, tokenring, etc. */
	unsigned char	nici_addrlen;		/* media address length */
	unsigned char	nici_hdrlen;		/* media header length */
	unsigned char	nici_link_state;		/* current link state */
	unsigned long	nici_mtu;		/* maximum transmission unit */
	unsigned long	nici_metric;		/* routing metric (external only) */
	unsigned long	nici_baudrate;		/* linespeed */
	/* volatile statistics */
	unsigned long	nici_ipackets;		/* packets received on interface */
	unsigned long	nici_ierrors;		/* input errors on interface */
	unsigned long	nici_opackets;		/* packets sent on interface */
	unsigned long	nici_oerrors;		/* output errors on interface */
	unsigned long	nici_collisions;		/* collisions on csma interfaces */
	unsigned long	nici_ibytes;		/* total number of octets received */
	unsigned long	nici_obytes;		/* total number of octets sent */
	unsigned long	nici_imcasts;		/* packets received via multicast */
	unsigned long	nici_omcasts;		/* packets sent via multicast */
	unsigned long	nici_iqdrops;		/* dropped on input, this interface */
	unsigned long	nici_noproto;		/* destined for unsupported protocol */
	unsigned long	nici_lastchange;	/* last operational state change */
};

/*
 *	Structure returned from eth_probe and passed to other driver
 *	functions.
 */
#define IFNAMESIZ	64
struct nic
{
	unsigned short  vendor,device;
	int		index;
	char		name[IFNAMESIZ];
	void            *pci_data;

	unsigned  long	ioaddr;
	int             irq;
	unsigned short	flags;	/* nic flags */
	unsigned char   node_addr[ETH_ALEN];
	struct in_addr	ip_addr;
	struct in_addr	netmask;


	int		timeout;

	struct	nic_data nic_data;/* stats and other data about nic */

	
	int		(*ioctl)(struct nic *,unsigned long cmd,caddr_t data);
        int             (*start)(struct nic *);
//	void		(*reset)(struct nic *);
        int             (*recv)(struct nic *,unsigned char *buf,int *len);
	int		(*transmit)(struct nic *, char *p,int plen);
	void		(*watchdog)(struct nic *);
};



/*
 * Interface request structure used for socket
 * ioctl's.  All interface ioctl's must have parameter
 * definitions which begin with ifr_name.  The
 * remainder may be interface specific.
 */
struct	ifreq {
	char	ifr_name[IFNAMESIZ];		/* if name, e.g. "en0" */
	union {
		struct in_addr ifru_addr;
		struct in_addr ifru_dstaddr;
		struct in_addr ifru_broadaddr;
		short	ifru_flags;
		int	ifru_metric;
		caddr_t	ifru_data;
	} ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags */
#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define	ifr_mtu		ifr_ifru.ifru_metric	/* mtu (overload) */
#define	ifr_media	ifr_ifru.ifru_metric	/* media options (overload) */
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
};



struct nbuf{
	unsigned char packet[1536];
	int packetlen;

	unsigned char *data;
	int len;

	struct nic *nic;
	/* Transport layer header */
	union
	{
		struct tcphdr	*th;
		struct udphdr	*uh;
		struct icmphdr	*icmph;
		unsigned char	*raw;
	} th;

	/* Network layer header */
	union
	{
		struct iphdr	*iph;
		struct ether_arp *arph;
		unsigned char	*raw;
	} nh;
  
	/* Link layer header */
	union 
	{	
	  	struct ethhdr	*ethernet;
	  	unsigned char 	*raw;
	} lh;
};


extern void netif_slowtimo(void *arg);
extern void netif_init(void);
extern void netif_rx(unsigned char *packet,int packet_len,struct nic *nic);
extern void netif_bh(void);
extern int netif_ioctl(u_long cmd,caddr_t data);
#endif	/* NIC_H */

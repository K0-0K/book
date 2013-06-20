/* 
 * $Id: NIC.C,v 1.7 2002/02/21 08:25:00 linfusheng Exp $
 */
#include "includes.h"
#include "pci.h"
#include "pci_ids.h"
#include "ether.h"
#include "nic.h"
#include "sockios.h"
#include "br.h"






#define RX_RING_SIZE	16

struct nbuf rx_ring[RX_RING_SIZE];

unsigned char in_read_index = 0,in_write_index = 0;



//#define NIC_DEBUG


#define N_NET_DEVICE	8

struct nic net_device[N_NET_DEVICE];
int net_device_found = 0;


struct devprobe
{
	int (*probe)(struct nic *dev);
	int status;	/* non-zero if autoprobe has failed */
};

struct devprobe nic_probes[] = {
#ifdef INCLUDE_RTL8139
	{rtl8139_probe,0},
#endif
	{NULL, 0}
};




/*
 * probe_list walks a list of probe functions and calls each so long
 * as a non-zero ioaddr is given, or as long as it hasn't already failed 
 * to find a card in the past (as recorded by "status") when asked to
 * autoprobe (i.e. a probe that fails to find a card when autoprobing
 * will not be asked to autoprobe again).  It exits when a card is found.
 */
static int  nic_probe(struct devprobe *plist)
{
	int i;
	struct devprobe *p = plist;
	struct nic *dev = &net_device[0];
	// we sugguest that size of net_device is enought 
	while (p->probe != NULL) {
		if (p->status == 0) {		/* has autoprobe failed yet? */
			p->status = p->probe(dev);	/* no, try autoprobe */
			if (p->status > 0) {
				for( i = 0 ; i < p->status ; i ++) {	
					sprintf(dev->name,"eth%d",net_device_found);	
					debug_print("found ether interface %s \r\n",dev->name);
					dev->index = net_device_found + 1;
					br_register_port(dev);
					dev ++;
					net_device_found ++;


				}
			}
		}
		p++;
	}
	return -1;
}

void netif_rx(unsigned char *packet,int packet_len,struct nic *nic)
{
	if((in_write_index + 1) % RX_RING_SIZE != in_read_index) {
		memcpy(rx_ring[in_write_index].packet,packet,packet_len);
		rx_ring[in_write_index].packetlen = packet_len;
		rx_ring[in_write_index].nic = nic;
		in_write_index ++;
		in_write_index %= RX_RING_SIZE;

		OSTaskResume(NET_PRIO);	//唤醒网络处理任务
	}
	else 
		debug_print("net buffer have fulled .... \r\n");
}


void netif_bh(void)
{

	struct ethhdr *eth_hdr = NULL;
	struct nbuf *nbuf;
	unsigned short type = 0;

	
	if (  in_read_index == in_write_index) {
		OSTaskSuspend(NET_PRIO); //挂起网络处理任务	
		return;
	}

	nbuf = &rx_ring[in_read_index];
#if 0
	br_recv(rx_ring[in_read_index].nic,
		 rx_ring[in_read_index].packet,
		 rx_ring[in_read_index].packetlen);
#else
	nbuf->data = nbuf->packet;
	nbuf->len = nbuf->packetlen;

	nbuf->lh.raw = nbuf->data;

	eth_hdr = nbuf->lh.ethernet;
	type = ntohs(eth_hdr->ether_type);


	nbuf->nh.raw = nbuf->lh.raw + ETH_HLEN;
	nbuf->data += ETH_HLEN;
	nbuf->len -= ETH_HLEN;

	switch(type) {
	case ETHERTYPE_IP:

		iprecv(nbuf);
		break;
	case ETHERTYPE_ARP:

		arprecv(nbuf);
		break;
	default:
		break;
	}

#endif
	in_read_index++ ;
	in_read_index %= RX_RING_SIZE;
	return;
}


struct nic *netif_lookup_by_name(char *name)
{
	int i;
	for( i = 0 ;i  < net_device_found ; i ++) {
		if(strncmp(net_device[i].name,name,IFNAMESIZ) == 0)
			return &net_device[i];	
	}
 	return NULL;
}

 

int
netif_ioctl(u_long cmd,caddr_t data)
{
	struct nic *dev;
	struct ifreq *ifr = (struct ifreq *)data;
	if((dev = netif_lookup_by_name(ifr->ifr_name)) == NULL)
		return -1;
	switch(cmd) {
	case 1:
		debug_print("get command code 1 \r\n");
		dev->ip_addr = ifr->ifr_addr;
		break;
	default:
		break;
	}
	return 0;
}


void netif_slowtimo(void *arg)
{
	int i;
	for( i = 0 ;i  < net_device_found ; i ++) {
		if(net_device[i].timeout == 0 )
			continue;
		if(net_device[i].timeout > 0 ) {
			net_device[i].timeout --;
		}
		if(net_device[i].timeout == 0 && net_device[i].watchdog) {
			(*net_device[i].watchdog)(&net_device[i]);
		}
	}
}

void netif_init(void)
{
	nic_probe(nic_probes);
}

/* 
 * $Log: NIC.C,v $
 * Revision 1.7  2002/02/21 08:25:00  linfusheng
 * update
 *
 * Revision 1.6  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.5  2002/02/06 02:16:25  linfusheng
 * update
 *
 * Revision 1.4  2002/02/05 05:26:03  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 00:46:25  linfusheng
 * update
 *
 */
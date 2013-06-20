/*
 * $Id: br.c,v 1.2 2002/02/21 08:25:00 linfusheng Exp $
 */

#include "includes.h"
#include "ether.h"
#include "nic.h"
#include "br.h"

//#define BR_DEBUG


static int br_flood(void *pkt,int len, int port);


static unsigned char bridge_ula[ETH_ALEN] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x00 };
static Bridge_data     bridge_info;			  /* (4.5.3)	 */
Port_data       port_info[All_ports];		  /* (4.5.5)	 */


/* MAG: Maximum port registered - used to speed up flooding and to make
 * have a large ports array more efficient
 */
static int max_port_used = 0; 

/* JRP: fdb cache 1/port save kmalloc/kfree on every frame */
/* entries timeout after this many seconds */
unsigned int fdb_aging_time = FDB_TIMEOUT; 

struct br_stat br_stats;
#define br_stats_cnt br_stats.packet_cnts



void br_register_port(struct nic *dev)
{
	port_info[ max_port_used ].dev = dev;
	debug_print("register bridge port %d ,dev index %d , dev name %s \r\n",
			max_port_used,dev->index,dev->name);
	max_port_used ++;
}

static void br_init_port(int port_no)
{
	int i;
	port_info[port_no].state = Forwarding;

	for( i = 0 ; i < 256 ; i ++) {
		port_info[port_no].fdb[i].have_free = 1;
	}

}



void br_init(void)
{						  /* (4.8.1)	 */
	int port_no;

	debug_print("NET4: Ethernet Bridge 007 for NET4.0\r\n");

	/* Set up brg device information */
	bridge_info.instance = 0;

	
	
	/*
	 * Form initial topology change time.
	 * The topology change timer is only used if this is the root bridge.
	 */
	
	bridge_info.topology_change_time = BRIDGE_MAX_AGE + BRIDGE_FORWARD_DELAY;       /* (4.5.3.13) */

	bridge_info.designated_root = bridge_info.bridge_id;	/* (4.8.1.1)	 */
	bridge_info.root_path_cost = Zero;
	bridge_info.root_port = No_port;


	bridge_info.bridge_max_age = BRIDGE_MAX_AGE;
	bridge_info.bridge_hello_time = BRIDGE_HELLO_TIME;
	bridge_info.bridge_forward_delay = BRIDGE_FORWARD_DELAY;
	bridge_info.hold_time = HOLD_TIME;

	bridge_info.max_age = bridge_info.bridge_max_age;	/* (4.8.1.2)	 */
	bridge_info.hello_time = bridge_info.bridge_hello_time;
	bridge_info.forward_delay = bridge_info.bridge_forward_delay;

	bridge_info.top_change_detected = 0;
	bridge_info.top_change = 0;

	max_port_used = One;
	
	for (port_no = One; port_no <= No_of_ports; port_no++) {	/* (4.8.1.4) */
		/* initial state = Disable */
		br_init_port(port_no);
	}



	br_stats.flags = 0; /*BR_UP | BR_DEBUG*/;	/* enable bridge */
	br_stats.policy = BR_ACCEPT;			/* Enable bridge to accpet all protocols */
	br_stats.exempt_protocols = 0;

	/* Vova Oksman: register the function for the PROCfs "bridge" file */

}






/*
 * this routine returns 0 when it learns (or updates) from the
 * frame, and 1 if we must dropped the frame due to multicast
 * limitations, or -1 because of not enough memory.
 *
 * NB Can be called when skb->nh.raw is NOT set up when
 * receiving frames on brg0 via brg_rx
 */

static int br_learn(struct ethhdr *eth, int port)	/* 3.8 */
{
	int i;
	struct fdb *f = NULL;
	Port_data  *p = &port_info[port];


	/* JRP: no reason to check port state again. We are called by
	 * br_receive_frame() only when in Learning or Forwarding
	 * Remark: code not realigned yet to keep diffs smaller
	 */

	/* don't keep group addresses in the tree */
	if (eth->ether_dhost[0] & 0x01)
		return 0;

	/* Get a free fdb */
	for( i = 0 ; i < 256; i ++) {
		if(!(p->fdb[i].have_free) && memcmp(p->fdb[i].ula,eth->ether_shost,ETH_ALEN) == 0) {
			f = &p->fdb[i];
			break;
		}
		if(p->fdb[i].have_free == 1 && f == NULL) {
			f = &p->fdb[i];
		}
	}

	if(f == NULL) {
		debug_print("in %d port , have no space to save new hwaddr . \r\n");
		return 0;
	}

	f->port = port;	/* source port */
	memcpy(f->ula, eth->ether_shost, 6);
	f->timer = CURRENT_TIME;
	f->flags = FDB_ENT_VALID;
	f->have_free = 0;

	return 0;
}

struct fdb *br_find_addr(unsigned char addr[6])
{
	int i,j;
	Port_data *p;
	struct fdb *f;
#ifdef BR_DEBUG
	debug_print("br_find_addr be call.... \r\n");
#endif
	
	for( i = 1 ;i < max_port_used; i ++) {
		for( j = 0 ; j < 256 ; j ++) {
			p = &port_info[i];
			f = &p->fdb[j];
			if(!(f->have_free) && memcmp(f->ula,addr,6) == 0)
				return f;
		}
	}

	return NULL;
}
/*
 * Forward the frame SKB to proper port[s].  PORT is the port that the
 * frame has come from; we will not send the frame back there.  PORT == 0
 * means we have been called from br_tx_fr(), not from br_receive_frame().
 *
 * this routine returns 1 if it consumes the frame, 0
 * if not...
 */

static int br_forward(void *pkt, int len, int port)	/* 3.7 */
{
	struct fdb *f;
	struct ethhdr *eth;
	eth = (struct ethhdr*)pkt;	
#ifdef BR_DEBUG
	debug_print("br_forward be call.... \r\n");
#endif

	
	
	/*
   	 * flood all ports with frames destined for a group
	 * address.  If frame came from above, drop it,
	 * otherwise it will be handled in br_receive_frame()
	 * Multicast frames will also need to be seen
	 * by our upper layers.
	 */	
	if (eth->ether_dhost[0] & 0x01) 
	{
#ifdef BR_DEBUG
		debug_print("dest is group address .... \r\n");
#endif


                /* group address */
                br_flood(pkt,len, port);
                /*
                 *      External groups are fed out via the normal source
                 *      This probably should be dropped since the flood will
                 *      have sent it anyway.
                 */
                if (port == 0)
                {
                        /* Locally generated */
                        ++br_stats_cnt.local_multicast;
                        return(0);
                }
                ++br_stats_cnt.forwarded_multicast;
		return(0);
	}
	else 
	{
		/* unicast frame, locate port to forward to */
		f = br_find_addr(eth->ether_dhost);
		/*
		 *	Send flood and drop.
		 */
		if (!f || !(f->flags & FDB_ENT_VALID)) 
		{
	 		if(f && (f->port == 0)) 
	 		{
				++br_stats_cnt.forwarded_unicast_up_stack;
#ifdef BR_DEBUG
				debug_print("br: port is invaild .. \r\n");
#endif

				return(0);
			}
			/* not found or too old; flood all ports */
			++br_stats_cnt.flood_unicast;

			debug_print("br_flood packet ... \r\n");

			br_flood(pkt,len, port);

			return(0);
		}

		/*
		 *	Sending
		 */
		if (f->port!=port && port_info[f->port].state == Forwarding) 
		{
			struct nic *dev;
#ifdef BR_DEBUG
			debug_print("br: the dest in diffrent port , so forward it .. \r\n");
#endif

			/* Has entry expired? */
			if (f->timer + fdb_aging_time < CURRENT_TIME) 
			{
#ifdef BR_DEBUG
				debug_print("br: port have invaild .. \r\n");
#endif
				/* timer expired, invalidate entry */
				f->flags &= ~FDB_ENT_VALID;
				/*
				 *	Send flood and drop original
				 */
				++br_stats_cnt.aged_flood_unicast;
				br_flood(pkt,len, port);
				return(0);
			}

			++br_stats_cnt.forwarded_unicast;

			/*
			 *	Send the buffer out.
			 */
			 
			dev = port_info[f->port].dev;

#ifdef BR_DEBUG
			debug_print("br: every thing is ok , forward it  .. \r\n");
#endif
			dev->transmit(dev,pkt,len);

			return(1);	/* skb has been consumed */
		}
		else 
		{

#ifdef BR_DEBUG
			debug_print("br: dest in same port .. \r\n");
#endif
			/* JRP: Needs to aged entry as well, if topology changes
			 * the entry would not age. Got this while swapping
			 * two cables !
			 *
			 *	Has entry expired?
			 */
			 
			if (f->timer + fdb_aging_time < CURRENT_TIME) 
			{
				/* timer expired, invalidate entry */
				f->flags &= ~FDB_ENT_VALID;
				++br_stats_cnt.drop_same_port_aged;
			}
			else 
				++br_stats_cnt.drop_same_port;
			/*
			 *	Arrived on the right port, we discard
			 */
			return(0);
		}
	}

}
/*
 * this routine sends a copy of the frame to all forwarding ports
 * with the exception of the port given.  This routine never
 * consumes the original frame.
 */
	
static int br_flood(void *pkt,int len, int port)
{
	int i;
	struct nic *dev;
#ifdef BR_DEBUG
	debug_print("br_flood be call. . \r\n");
#endif
	for (i = One; i < max_port_used; i++) 
	{
#ifdef BR_DEBUG
		debug_print("flood packet on port %d \r\n", i);
#endif
		if (i == port)	/* don't send back where we got it */
			continue;
		if (port_info[i].state == Forwarding) 
		{
			dev= port_info[i].dev;
			/* To get here we must have done ARP already,
			   or have a received valid MAC header */
			dev->transmit(dev,pkt,len);
		}
	}
#ifdef BR_DEBUG
	debug_print("br_flood finished ... \r\n");
#endif
	return(0);
}


int
br_recv(struct nic *d, void *pkt, int len)
{
	int port = 0,ret = 0;
	Port_data *p;
	struct ethhdr *eth;
	eth = (struct ethhdr*)pkt;

#ifdef BR_DEBUG
	debug_print("br: br_recv be found .. \r\n");
#endif

	port = d->index;
	if(!port) {
#ifdef BR_DEBUG
		debug_print("br: found zero port .. \r\n");
#endif
		return 0;
	}

	p = &port_info[port];
#ifdef BR_DEBUG
	debug_print("br: frame come from %d port \r\n", port);
	debug_print_buf("the data to be sent :",pkt,len);

#endif

	if(p->state == Disabled) {
 		/* We are here if BR_UP even if this port is Disabled.
 		 * Send everything up
 		 */
#ifdef BR_DEBUG
		debug_print("br: %d port state is Disabled  .. \r\n",port);
#endif

		++br_stats_cnt.port_disable_up_stack;
		return 0;
		
	}

 	/* Here only if not disable.
 	 * Remark: only frames going up will show up in NIT (tcpdump)
 	 */

	/* JRP: even if port is Blocking we need to process the Spanning Tree
	 * frames to keep the port in that state
	 */
	if (memcmp(eth->ether_dhost, bridge_ula, ETH_ALEN) == 0) 
	{
		++br_stats_cnt.rcv_bpdu;
#ifdef BR_DEBUG
		debug_print("br: why the dest of frame is bridge  .. \r\n");
#endif

		/* br_bpdu consumes skb */
		return(1);
	}	


	switch (p->state) 
	{
		case Learning:
			/* fall through */
		case Listening:
			/* fall through */
		case Blocking:
			++br_stats_cnt.notForwarding;
#ifdef BR_DEBUG
			debug_print("br: We have to blocking the frame .. \r\n");
#endif

			return 0;
		case Forwarding:

			if((ret = br_learn(eth, port))) {	/* 3.8 */
				if (ret > 0) {
					++br_stats_cnt.drop_multicast;
				}
#ifdef BR_DEBUG
				debug_print("br: br_learn return %d code .. \r\n",ret);
#endif

				return 0;
			}
			/* Now this frame came from one of bridged
			   ports this means we should attempt to forward it.
			   JRP: local addresses are now in the AVL tree,
			   br_forward will pass frames up if it matches
			   one of our local MACs or if it is a multicast
			   group address.
			   br_forward() will not consume the frame if this
			   is the case */
			return(br_forward(pkt,len,port));
				
		
		default:
			++br_stats_cnt.unknown_state;
			debug_print("br_receive_frame: port [%i] unknown state [%i]\r\n",
					port, p->state);
			return(0);	/* discard frame */
	}




	return 0;
}



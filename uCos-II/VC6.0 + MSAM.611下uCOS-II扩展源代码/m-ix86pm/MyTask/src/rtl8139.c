/* 
 * $Id: rtl8139.c,v 1.10 2002/02/21 08:25:00 linfusheng Exp $
 */


#include "includes.h"
#include "ether.h"
#include "nic.h"
#include "pci.h"
#include "pci_ids.h"

#include "ucos_timer.h"
#include "interrupt.h"


#define RTL_TIMEOUT (1*18)


//#define USE_IO_OPS
		

#ifdef USE_IO_OPS

#define RTL_R8(reg)		UCOS_INB (((unsigned long)ioaddr) + (reg))
#define RTL_R16(reg)		UCOS_INW (((unsigned long)ioaddr) + (reg))
#define RTL_R32(reg)		((unsigned long) UCOS_IND (((unsigned long)ioaddr) + (reg)))
#define RTL_W8(reg, val8)	UCOS_OUTB ((val8), ((unsigned long)ioaddr) + (reg))
#define RTL_W16(reg, val16)	UCOS_OUTW ((val16), ((unsigned long)ioaddr) + (reg))
#define RTL_W32(reg, val32)	UCOS_OUTD ((val32), ((unsigned long)ioaddr) + (reg))
#define RTL_W8_F		RTL_W8
#define RTL_W16_F		RTL_W16
#define RTL_W32_F		RTL_W32
#else
#define RTL_R8(reg)		readb(((unsigned long)ioaddr) + (reg))
#define RTL_R16(reg)		readw(((unsigned long)ioaddr) + (reg))
#define RTL_R32(reg)		((unsigned long) readl (((unsigned long)ioaddr) + (reg)))
#define RTL_W8(reg, val8)	writeb ((val8), ((unsigned long)ioaddr) + (reg))
#define RTL_W16(reg, val16)	writew ((val16), ((unsigned long)ioaddr) + (reg))
#define RTL_W32(reg, val32)	writel ((val32), ((unsigned long)ioaddr) + (reg))
#define RTL_W8_F		RTL_W8
#define RTL_W16_F		RTL_W16
#define RTL_W32_F		RTL_W32


#endif







#undef DEBUG_RTL8139

//#define DEBUG_RTL8139




/* Symbolic offsets to registers. */
enum RTL8139_registers {
	MAC0=0,			/* Ethernet hardware address. */
	MAR0=8,			/* Multicast filter. */
	TxStatus0=0x10,		/* Transmit status (four 32bit registers). */
	TxAddr0=0x20,		/* Tx descriptors (also four 32bit). */
	RxBuf=0x30, RxEarlyCnt=0x34, RxEarlyStatus=0x36,
	ChipCmd=0x37, RxBufPtr=0x38, RxBufAddr=0x3A,
	IntrMask=0x3C, IntrStatus=0x3E,
	TxConfig=0x40, RxConfig=0x44,
	Timer=0x48,		/* general-purpose counter. */
	RxMissed=0x4C,		/* 24 bits valid, write clears. */
	Cfg9346=0x50, Config0=0x51, Config1=0x52,
	TimerIntrReg=0x54,	/* intr if gp counter reaches this value */
	MediaStatus=0x58,
	Config3=0x59,
	MultiIntr=0x5C,
	RevisionID=0x5E,	/* revision of the RTL8139 chip */
	TxSummary=0x60,
	MII_BMCR=0x62, MII_BMSR=0x64, NWayAdvert=0x66, NWayLPAR=0x68,
	NWayExpansion=0x6A,
	DisconnectCnt=0x6C, FalseCarrierCnt=0x6E,
	NWayTestReg=0x70,
	RxCnt=0x72,		/* packet received counter */
	CSCR=0x74,		/* chip status and configuration register */
	PhyParm1=0x78,TwisterParm=0x7c,PhyParm2=0x80,	/* undocumented */
	/* from 0x84 onwards are a number of power management/wakeup frame
	 * definitions we will probably never need to know about.  */
};

enum ChipCmdBits {
	CmdReset=0x10, CmdRxEnb=0x08, CmdTxEnb=0x04, RxBufEmpty=0x01, };

/* Interrupt register bits, using my own meaningful names. */
enum IntrStatusBits {
	PCIErr=0x8000, PCSTimeout=0x4000, CableLenChange= 0x2000,
	RxFIFOOver=0x40, RxUnderrun=0x20, RxOverflow=0x10,
	TxErr=0x08, TxOK=0x04, RxErr=0x02, RxOK=0x01,

	RxAckBits = RxFIFOOver | RxOverflow | RxOK,
};
enum TxStatusBits {
	TxHostOwns=0x2000, TxUnderrun=0x4000, TxStatOK=0x8000,
	TxOutOfWindow=0x20000000, TxAborted=0x40000000,
	TxCarrierLost=0x80000000,

};
enum RxStatusBits {
	RxMulticast=0x8000, RxPhysical=0x4000, RxBroadcast=0x2000,
	RxBadSymbol=0x0020, RxRunt=0x0010, RxTooLong=0x0008, RxCRCErr=0x0004,
	RxBadAlign=0x0002, RxStatusOK=0x0001,
};

enum MediaStatusBits {
	MSRTxFlowEnable=0x80, MSRRxFlowEnable=0x40, MSRSpeed10=0x08,
	MSRLinkFail=0x04, MSRRxPauseFlag=0x02, MSRTxPauseFlag=0x01,
};

enum MIIBMCRBits {
	BMCRReset=0x8000, BMCRSpeed100=0x2000, BMCRNWayEnable=0x1000,
	BMCRRestartNWay=0x0200, BMCRDuplex=0x0100,
};

enum CSCRBits {
	CSCR_LinkOKBit=0x0400, CSCR_LinkChangeBit=0x0800,
	CSCR_LinkStatusBits=0x0f000, CSCR_LinkDownOffCmd=0x003c0,
	CSCR_LinkDownCmd=0x0f3c0,
};

/* Bits in RxConfig. */
enum rx_mode_bits {
	RxCfgWrap=0x80,
	AcceptErr=0x20, AcceptRunt=0x10, AcceptBroadcast=0x08,
	AcceptMulticast=0x04, AcceptMyPhys=0x02, AcceptAllPhys=0x01,
};


static const unsigned short rtl8139_intr_mask =
	PCIErr | PCSTimeout | RxUnderrun | RxOverflow | RxFIFOOver |
	TxErr | TxOK | RxErr | RxOK;

/* Maximum events (Rx packets, etc.) to handle at each interrupt. */
static int max_interrupt_work = 20;


#define N_RTL8139	4

struct rtl_private_data{
	struct nic *nic;
};

/* PCI Tuning Parameters
   Threshold is bytes transferred to chip before transmission starts. */

struct rtl_private_data private_datas[N_RTL8139];
static unsigned long  ioaddr;


#define TX_FIFO_THRESH 256      /* In bytes, rounded down to 32 byte units. */
#define RX_FIFO_THRESH  4       /* Rx buffer level before first PCI xfer.  */
#define RX_DMA_BURST    4       /* Maximum PCI burst, '4' is 256 bytes */
#define TX_DMA_BURST    4       /* Calculate as 16<<val. */
#define NUM_TX_DESC     4       /* Number of Tx descriptor registers. */
#define TX_BUF_SIZE	MAX_ETH_FRAME_SIZE	/* FCS is added by the chip */
#define RX_BUF_LEN_IDX 0	/* 0, 1, 2 is allowed - 8,16,32K rx buffer */
#define RX_BUF_LEN (8192 << RX_BUF_LEN_IDX)

static unsigned int cur_rx,cur_tx;
/* The RTL8139 can only transmit from a contiguous, aligned memory block.  */
static unsigned char tx_buffer[TX_BUF_SIZE];
static unsigned char rx_ring[RX_BUF_LEN+16];



int read_eeprom(int location);
static void rtl_reset(struct nic* nic);
static int rtl_recv(struct nic *nic,unsigned char *packet,int *packetlen);
static int rtl_transmit(struct nic *nic,  char *data,int len);
static int rtl_ioctl(struct nic *nic,u_long cmd,caddr_t data);

/* Serial EEPROM section. */

/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK    0x04    /* EEPROM shift clock. */
#define EE_CS           0x08    /* EEPROM chip select. */
#define EE_DATA_WRITE   0x02    /* EEPROM chip data in. */
#define EE_WRITE_0      0x00
#define EE_WRITE_1      0x02
#define EE_DATA_READ    0x01    /* EEPROM chip data out. */
#define EE_ENB          (0x80 | EE_CS)

/*
	Delay between EEPROM clock transitions.
	No extra delay is needed with 33Mhz PCI, but 66Mhz may change this.
*/

#define eeprom_delay()  RTL_R8(Cfg9346)

/* The EEPROM commands include the alway-set leading bit. */
#define EE_WRITE_CMD    (5 << 6)
#define EE_READ_CMD     (6 << 6)
#define EE_ERASE_CMD    (7 << 6)

static int read_eeprom(int location)
{
	int i;
	unsigned int retval = 0;
	long ee_addr = ioaddr + Cfg9346;
	int read_cmd = location | EE_READ_CMD;

	RTL_W8(Cfg9346,EE_ENB & ~EE_CS);
	RTL_W8(Cfg9346,EE_ENB);

	/* Shift the read command bits out. */
	for (i = 10; i >= 0; i--) {
		int dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		RTL_W8(Cfg9346,EE_ENB | dataval);
		eeprom_delay();
		RTL_W8(Cfg9346,EE_ENB | dataval | EE_SHIFT_CLK);
		eeprom_delay();
	}
	RTL_W8(Cfg9346,EE_ENB);
	eeprom_delay();

	for (i = 16; i > 0; i--) {
		RTL_W8(Cfg9346,EE_ENB | EE_SHIFT_CLK);
		eeprom_delay();
		retval = (retval << 1) | ((RTL_R8(Cfg9346) & EE_DATA_READ) ? 1 : 0);
		RTL_W8(Cfg9346,EE_ENB);
		eeprom_delay();
	}

	/* Terminate the EEPROM access. */
	RTL_W8(Cfg9346,~EE_CS);

	return retval;
}


static void rtl8139_rx_interrupt (struct rtl_private_data *private_data)
{
#ifdef	DEBUG_RTL8139
	debug_print("enter rx_interrupt process \r\n ! ");
#endif
#ifdef	DEBUG_RTL8139
	debug_print("In rtl8139_rx(), current %4.4x BufAddr %4.4x,"
		 " free to %4.4x, Cmd %2.2x.\r\n", cur_rx,
		 RTL_R16 (RxBufAddr),
		 RTL_R16 (RxBufPtr), RTL_R8 (ChipCmd));
#endif
	while ((RTL_R8 (ChipCmd) & RxBufEmpty) == 0) {
		int ring_offset = cur_rx % RX_BUF_LEN;
		DWORD rx_status;
		unsigned int rx_size;
		unsigned int pkt_size;

		/* read size+status of next frame from DMA ring buffer */
		rx_status = (*(DWORD *) (rx_ring + ring_offset));
		rx_size = rx_status >> 16;
		pkt_size = rx_size - 4;

#ifdef	DEBUG_RTL8139
		debug_print ("rtl8139_rx() status %4.4x, size %4.4x,"
			 " cur %4.4x.\r\n", rx_status,
			 rx_size, cur_rx);

#endif
		/* Packet copy from FIFO still in progress.
		 * Theoretically, this should never happen
		 * since EarlyRx is disabled.
		 */
		if (rx_size == 0xfff0) {
//			tp->xstats.early_rx++;
			break;

		}

		/* If Rx err or invalid rx_size/rx_status received
		 * (which happens if we get lost in the ring),
		 * Rx process gets reset, so we abort any further
		 * Rx processing.
		 */
		if ((rx_size > (MAX_ETH_FRAME_SIZE+4)) ||
		    (rx_size < 8) ||
		    (!(rx_status & RxStatusOK))) {
//			rtl8139_rx_err (rx_status, dev, tp, ioaddr);
			return;
		}

		/* Malloc up new buffer, compatible with net-2e. */
		/* Omit the four octet CRC from the length. */

		/* TODO: consider allocating skb's outside of
		 * interrupt context, both to speed interrupt processing,
		 * and also to reduce the chances of having to
		 * drop packets here under memory pressure.
		 */
		netif_rx(&rx_ring[ring_offset + 4], pkt_size,private_data->nic);

		cur_rx = (cur_rx + rx_size + 4 + 3) & ~3;
		RTL_W16 (RxBufPtr, cur_rx - 16);

		if (RTL_R16 (IntrStatus) & RxAckBits)
			RTL_W16_F (IntrStatus, RxAckBits);
	}

	return;
}

static void rtl8139_weird_interrupt(struct rtl_private_data *private_data)
{
#ifdef	DEBUG_RTL8139
	debug_print("enter weird_interrupt process \r\n !");
#endif
	return;
}
static void rtl8139_tx_interrupt (struct rtl_private_data *private_data)
{
//	unsigned long txstatus;

	debug_print("rtl_tx_inter() \r\n");

#ifdef	DEBUG_RTL8139
	debug_print("enter tx_interrupt process ! \r\n ");
#endif

//	txstatus = RTL_R32 (TxStatus0);

	/* Clear timeout flag */
#ifdef DEBUG_RTL8139
	debug_print("interrupt from %s \r\n",private_data->nic->name);
#endif
	private_data->nic->timeout = 0;


	return;
}

int rtl_interrupt(void *arg)
{
	struct rtl_private_data *private_data = (struct rtl_private_data *)arg;
	struct nic *nic = private_data->nic;
	int ackstat, status;
	int boguscnt = max_interrupt_work;
	int link_changed = 0; /* avoid bogus "uninit" warning */

	/* Mask the bit that says "this is an io addr" */
	ioaddr = nic->ioaddr;
	
	do {
		status = RTL_R16 (IntrStatus);

		/* h/w no longer present (hotplug?) or major error, bail */
		if (status == 0xFFFF)
			break;

		if ((status &
		     (PCIErr | PCSTimeout | RxUnderrun | RxOverflow |
		      RxFIFOOver | TxErr | TxOK | RxErr | RxOK)) == 0)
			break;
		/* Acknowledge all of the current interrupt sources ASAP, but
		   an first get an additional status bit from CSCR. */
		if (status & RxUnderrun)
			link_changed = RTL_R16 (CSCR) & CSCR_LinkChangeBit;


		ackstat = status & ~(RxAckBits | TxErr);
		RTL_W16 (IntrStatus, ackstat);
#ifdef	DEBUG_RTL8139
		debug_print("interrupt  status=%#4.4x ackstat=%#4.4x new intstat=%#4.4x.\r\n",
			   status,ackstat, RTL_R16 (IntrStatus));
#endif
		if (status & RxAckBits)
			rtl8139_rx_interrupt (private_data);

		/* Check uncommon events with one test. */
		if (status & (PCIErr | PCSTimeout | RxUnderrun | RxOverflow |
		  	      RxFIFOOver | RxErr))
			rtl8139_weird_interrupt (private_data);

		if (status & (TxOK | TxErr)) {
			rtl8139_tx_interrupt (private_data);
			if (status & TxErr)
				RTL_W16 (IntrStatus, TxErr);
		}

		boguscnt--;
	}while (boguscnt > 0);

	if (boguscnt <= 0) {
#ifdef	DEBUG_RTL8139
		debug_print ("Too much work at interrupt, "
			"IntrStatus=0x%4.4x.\n", status);
#endif
		/* Clear all interrupt sources. */
		RTL_W16 (IntrStatus, 0xffff);
	}
	
	_outp( ICR_A, EOI );
	_outp( ICR_B, EOI );

	return 0;
}



int 
rtl8139_probe(struct nic *nic)
{
	int i;
	struct pci_device *p = NULL;
	int speed10, fullduplex;
	int found = 0;
	p = lookup_pci_device(PCI_VENDOR_ID_REALTEK,PCI_DEVICE_ID_REALTEK_8139);
	while(p&& found < N_RTL8139){
	
		nic->pci_data = p;
#ifdef USE_IO_OPS
		nic->ioaddr = p->ioaddr & ~3;
#else
		nic->ioaddr = p->membase;
#endif

		nic->irq = p->irq;
		nic->device = p->device;
		nic->vendor = p->vendor;
	
	
		/*
		 * This bit below is not necessary at all since the pci.c subsystem
		 * is supposed to find the NIC, but I will leave it in since it
		 * hardly will ever execute this test.
		 */
		if (nic->ioaddr == 0) {
			return -1;
		}

		/* Mask the bit that says "this is an io addr" */
		ioaddr = nic->ioaddr;

		/* Bring the chip out of low-power mode. */
		RTL_W8(Cfg9346,0x00);

		if (read_eeprom(0) != 0xffff) {
			unsigned short *ap = (unsigned short*)nic->node_addr;
			for (i = 0; i < 3; i++)
				*ap++ = read_eeprom(i + 7);
		} else {
			unsigned char *ap = (unsigned char*)nic->node_addr;
			for (i = 0; i < ETH_ALEN; i++)
				*ap++ = RTL_R8(MAC0 + i);
		}

		speed10 = RTL_R8(MediaStatus) & MSRSpeed10;
		fullduplex = RTL_R16(MII_BMCR) & BMCRDuplex;
		debug_print("ioaddr %lX, addr %02x:%02x:%02x:%02x:%02x:%02x  %sMbps %s-duplex\r\n", 
			     ioaddr,
			     nic->node_addr[0],
			     nic->node_addr[1],
			     nic->node_addr[2],
			     nic->node_addr[3],
			     nic->node_addr[4],
			     nic->node_addr[5],
			     speed10 ? "10" : "100",
			     fullduplex ? "full" : "half");


		rtl_reset(nic);

		IRQ_REQUEST(nic->irq,rtl_interrupt,&private_datas[found]);

		private_datas[found].nic = nic;

		nic->watchdog = rtl_reset;
		nic->transmit = rtl_transmit;
		nic->ioctl = rtl_ioctl;
		found ++;
		nic ++;
		p = p->next;

	}

	return found;
}




static void rtl_reset(struct nic* nic)
{
	int i;
	/* Mask the bit that says "this is an io addr" */
	ioaddr = nic->ioaddr;

	RTL_W8(ChipCmd,CmdReset);

	cur_rx = 0;
	cur_tx = 0;

	/* Give the chip 10ms to finish the reset. */
	load_timer2(10*TICKS_PER_MS);
	while ((RTL_R8(ChipCmd) & CmdReset) != 0 && timer2_running())
		/* wait */;

	for (i = 0; i < ETH_ALEN; i++)
		RTL_W8(MAC0 + i,nic->node_addr[i]);

	/* Must enable Tx/Rx before setting transfer thresholds! */
	RTL_W8(ChipCmd,CmdRxEnb | CmdTxEnb);
	RTL_W32(RxConfig,(RX_FIFO_THRESH<<13) | (RX_BUF_LEN_IDX<<11) | (RX_DMA_BURST<<8));		/* accept no frames yet!  */
	RTL_W32(TxConfig,(TX_DMA_BURST<<8)|0x03000000);

	/* The Linux driver changes Config1 here to use a different LED pattern
	 * for half duplex or full/autodetect duplex (for full/autodetect, the
	 * outputs are TX/RX, Link10/100, FULL, while for half duplex it uses
	 * TX/RX, Link100, Link10).  This is messy, because it doesn't match
	 * the inscription on the mounting bracket.  It should not be changed
	 * from the configuration EEPROM default, because the card manufacturer
	 * should have set that to match the card.  */


	RTL_W32(RxBuf,(unsigned long)rx_ring);

	/* Start the chip's Tx and Rx process. */
	RTL_W32(RxMissed,0);
	/* set_rx_mode */
	RTL_W8(RxConfig,AcceptBroadcast|AcceptMyPhys/*|AcceptAllPhys*/);
	/* If we add multicast support, the MAR0 register would have to be
	 * initialized to 0xffffffffffffffff (two 32 bit accesses).  Etherboot
	 * only needs broadcast (for ARP/RARP/BOOTP/DHCP) and unicast.  */
	RTL_W8(ChipCmd,CmdRxEnb | CmdTxEnb);

	/* Disable all known interrupts by setting the interrupt mask. */
	RTL_W16(IntrMask,0);

	RTL_W16(IntrMask,rtl8139_intr_mask);

	/* Clear watchdog timer */
	nic->timeout = 0;
}

static int rtl_transmit(struct nic *nic,  char *data,int len)
{
	/* Mask the bit that says "this is an io addr" */
	ioaddr = nic->ioaddr;
	

	debug_print("rtl_transmit() \r\n");

	/* Wait ... */	
	while(nic->timeout > 0) 
		;

	memcpy(tx_buffer ,data, len);

	/* Note: RTL8139 doesn't auto-pad, send minimum payload (another 4
	 * bytes are sent automatically for the FCS, totalling to 64 bytes). */
	while (len < ETH_ZLEN) {
		tx_buffer[len++] = '\0';
	}

	RTL_W32(TxAddr0 + cur_tx*4,(unsigned long)tx_buffer);
	RTL_W32(TxStatus0 + cur_tx*4,((TX_FIFO_THRESH<<11) & 0x003f0000) | len);


	/* Set a timer just in case we never hear from the board again . */
	nic->timeout = RTL_TIMEOUT;

	return len;
}

int
rtl_ioctl(struct nic *nic,u_long cmd,caddr_t data)
{
	/* Mask the bit that says "this is an io addr" */
	ioaddr = nic->ioaddr;
	return 0;
}




/* 
 * $Log: rtl8139.c,v $
 * Revision 1.10  2002/02/21 08:25:00  linfusheng
 * update
 *
 * Revision 1.9  2002/02/07 10:25:35  linfusheng
 * update
 *
 * Revision 1.8  2002/02/07 01:08:53  linfusheng
 * update
 *
 * Revision 1.7  2002/02/06 07:59:04  linfusheng
 * update
 *
 * Revision 1.6  2002/02/06 07:42:18  linfusheng
 * update
 *
 * Revision 1.5  2002/02/06 07:28:51  linfusheng
 * update
 *
 * Revision 1.4  2002/02/06 02:16:25  linfusheng
 * update
 *
 * Revision 1.3  2002/01/28 01:13:52  linfusheng
 * update
 *
 * Revision 1.2  2002/01/28 00:46:25  linfusheng
 * update
 *
 */


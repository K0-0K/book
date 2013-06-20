
/* 
 * $Id: cards.h,v 1.3 2002/02/07 10:25:41 linfusheng Exp $
 */

#ifndef	CARDS_H
#define CARDS_H


/* OK, this is how the PCI support hack works:  if pci.h is included before
 * this file is included, assume that the driver supports PCI.  This means that
 * this file is usually included last.  */

#ifdef	PCI_H
#define PCI_ARG(x) ,x
#else
#define PCI_ARG(x)
#endif

#ifdef	INCLUDE_WD
extern int  wd_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_3C503
extern int  t503_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_VIA_RHINE
extern int  rhine_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_NE
extern int  ne_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_NS8390
extern int  nepci_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_3C509
extern int  t509_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_3C529
extern int  t529_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_3C595
extern int  t595_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_3C90X
extern int  a3c90x_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_EEPRO
extern int  eepro_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_EEPRO100
extern int  eepro100_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_EPIC100
extern int  epic100_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_OTULIP
extern int  otulip_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_TULIP
extern int  tulip_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_DAVICOM
extern int  davicom_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_CS89X0
extern int  cs89x0_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_LANCE
extern int  lancepci_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_NE2100
extern int  ne2100_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_NI6510
extern int  ni6510_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_SK_G16
extern int  SK_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_3C507
extern int  t507_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_NI5010
extern int  ni5010_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_NI5210
extern int  ni5210_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_EXOS205
extern int  exos205_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_SMC9000
extern int  smc9000_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_TIARA
extern int  tiara_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_DEPCA
extern int  depca_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_RTL8139
extern int  rtl8139_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_W89C840
extern int  w89c840_probe(struct nic *);
	
#endif

#ifdef	INCLUDE_SIS900
extern int  sis900_probe(struct nic *);
        
#endif

#ifdef	INCLUDE_NATSEMI
extern int  natsemi_probe(struct nic *);
        
#endif

#endif	/* CARDS_H */
/* $Log: cards.h,v $
/* Revision 1.3  2002/02/07 10:25:41  linfusheng
/* update
/*
/* Revision 1.2  2002/01/28 00:46:25  linfusheng
/* update
/*
 */

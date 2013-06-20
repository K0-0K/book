/* 
 * $Id: pci.c,v 1.5 2002/02/07 01:08:53 linfusheng Exp $
 */

#include "includes.h"
#include "pci.h"
#include "pci_ids.h"

static struct pci_device pci_device_list[MAX_PCI_DEVICE_LIST];
int pci_device_num = 0;

/*#define	DEBUG	1*/

#define  PCIBIOS_SUCCESSFUL                0x00

/*
 * Functions for accessing PCI configuration space with type 1 accesses
 */

#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))

int pcibios_read_config_byte(unsigned long  bus, unsigned long  device_fn,
			       unsigned long  where, unsigned char *value)
{
    UCOS_OUTD(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    *value = UCOS_INB(0xCFC + (where&3));
    return PCIBIOS_SUCCESSFUL;
}

int pcibios_read_config_word (unsigned long  bus,
    unsigned long  device_fn, unsigned long  where, unsigned short *value)
{
    UCOS_OUTD(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    *value = UCOS_INW(0xCFC + (where&2));
    return PCIBIOS_SUCCESSFUL;
}

int pcibios_read_config_dword (unsigned long  bus, unsigned long  device_fn,
				 unsigned long  where, unsigned long  *value)
{
    UCOS_OUTD(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    *value = UCOS_IND(0xCFC);
    return PCIBIOS_SUCCESSFUL;
}

int pcibios_write_config_byte (unsigned long  bus, unsigned long  device_fn,
				 unsigned long  where, unsigned char value)
{
    UCOS_OUTD(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    UCOS_OUTB(value, 0xCFC + (where&3));
    return PCIBIOS_SUCCESSFUL;
}

int pcibios_write_config_word (unsigned long  bus, unsigned long  device_fn,
				 unsigned long  where, unsigned short value)
{
    UCOS_OUTD(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    UCOS_OUTW(value, 0xCFC + (where&2));
    return PCIBIOS_SUCCESSFUL;
}

int pcibios_write_config_dword (unsigned long  bus, unsigned long  device_fn,
				 unsigned long  where, unsigned long value)
{
    UCOS_OUTD(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    UCOS_OUTD(value, 0xCFC);
    return PCIBIOS_SUCCESSFUL;
}

#undef CONFIG_CMD


void read_PCI_config(struct pci_config_data *cfg,
		     unsigned long bus,
		     unsigned long device_fn)
{
	unsigned long i;
	DWORD *l = (DWORD *)cfg;
	for (i = 0 ; i < sizeof(struct pci_config_data)/sizeof(DWORD) ; i++) {
		pcibios_read_config_dword(bus,device_fn,i*sizeof(DWORD),&l[i]);
    }
}


void init_pci_bus(void)
{
	unsigned long devfn,  bus;
	unsigned char hdr_type = 0;
	unsigned short vendor, device;
	unsigned short reg;
	unsigned long l;
	unsigned long ioaddr;

	/* Scan all PCI buses, until we find our card.
	 * We could be smart only scan the required busses but that
	 * is error prone, and tricky.
	 * By scanning all possible pci busses in order we should find
	 * our card eventually.
	 */
	for (bus = 0; bus < 0xff; bus ++) {
		for (devfn = 0; devfn < 0xff; devfn ++) {
                        struct pci_device *pcidevice_ptr = &pci_device_list[pci_device_num];

			if (PCI_FUNC (devfn) == 0)
				pcibios_read_config_byte(bus, devfn, PCI_HEADER_TYPE, &hdr_type);
			else if (!(hdr_type & 0x80))	/* not a multi-function device */
				continue;
			pcibios_read_config_dword(bus, devfn, PCI_VENDOR_ID, &l);
			/* some broken boards return 0 if a slot is empty: */
			if (l == 0xffffffff || l == 0x00000000) {
				hdr_type = 0;
				continue;
			}

			vendor = (unsigned short)(l & 0xffff);
			device = (unsigned short) ((l >> 16) & 0xffff);



			pcidevice_ptr->vendor = vendor;
			pcidevice_ptr->device = device;
			pcidevice_ptr->devfn = devfn;
			pcidevice_ptr->bus = bus;

			// Get 256 byte config space
			read_PCI_config(&pcidevice_ptr->U.config_data,bus,devfn);

			// Lookup ioaddr from five baseaddr
			for (reg = PCI_BASE_ADDRESS_0; reg <= PCI_BASE_ADDRESS_5; reg += 4) {
				pcibios_read_config_dword(bus, devfn, reg, &ioaddr);

				if ((ioaddr & PCI_BASE_ADDRESS_IO_MASK) == 0 || (ioaddr & PCI_BASE_ADDRESS_SPACE_IO) == 0)
					continue;
				/* Strip the I/O address out of the returned value */
				ioaddr &= PCI_BASE_ADDRESS_IO_MASK;
				break;
			 }
			 pcidevice_ptr->ioaddr = (unsigned short)(ioaddr & 0xffff);
			 pcidevice_ptr->membase =  pcidevice_ptr->U.config_data.U.nonbridge.base_address1 ;
			 pcidevice_ptr->irq = pcidevice_ptr->U.config_data.U.nonbridge.interrupt_line;
			 pci_device_num ++;
		}
	}
}


void disp_pci_config(struct pci_config_data *pci_config)
{
	debug_print("command register: %04x \r\n",pci_config->command_reg);
	debug_print("status register: %04x \r\n",pci_config->status_reg);
	debug_print("interrupt line : %d \r\n",pci_config->U.nonbridge.interrupt_line);
	debug_print("subclass: %02x(%d) ",pci_config->subclass,pci_config->subclass);
	debug_print("classcode:%02x(%d) ",pci_config->classcode,pci_config->classcode);
	debug_print("\r\n");
	debug_print("baseaddr(0-5): ");
	debug_print("%08lx ",pci_config->U.nonbridge.base_address0);
	debug_print("%08lx ",pci_config->U.nonbridge.base_address1);
	debug_print("%08lx ",pci_config->U.nonbridge.base_address2);

	debug_print("%08lx ",pci_config->U.nonbridge.base_address3);
	debug_print("%08lx ",pci_config->U.nonbridge.base_address4);
	debug_print("%08lx ",pci_config->U.nonbridge.base_address5);
	debug_print("\r\n");

	return;
}
void dump_pci_device(void)
{
	int i;
	for( i = 0 ; i < pci_device_num ; i ++) {
		unsigned short pci_command = 0,new_command = 0;
		debug_print("device[%d]: %s \r\n",i,pci_device_list[i].name);
		debug_print("ioaddr = %04x memory address = %08lx \r\n",
						pci_device_list[i].ioaddr,
						pci_device_list[i].membase);

//		debug_print_buf("pci config data",pci_device_list[i].U.raw,256);
		disp_pci_config(&pci_device_list[i].U.config_data);

	}
}

struct pci_device *
lookup_pci_device(unsigned short vendor,unsigned short device)
{
	int i;
	struct pci_device *prev_device = NULL;
	struct pci_device *first_device = NULL;
	if(vendor == 0 || device == 0 )
			return NULL;
	for( i = 0 ; i < pci_device_num ; i ++) {
		if(pci_device_list[i].vendor == vendor &&
			pci_device_list[i].device == device) {
			if(first_device == NULL)
				first_device = &pci_device_list[i];
			if(prev_device)
				prev_device->next = &pci_device_list[i];
			prev_device = &pci_device_list[i];
		 }
	}
	return first_device;
}
/* 
 * $Log: pci.c,v $
 * Revision 1.5  2002/02/07 01:08:53  linfusheng
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
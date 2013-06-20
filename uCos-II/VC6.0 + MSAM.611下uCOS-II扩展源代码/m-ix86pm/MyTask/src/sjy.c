/*
 * $Id: sjy.c,v 1.3 2002/02/07 10:25:35 linfusheng Exp $
 */
#include "includes.h"
#include "pci.h"


//#define USE_IO_OPS
#ifdef USE_IO_OPS

#define SJY_R8(reg)		UCOS_INB (((unsigned long)ioaddr) + (reg))
#define SJY_R16(reg)		UCOS_INW (((unsigned long)ioaddr) + (reg))
#define SJY_R32(reg)		((unsigned long) UCOS_IND (((unsigned long)ioaddr) + (reg)))
#define SJY_W8(reg, val8)	UCOS_OUTB ((val8), ((unsigned long)ioaddr) + (reg))
#define SJY_W16(reg, val16)	UCOS_OUTW ((val16), ((unsigned long)ioaddr) + (reg))
#define SJY_W32(reg, val32)	UCOS_OUTD ((val32), ((unsigned long)ioaddr) + (reg))

#else


/* write MMIO register */
#define SJY_W8(reg, val8)	writeb ((val8), ioaddr + (reg))
#define SJY_W16(reg, val16)	writew ((val16), ioaddr + (reg))
#define SJY_W32(reg, val32)	writel ((val32), ioaddr + (reg))


/* read MMIO register */
#define SJY_R8(reg)		readb (ioaddr + (reg))
#define SJY_R16(reg)		readw (ioaddr + (reg))
#define SJY_R32(reg)		((unsigned long) readl (ioaddr + (reg)))

#endif /* USE_IO_OPS */

#define MIN(x,y)	(x > y ? y:x)
#define MAX(x,y)	(x > y ? x:y)

#define FIRST_BUF_SIZE	1024


struct sjy_board{
	unsigned long base_addr0;
	unsigned long base_addr1;
	unsigned long base_addr4;

	unsigned long data_addr;

	int irq;
	struct pci_device *pdev;
};

#define SJY_NBOARD 	8

struct sjy_board sjy_board[SJY_NBOARD];
int sjy_found = 0;
unsigned long ioaddr;

void sjy22_init(void)
{
	int i = 0;
	struct pci_device *pci_ptr = NULL;
	pci_ptr = lookup_pci_device(0x1234,0x5678);
	if(pci_ptr) {
			sjy_board[i].irq = pci_ptr->irq;

			sjy_board[i].base_addr0 = pci_ptr->U.config_data.U.nonbridge.base_address0 & ~3;
			sjy_board[i].base_addr1 = pci_ptr->U.config_data.U.nonbridge.base_address1 & ~3;
			sjy_board[i].base_addr4 = pci_ptr->U.config_data.U.nonbridge.base_address4 & ~3;


#ifdef USE_IO_OPS
                        ioaddr = (pci_ptr->U.config_data.U.nonbridge.base_address2 & ~3);
                        sjy_board[i].data_addr = (long) ioaddr;

#else
			ioaddr = pci_ptr->U.config_data.U.nonbridge.base_address3;
			sjy_board[i].data_addr = (long) ioaddr;
#endif /* USE_IO_OPS */


		return;
	}
}

WORD 
write_to_zero(WORD code,WORD inlen, WORD *in_buf,int is_first,struct sjy_board *board)
{
	
	WORD i = 0;


        if(inlen > FIRST_BUF_SIZE)
                return -1;

	/* Declare memory address */
	if(is_first) {
		UCOS_OUTW(0x01,board->base_addr1);	
	}
	else {
		UCOS_OUTW(FIRST_BUF_SIZE * 2 + 1,board->base_addr1);
	}


	/* Write data */
	ioaddr = board->data_addr;	

	SJY_W16(0,code);	
	SJY_W16(0,inlen);
	for( i = 0 ; i < inlen ; i++) {
		SJY_W16(0,in_buf[i]);				
	}



	/* Tell f206 ... */
	if(is_first) {
		UCOS_OUTW(0x03,board->base_addr4);
	}
	else {
		UCOS_OUTW(0x04,board->base_addr4);
	}

	return 0;
}

WORD 
read_from_zero(WORD *code ,WORD *outlen, WORD *out_buf,WORD buf_size,int is_first,struct sjy_board *board)
{
        WORD i,count ;



	/* Check vaild */
	if(is_first) {
		while((UCOS_INW(board->base_addr4) & 0x01) != 0x01);
		UCOS_OUTW(0x01,board->base_addr4);


		UCOS_OUTW(0x01,board->base_addr1);	
	}
	else {
		while((UCOS_INW(board->base_addr4) & 0x0100) != 0x0100) ;
		UCOS_OUTW(0x02,board->base_addr4);


		UCOS_OUTW(FIRST_BUF_SIZE * 2 + 1,board->base_addr1);
	}
	


	ioaddr = board->data_addr;
	*code = SJY_R16(0);
	count = SJY_R16(0);
	if(count > buf_size) {
		return -1;
	}

        for( i = 0 ; i < count ; i++) {
		out_buf[i] = SJY_R16(0);
        }

	*outlen = count;
	return 0;
}

WORD 
comm_to_card(WORD code,
	     WORD *in_buf,
 	     WORD inlen,
	     WORD *out_buf,
	     WORD *outlen,
	     WORD out_bufsize)	
{
	WORD count,left,size;
	int  zero , zero_flag[2] = {0,0},xor_flag;
	WORD error = 0;
	

	WORD *out_ptr = out_buf;
	WORD *in_ptr = in_buf;
	left = inlen;
	size = out_bufsize;

	if(code == 0x63)
		xor_flag = 1;
	else
		xor_flag = 0;

	zero = 0;


	while(left > 0 || (zero_flag[0] || zero_flag[1])){
		if(zero_flag[zero]) {
			//Maybe there is data to be read 
			if(read_from_zero(&error ,&count, out_ptr,size,zero^1,&sjy_board[0]))
				return -1;//Input buffer len false	

			if(error) // F206 return error code 	
				return error;
			zero_flag[zero] = 0;

			out_ptr += count;
			size -= count;
			
		}
	
		if(left) {
			
			count = MIN(left,FIRST_BUF_SIZE);
			write_to_zero(code,count,in_ptr,zero^1,&sjy_board[0]);
			zero_flag[zero] = 1;		

			in_ptr += count;
			left -= count;
		}
		zero ^= xor_flag;
	
	}

	*outlen = out_bufsize - size;
	return error;
		
}

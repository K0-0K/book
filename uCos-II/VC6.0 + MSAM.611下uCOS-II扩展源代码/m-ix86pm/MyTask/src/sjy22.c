/*
 * $Id: sjy22.c,v 1.3 2002/02/21 08:25:00 linfusheng Exp $
 */ 
#include "sjy22.h"

extern unsigned short comm_to_card(unsigned short code, unsigned short *in_buf, unsigned short inlen, unsigned short *out_buf,unsigned short *outlen, unsigned short out_bufsize);


int 
InitWithKey(unsigned short EncryptCmd, unsigned short *Key,unsigned short keyLen)
{
	unsigned short error = 0;
	unsigned short tmp[64];
	unsigned short outlen = 0;
	memset( tmp,0,64 << 1 );

	tmp[0]=EncryptCmd;
	memcpy(&tmp[1],Key,keyLen << 1);
	error = comm_to_card(cmdENC_INITKEY,tmp,keyLen + 1,tmp,&outlen,64);
	return error;
}


int 
Encrypt(unsigned short *in,unsigned short inlen,unsigned short *out)
{
	unsigned short i,n,left,offset = 0;
	unsigned short error = 0;
	unsigned short outlen = 0;
	n = inlen / F206_RAM_SIZE;
	for(i=0;i<n;i++){

		if((error = comm_to_card(cmdBCA_UPD,&in[offset],F206_RAM_SIZE,&out[offset],&outlen,F206_RAM_SIZE)))
			return error;
		offset += F206_RAM_SIZE;	
	}
	left = inlen % F206_RAM_SIZE;
	error =  comm_to_card(cmdBCA_UPD,&in[offset],left,&out[offset],&outlen,left);
	return error;
}

/* We sugguest that, the inputlen is align 8 , and key is 128 bit length */
int  
SJY22_encrypt(unsigned char *input,int inputlen,unsigned char *output,unsigned char *keybyte,int keylen,unsigned char *iv,int ivlen)
{
	InitWithKey(BCAIIE_CMD,(unsigned short *)keybyte,(unsigned short )(keylen >> 1));
	Encrypt((unsigned short *)input,(unsigned short )(inputlen >> 1), (unsigned short *)output);
	return 0;
}

int 
SJY22_decrypt(unsigned char *input,int inputlen,unsigned char *output,unsigned char *keybyte,int keylen,unsigned char *iv,int ivlen)
{
        InitWithKey(BCAIID_CMD,(unsigned short *)keybyte,(unsigned short)(keylen >> 1));
        Encrypt((unsigned short *)input,(unsigned short)(inputlen >> 1), (unsigned short *)output);
	return 0;
}


void 
SJY22_hash_Init (void *ctx)
{
	return;
}


void 
SJY22_hash_Update(void *ctx, unsigned char *input, unsigned int inputlen)
{
	return;
}
void 
SJY22_hash_Final(unsigned char output[16], void *ctx)
{
	return;
}


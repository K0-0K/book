/* $Id: sjy22.h,v 1.1 2002/02/06 02:17:11 linfusheng Exp $
 */
#ifndef SJY22_H
#define SJY22_H

#define	F206_RAM_SIZE		4064	// F206 BCA分组长度

#define	cmdLCD_RESET		0xe0   	//
#define	cmdLCD_HORIZONTAL	0xe1   	//
#define	cmdLCD_VERTICAL		0xe2   	//
#define	cmdLCD_UPDATA		0xe3   	//
#define	cmdLCD_CPMEMORY		0xe4   	//

/* function code */
#define	cmdINIT_CARD		0x00   	//保密卡初始化函数dspResetCard()
#define cmdGET_CARD		0x01   	//获得保密卡的一般性信息(卡序号)
#define	cmdGET_LOG	       	0x02   	//获得保密卡的审计日志信息
#define	cmdGET_LOGNUM     	0x03   	//获得保密卡的审计日志信息
#define	cmdCLR_LOG	       	0x04   	//清除保密卡的审计日志信息
#define	cmdSET_TIME		0x05   	//设置保密卡内部时钟
#define	cmdGET_TIME		0x06   	//获得保密卡内部时钟

#define	cmdHAV_ICHEAD		0x2a   	//是否存在IC卡
#define	cmdSHK_IC		0x2b   	//建立IC卡与读写器的握手通道
#define	cmdWRT_ICHEAD		0x20   	//制卡时写入IC卡的头部信息
#define	cmdWRT_ICKK		0x21   	//写KK到密钥管理卡	dspWriteKKToKeyCard()
#define	cmdWRT_ICMKT		0x22   	//写MKT到密钥管理卡	dspWriteMKTToKeyCard()
#define	cmdWRT_ICUSR		0x23   	//写用户信息到操作员卡
#define	cmdCLR_IC	        0x24	//清除IC卡中所有内容	dspClearICCard()
#define	cmdGET_ICHEAD		0x25	//获得IC卡头部信息
#define	cmdGET_USRNUM	   	0x26	//取保密卡内有效用户数目
#define	cmdGET_USRPIN		0x27 	//取保密卡内某个用户信息
#define	cmdAUT_USRPIN		0x28 	//验证身份卡的身份是否合格
#define	cmdCLR_USRPIN		0x29 	//清除保密卡内某个用户信息

#define	cmdINIT_KP1	      	0x40  	//初始化主密钥	dspInitKP1()
#define	cmdWRT_KK	      	0x41  	//写入KK算法	dspWriteKKToCard
#define	cmdWRT_MKT		0x42  	//写入MKT算法	dspWriteMKTToCard()
#define	cmdCHG_KK	      	0x43  	//通过SMC自动更新KK
#define	cmdREC_MKT		0x44  	//从SMC接收新的端端密钥矩阵
#define	cmdCHG_MKT		0x45  	//启用新的端端密钥矩阵dspEnabledNewMKT()
#define	cmdSTOP_CARD		0x46  	//通过SMC自动销毁保密卡dspStopSecurityCard()

#define	cmdENC_INIT	      	0x60 	//加解密算法初始化(密钥在卡内)
#define	cmdENC_INITSTR		0x61 	//流密码算法加密初始化(密钥在卡内)
#define	cmdDEC_INITSTR		0x67 	//流密码算法解密初始化(密钥在卡内)
#define	cmdENC_INITKEY		0x62 	//加解密算法初始化（密钥由应用程序送入）
#define	cmdENC_INITKP		0x69 	//加解密算法初始化（使用主密钥）
#define	cmdENC_INITKK		0x6a 	//加解密算法初始化（使用密钥加密密钥）

#define	cmdBCA_UPD		0x63 	//分组128加解密算法 dspBCAUpdate()
#define	cmdHASH_UPD		0x64 	//Hash认证码计算函数dspHashUpdate()
#define	cmdSTR_UPD	      	0x66 	//流密码解密算法 dspStreamUpdate()
#define	cmdBCA_FIN		0x68 	//分组128加解密结束算法 dspBCAFinal()
#define	cmdHASH_FIN		0x65 	//获得最终认证码 dspHashFinal()
#define	cmdSTR_FIN	      	0x69 	//流密码解结束算法 dspStreamFinal()

#define	cmdGEN_RND		0x80 	//生成任意长的随机数bscGenerateRandom()
#define	cmdGEN_RNDBCA		0x81 	//生成任意长的高质量的随机数

#define		BCAIIE_CMD 	0x01	// 128位BCAII加密
#define		BCAIID_CMD 	0x08	// 128位BCAII解密
#define		HASH_CMD   	0x45	// 128位HASH认证
#define		STREAM_CMD 	0x00	// 128流密码算法

#define		BCA64E_CMD 	0x07	// 64位BCAII加密
#define		BCA64D_CMD 	0x03	// 64位BCAII解密
#define		HASH64_CMD  	0x47	// 64位HASH认证
#define		STREAM64_CMD 	0x00	// 64流密码算法

#define		BCAIIIE_CMD 	0xa0	// 128位BCAIII加密
#define		BCAIIID_CMD 	0xa1	// 128位BCAIII解密

#define		KSLENGTH 	16	/*会话密钥长度 */
#define		MACLENGTH 	16	/*会话密钥长度 */

#define		CARD_READY	6
#define		CARD_BAD	4
#define		CARD_BUSY	8

#define		CARD_NUM	2 	/* card number is 2 */


/* We sugguest that, the inputlen is align 8 , and key is 128 bit length */
extern int  SJY22_encrypt(unsigned char *input,int inputlen,unsigned char *output,unsigned char *keybyte,int keylen,unsigned char *iv,int ivlen);

extern int SJY22_decrypt(unsigned char *input,int inputlen,unsigned char *output,unsigned char *keybyte,int keylen,unsigned char *iv,int ivlen);



extern void SJY22_hash_Init (void *ctx);
extern void SJY22_hash_Update(void *ctx, unsigned char *input, unsigned int inputlen);
extern void SJY22_hash_Final(unsigned char output[16], void *ctx);

#endif

////////////////I2C数据读写模块////////////////////////////////////
//sbit    SCL=P1^3;                                   //I2C总线时钟口
//sbit    SDA=P1^4;                                   //I2C总线数据口
#define Write24c02	0xa0                                //24c02写地址
#define Read24c02	0xa1                                //24c02读地址
#define Write8563	0xa2                                //24c02写地址
#define Read8563	0xa3                                //24c02读地址
#define Wri24c02	0xa6                                //备份24c02写地址
#define Rea24c02	0xa7                               //备份24c02读地址

void DelayMs(unsigned int number)
{
	unsigned char temp;
	for(;number!=0;number--) {
		for(temp=112;temp!=0;temp--) {
		}
	}
}
/***************************************************************************/

void Start()
{
	SDA=1;
	SCL=1;
	_nop_();
	SDA=0;
	_nop_();
	SCL=0;
}

/***************************************************************************/
void Stop()
{
	SCL=0;
	SDA=0;
	SCL=1;
	_nop_();
	SDA=1;
	_nop_ ();
}

/***************************************************************************/
void Ack()
{
	SDA=0;
	SCL=1;
	_nop_ ();
	SCL=0;
	SDA=1;
}

/***************************************************************************/
void NoAck()
{
	SDA=1;
	SCL=1;
	_nop_ ();
	SCL=0;
}

/***************************************************************************/
unsigned char TestAck()
{
	unsigned char ErrorBit;
	SDA=1;
	SCL=1;
	_nop_ ();
	ErrorBit=SDA;
	SCL=0;
	return(ErrorBit);
}

/***************************************************************************/
void Write8Bit(unsigned char input)
{
	unsigned char temp;
	SCL=0;
	for(temp=8;temp!=0;temp--)
	{
		SDA=(bit)(input&0x80);
		SCL=1;
		_nop_();
		SCL=0;
		input=input<<1;
	}
}

/***************************************************************************/
void Writei2c(unsigned char *Wdata,unsigned char RomAddress,unsigned char number)
{
	Start();
	Write8Bit(Write24c02);
	TestAck();
	Write8Bit(RomAddress);
	TestAck();
	for(;number!=0;number--) {
		Write8Bit(*Wdata);
		TestAck();
		Wdata++;
	}
	Stop();
	DelayMs(5);
}
/***************************************************************************/
unsigned char Read8Bit()
{
	unsigned char temp,rbyte=0;
	SCL=0;
	for(temp=8;temp!=0;temp--)
	{
		SCL=1;
		rbyte=rbyte<<1;
		rbyte=rbyte|((unsigned char)(SDA));
		SCL=0;
	}
	return(rbyte);
}

/***************************************************************************/
void Readi2c(unsigned char *RamAddress,unsigned char RomAddress,unsigned char bytes)
{
	Start();
	Write8Bit(Write24c02);
	TestAck();
	Write8Bit(RomAddress);
	TestAck();
	Start();
	Write8Bit(Read24c02);
	TestAck();
	while(bytes!=1)
	{
		*RamAddress=Read8Bit();
		Ack();
		RamAddress++;
		bytes--;
	}
	*RamAddress=Read8Bit();
	NoAck();
	Stop();
}
////////////////I2C数据读写模块完////////////////////////////////////

void Writei2ctime(unsigned char *Wdata,unsigned char RomAddress,unsigned char number)
{
	Start();
	Write8Bit(Write8563);
	_nop_();
	TestAck();
	_nop_();
	Write8Bit(RomAddress);
	_nop_();
	TestAck();
	_nop_();
	for(;number!=0;number--) {
		Write8Bit(*Wdata);
		TestAck();
		Wdata++;
	}
	Stop();
	DelayMs(2);
}
void Readi2ctime(unsigned char *RamAddress,unsigned char RomAddress,unsigned char bytes)
{
	Start();
	Write8Bit(Write8563);
	TestAck();
	Write8Bit(RomAddress);
	TestAck();
	Start();
	Write8Bit(Read8563);
	TestAck();
	while(bytes!=1)
	{
		*RamAddress=Read8Bit();
		Ack();
		RamAddress++;
		bytes--;
	}
	*RamAddress=Read8Bit();
	NoAck();
	Stop();
}
////////////////I2C数据读写模块完////////////////////////////////////
//读入年月日
void readtimer(unsigned char* distime,unsigned char bday)
{
	//当前时间读入到distime中，依次为年-月-日 时:分:秒
	unsigned char tempc[5];
	if(bday)
	{
		Readi2ctime(tempc,0x05,0x04);
		//日
		tempc[0]=tempc[0]&0x3F;
		distime[8]=0;
		distime[7]=(tempc[0]&0x0F)+48;
		distime[6]=tempc[0]/16+48;
		//月
		tempc[2]=tempc[2]&0x1F;
		distime[5]='.';
		distime[4]=(tempc[2]&0x0F)+48;
		distime[3]=tempc[2]/16+48;
		//年
		distime[2]='.';
		distime[1]=(tempc[3]&0x0F)+48;
		distime[0]=tempc[3]/16+48;	
	}
	else
	{
		Readi2ctime(tempc,0x02,0x05);
		//秒
		tempc[0]=tempc[0]&0x7F;
		distime[6]=tempc[0]/16+48;
		distime[7]=(tempc[0]&0x0F)+48;
		distime[8]=0;
		//分
		tempc[1]=tempc[1]&0x7F;
		distime[3]=tempc[1]/16+48;
		distime[4]=(tempc[1]&0x0F)+48;
		distime[5]=':';
		//时
		tempc[2]=tempc[2]&0x3F;
		distime[0]=tempc[2]/16+48;
		distime[1]=(tempc[2]&0x0F)+48;
		distime[2]=':';
	}
}
void settimer(unsigned char *str,unsigned char dd)
{
	//依次为20年月日,星期时分秒各占两字节分两次输入写入到8563中
	unsigned char tempc[4];
	if(dd)
	{
		tempc[3]=(str[0]-48)*16+str[1]-48;//年
		tempc[2]=(str[3]-48)*16+str[4]-48;//月
		tempc[1]=0;
		tempc[0]=(str[6]-48)*16+str[7]-48;//日
		Writei2ctime(tempc,5,4);
	}
	else
	{
		tempc[2]=(str[0]-48)*16+str[1]-48;//时
		tempc[1]=(str[3]-48)*16+str[4]-48;//分
		tempc[0]=(str[6]-48)*16+str[7]-48;//秒
		Writei2ctime(tempc,2,3);
	}
}


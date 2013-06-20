#include <includes.h>
#include <ctype.h>
#include <aultdef.h>
#include <i2c.h>
#include <hhh16.h>
#include <chatfont.h>
#include <sed1330.h>
#include <8253.h>
#define KCREETRANT		reentrant

void sys_init(void *lcnpdata) reentrant;
void mysecondtask(void *lcndata) reentrant;
void main1test(void *lcnpdata) KCREETRANT;
void gettesttype(void *lcnpdata) KCREETRANT;
void lcngetkey(void *lcnpdata) reentrant;
void getcurvault() reentrant;
void getstr8255() reentrant;

OS_STK sys_init_Stk[MaxStkSize];
OS_STK mysecondtask_Stk[MaxStkSize];
OS_STK lcngetkey_Stk[MaxStkSize];
OS_STK main1test_Stk[MaxStkSize];
OS_STK gettesttype_Stk[MaxStkSize];


void init_serial() reentrant
{	
	PCON &= 0x7F;				
	TH1 = 0xFD;	
	TMOD |= 0X20;                      //定时器1方式2
	SCON = 0X50;                      //允许接收
	TR1 = 1;                          //启动定时器1
	ES = 1;                           //串行口中断允许
}

//串口中断处理
void serial() reentrant
{   
	if(RI)  
    {  
		RI = 0;
		rs485in = SBUF;
	}
	else
    { 
		TI = 0; 
		if(bout485 > 0)
		{
			SBUF = rs485out; 
			bout485 = 0;
		}
		else
		{
			RS485 = 1;
		}
	}
}

unsigned int advaut;					//AD574采样中间数值
void getad() reentrant
{
	unsigned char i,tempcl;
	unsigned int tempch;
	advaut=0;
	for(i=0;i<16;i++)
	{
		ADCOM=0;        //启动AD转换
		adbusy=1;
		tempcl=200;
		while(tempcl&&adbusy)
		{
			tempcl++;
		}
		tempch=(unsigned int)ADHI;
		tempcl=ADLO;
		advaut+=(tempch*16+(unsigned int)(tempcl/16));
	}
	advaut/=16;	
}
unsigned char gettimestr(unsigned int x,unsigned char y,unsigned char nu,unsigned char bdate) reentrant
{
	unsigned char tem;
	keystaut=0;
	rectangle(SAD2,x-1,y,x+nu*8,y+16,1);
	tem=0;
	OSSemPend(KeySem,0,&lcnerr);
	if(key==ESC||key==ENTER)
	{
		rectangle(SAD2,x-1,y,x+nu*8,y+16,0);
		return(0);
	}
	for(tem=0;tem<nu;tem++)
		disstr[tem]=' ';
	if(bdate)
	{
		disstr[2]='.';
		disstr[5]='.';
	}
	else
	{
		disstr[2]=':';
		disstr[5]=':';
	}
	disstr[tem]=0;
	tem=0;
	while(tem<nu)
	{
		if(isdigit(key))
		{
			disstr[tem]=key;
			myprintf(SAD1,TA_LEFT,x,y,disstr);
			tem++;
			if(tem==2||tem==5)
				tem++;
		}
		else
		{
			switch(key)
			{
			case ENTER:
				keystaut=0;
				rectangle(SAD2,x-1,y,x+nu*8,y+16,0);
				return(1);
			default:
				break;
			}
		}
		if(tem<nu)
			OSSemPend(KeySem,0,&lcnerr);
	}
	rectangle(SAD2,x-1,y,x+nu*8,y+16,0);
	keystaut=0;
	return(1);
}
unsigned char getstrnumber(unsigned int x,unsigned char y,unsigned char nu) reentrant
{
	unsigned char tem;
	keystaut=0;
	linex(SAD1,y+16,x-1,x+nu*8);
	tem=0;
	OSSemPend(KeySem,0,&lcnerr);
	if(key==ESC||key==ENTER)
	{
		rectangle(SAD1,x-1,y+15,x+nu*8,y+16,0);
		return(0);
	}
	for(tem=0;tem<nu;tem++)
		disstr[tem]=' ';
	disstr[tem]=0;
	tem=0;
	while(tem<nu)
	{
		if(isdigit(key))
		{
			disstr[tem]=key;
			myprintf(SAD1,TA_LEFT,x,y,disstr);
			tem++;
		}
		else
		{
			switch(key)
			{
			case ENTER:
				keystaut=0;
				rectangle(SAD1,x-1,y,x+nu*8,y+16,0);
				myprintf(SAD1,TA_LEFT,x,y,disstr);
				return(1);
			case ESC:
				if(tem>0)
					tem--;
				disstr[tem]=' ';
				myprintf(SAD1,TA_LEFT,x,y,disstr);
				disstr[tem]=0;
				break;
			default:
				break;
			}
		}
		if(tem<nu)
			OSSemPend(KeySem,0,&lcnerr);
	}
	keystaut=0;
	rectangle(SAD1,x-1,y,x+nu*8,y+16,0);
	myprintf(SAD1,TA_LEFT,x,y,disstr);
	return(1);
}

unsigned char getstr(unsigned int x,unsigned char y,unsigned char nu) reentrant
{
	unsigned char tem;
	keystaut=0;
	rectangle(SAD2,x-1,y,x+nu*8+8,y+16,1);
	myprintf(SAD1,TA_LEFT,x+nu*8,y,"v");
	tem=0;
	OSSemPend(KeySem,0,&lcnerr);
	if(key==ESC||key==ENTER)
	{
		myprintf(SAD1,TA_LEFT,x+nu*8,y," ");
		rectangle(SAD2,x-1,y,x+nu*8+8,y+16,0);
		return(0);
	}
	for(tem=0;tem<nu;tem++)
		disstr[tem]=' ';
	disstr[tem]=0;
	tem=0;
	myprintf(SAD1,TA_LEFT,x+nu*8,y,"v");
	while(tem<nu)
	{
		if(isprint(key))
		{
			disstr[tem]=key;
			myprintf(SAD1,TA_LEFT,x,y,disstr);
			tem++;
		}
		else
		{
			switch(key)
			{
			case ENTER:
				keystaut=0;
				rectangle(SAD2,x-1,y,x+nu*8+8,y+16,0);
				myprintf(SAD1,TA_LEFT,x+nu*8,y," ");
				return(1);
			case LEFT:
				keystaut=24;
				myprintf(SAD1,TA_LEFT,x+nu*8,y,"<");
				break;
			case UP:
				keystaut=48;
				myprintf(SAD1,TA_LEFT,x+nu*8,y,"^");
				break;
			case RIGHT:
				keystaut=72;
				myprintf(SAD1,TA_LEFT,x+nu*8,y,">");
				break;
			case DOWN:
				keystaut=0;
				myprintf(SAD1,TA_LEFT,x+nu*8,y,"v");
				break;
			case ESC:
				if(tem>0)
					tem--;
				disstr[tem]=' ';
				myprintf(SAD1,TA_LEFT,x,y,disstr);
				disstr[tem]=0;
				break;
			default:
				break;
			}
		}
		if(tem<nu)
			OSSemPend(KeySem,0,&lcnerr);
	}
	rectangle(SAD2,x-1,y,x+nu*8+8,y+16,0);
	myprintf(SAD1,TA_LEFT,x+nu*8,y," ");
	keystaut=0;
	return(1);
}

//输入系统时间及网络信息
void setsysini() reentrant
{
	unsigned char tem;

	clssed(SAD1);
	clssed(SAD2);
	linex(SAD1,60,70,250);
	linex(SAD1,80,70,250);
	linex(SAD1,122,70,250);

	liney(SAD1,70,60,122);
	liney(SAD1,250,60,122);
	
	myprintf(SAD1,TA_CENTER,160,62,"系统时钟设定");
	myprintf(SAD1,TA_LEFT,90,85,"当前日期:");
	myprintf(SAD1,TA_LEFT,90,104,"当前时间:");

	readtimer(disstr,1);
	myprintf(SAD1,TA_LEFT,165,85,disstr);
	readtimer(disstr,0);
	myprintf(SAD1,TA_LEFT,165,104,disstr);

	rectangle(SAD2,90,83,160,101,1);
	tem=0;
	while(1)
	{
		OSSemPend(KeySem,0,&lcnerr);
		if(key==INPUT)//输入键
		{
			switch(tem)
			{
			case 0:
				if(gettimestr(165,84+tem*19,8,1))
					settimer(disstr,1);
				break;
			case 1:
				if(gettimestr(165,84+tem*19,8,0))
					settimer(disstr,0);				
				break;
			default:
				break;
			}
		}
		else
		{
			if(key==UP||key==DOWN)//上下方向键
			{
				if(key==DOWN)
				{
					rectangle(SAD2,90,83+tem*19,160,101+tem*19,0);
					if(tem<1)
						tem++;
					else
						tem=0;
					rectangle(SAD2,90,83+tem*19,160,101+tem*19,1);
				}
				else
				{
					rectangle(SAD2,90,83+tem*19,160,101+tem*19,0);
					if(tem>0)
						tem--;
					else
						tem=1;
					rectangle(SAD2,90,83+tem*19,160,101+tem*19,1);
				}
			}
			else
			{
				if(key==ENTER)
				{
					return;
				}
				else
				{
					if(key==ESC)
						return;
				}
			}
		}
	}
}
void showlogo() reentrant
{
	unsigned char lcncount;
	xdata union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
    curadd.add=SAD3;
    LCDORDER=0X46;                       /*设置光标地址*/
    LCDDATA=curadd.addhalf.addlo;
    LCDDATA=curadd.addhalf.addhi;
	LCDORDER=0X4C;
	LCDORDER=0X42;
	curadd.add=0;
	while(curadd.add<sizeof(Bmp015))
	{
		if(Bmp015[curadd.add]==0)
		{
			curadd.add++;
			lcncount=Bmp015[curadd.add];
			while(lcncount)
			{
				LCDDATA=0;
				lcncount--;
			}
			curadd.add++;
		}
		else
		{
			LCDDATA=Bmp015[curadd.add];
			curadd.add++;
		}
	}
}
//系统初始化
void sys_init(void *lcnpdata) reentrant
{
	unsigned char tem,tem1;

	lcnpdata = lcnpdata;
	ADNUMBER1 = 1;
	ADNUMBER2 = 1;
	ADNUMBER3 = 1;
	//8279和SED1330初始化
	COM8279=0xd1;                   //总清除命令160us
    initlcd();
	showlogo();
	clssed(SAD1);
	clssed(SAD2);
	COM8279=0x08;COM8279=0x32;          //键盘,显示方式,时钟分频
	initcount();
	if(onpower!=0x55)
	{
		keytype=0;
		onpower=0x55;
	}
	

	tem = 0x55;
	for(tem1=0;tem1<10;tem1++)
	{
		Writei2c(&tem,tem1+160,1);
	}
	for(tem1=0;tem1<10;tem1++)
	{
		Readi2c(&tem,tem1+160,1);
		if(tem != 0x55)
			ok24c02 = 0;
	}
	//创建键盘信号量
	KeySem=OSSemCreate(0);

	tem=0x81;//1024
	Writei2ctime(&tem,0x0D,0x01);

#if OS_TASK_STAT_EN	
	OSStatInit();
#endif
	OSTaskCreate(mysecondtask, (void *)0, &mysecondtask_Stk[0],3);
	OSTaskCreate(lcngetkey, (void *)0, &lcngetkey_Stk[0],4);
	OSTaskCreate(gettesttype, (void *)0, &gettesttype_Stk[0],5);	
	init_serial();
	//消除警告
	OSVersion();
	OSTaskDelReq(OS_PRIO_SELF);
	//消除警告
	OSTaskDel(OS_PRIO_SELF);
}

void gettesttype(void *lcnpdata) KCREETRANT
{
	unsigned char lastselect = 0;
	unsigned char tem = 0;

	lcnpdata = lcnpdata;
	clssed(SAD1);
	clssed(SAD2);
	linex(SAD1,39,48,270);
	myprintf(SAD1,TA_CENTER,160,20,"通用板系统调试程序");
	myprintf(SAD1,TA_CENTER,160,90, "用18键通用键盘");
	myprintf(SAD1,TA_CENTER,160,120,"用20键通用键盘");	
	myprintf(SAD1,TA_CENTER,160,200,"唐山百川智能机器有限公司");	
	rectangle(SAD2,0,222,319,239,1);
	readtimer(disstr,1);
	myprintf(SAD1,TA_LEFT,186,223,disstr);	
	myprintf(SAD1,TA_LEFT,2,223,"按键选择键盘");
	lastselect = keytype;
	rectangle(SAD2,103,88+lastselect*30,217,108+lastselect*30,1);
	LCDORDER=0X59;
	LCDDATA=0X14;	
	while(1)
	{
		OSSemPend(KeySem,2,&lcnerr);
		if(lcnerr==OS_NO_ERR)
		{
			rectangle(SAD2,103,88+lastselect*30,217,108+lastselect*30,0);
			if(lastselect > 0)
			{
				keytype = 0;
				lastselect = 0;
			}
			else
			{
				keytype = 1;
				lastselect = 1;
			}
			rectangle(SAD2,103,88+lastselect*30,217,108+lastselect*30,1);
		}
		if(bSecond)
		{
			tem++;
			if(tem > 4)
			{
				OSTaskCreate(main1test, (void *)0, &main1test_Stk[0],6);
				OSTaskDel(OS_PRIO_SELF);
			}
			bSecond=0;					
			readtimer(disstr,0);
			myprintf(SAD1,TA_LEFT,254,223,disstr);
		}
	}
}

void lcngetkey(void *lcnpdata) reentrant
{
	unsigned char k;
	unsigned char newkey = 1;
	lcnpdata=lcnpdata;
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/10);
		if(COM8279 & 0x0f)
		{
			if(newkey)
			{
				if(COM8279 & 0x0f)
				{
					COM8279=0x50;   //读FIFO命令自动地址增量
					k=DAT8279;
					k=k&0x1f;               //取键盘数据低5位(32位键盘模式)
					if(keytype == 0)
					{
						key=keytab18[k+keystaut];
					}
					else
					{
						key=keytab20[k+keystaut];
					}
					keyinterrupt = 0;
					OSSemPost(KeySem);
					newkey = 0;
				}
			}
			else
			{
				COM8279=0xC2;
			}
		}
		else
		{
			newkey = 1;
		}
	}
}
//秒定时中断
void mysecondtask(void *lcndata) reentrant
{
	lcndata=lcndata;
	while(1)
	{
		bSecond=1;
		getconut();
		OSTimeDly(OS_TICKS_PER_SEC);	
	}
}

void showmain1() reentrant
{
	clssed(SAD1);
	clssed(SAD2);
	rectangle(SAD2,0,222,319,239,1);
	readtimer(disstr,1);
	myprintf(SAD1,TA_LEFT,186,223,disstr);
	myprintf(SAD1,TA_LEFT,2,223,"选功能.修改输出值");

	liney(SAD1,0,0,220);
	liney(SAD1,319,0,220);

	linex(SAD1,0,0,319);
	linex(SAD1,220,0,319);
	myprintf(SAD1,TA_LEFT,8,2,  "AD0:%4u:%6.3fV",adtemp[0],result[0]);	
	myprintf(SAD1,TA_LEFT,8,22, "AD1:%4u:%6.3fV",adtemp[1],result[1]);	
	myprintf(SAD1,TA_LEFT,8,42, "AD2:%4u:%6.3fV",adtemp[2],result[2]);	
	myprintf(SAD1,TA_LEFT,8,62, "AD3:%4u:%6.3fV",adtemp[3],result[3]);	
	myprintf(SAD1,TA_LEFT,8,82, "AD4:%4u:%6.3fV",adtemp[4],result[4]);	
	myprintf(SAD1,TA_LEFT,8,102,"AD5:%4u:%6.3fV",adtemp[5],result[5]);	
	myprintf(SAD1,TA_LEFT,8,122,"AD6:%4u:%6.3fV",adtemp[6],result[6]);	
	myprintf(SAD1,TA_LEFT,8,142,"AD7:%4u:%6.3fV",adtemp[7],result[7]);
	getstr8255();
	myprintf(SAD1,TA_LEFT,168,162,"OUT8255A:%s",do8255a);
	myprintf(SAD1,TA_LEFT,168,182,"OUT8255B:%s",do8255b);
	myprintf(SAD1,TA_LEFT,168,202,"OUT8255C:%s",do8255c);
	myprintf(SAD1,TA_LEFT,8,162,"IN8255A:%s",do8255a);
	myprintf(SAD1,TA_LEFT,8,182,"IN8255B:%s",do8255b);
	myprintf(SAD1,TA_LEFT,8,202,"IN8255C:%s",do8255c);
	myprintf(SAD1,TA_LEFT,168,2,  "82530:%5u",count8253[0][0]);	
	myprintf(SAD1,TA_LEFT,168,22, "82531:%5u",count8253[1][0]);	
	myprintf(SAD1,TA_LEFT,168,42, "82532:%5u",count8253[2][0]);	
	myprintf(SAD1,TA_LEFT,168,62, "T0   :%5u",count8253[3][0]);	
	if(ok24c02 == 0)
		myprintf(SAD1,TA_LEFT,168,82, "24C02:读写错误");
	else
		myprintf(SAD1,TA_LEFT,168,82, "24C02:读写正常");
	myprintf(SAD1,TA_LEFT,168,102,"485IN:%02XH:%c",(unsigned int)rs485in,rs485in);
	myprintf(SAD1,TA_LEFT,168,122,"485OU:");
	vda667 = da667;
	vda667 /=409.5;
	myprintf(SAD1,TA_LEFT,168,142, "DA667:%4u:%6.3fV",da667,vda667);	
	liney(SAD1,160,120,220);
	linex(SAD1,120,160,319);
}

void getstr8255() reentrant
{
	if(a1)
		do8255a[0] = '1';
	else
		do8255a[0] = '0';

	if(a2)
		do8255a[1] = '1';
	else
		do8255a[1] = '0';

	if(a3)
		do8255a[2] = '1';
	else
		do8255a[2] = '0';

	if(a4)
		do8255a[3] = '1';
	else
		do8255a[3] = '0';

	if(a5)
		do8255a[4] = '1';
	else
		do8255a[4] = '0';

	if(a6)
		do8255a[5] = '1';
	else
		do8255a[5] = '0';

	if(a7)
		do8255a[6] = '1';
	else
		do8255a[6] = '0';

	if(a8)
		do8255a[7] = '1';
	else
		do8255a[7] = '0';

	if(b1)
		do8255b[0] = '1';
	else
		do8255b[0] = '0';

	if(b2)
		do8255b[1] = '1';
	else
		do8255b[1] = '0';

	if(b3)
		do8255b[2] = '1';
	else
		do8255b[2] = '0';

	if(b4)
		do8255b[3] = '1';
	else
		do8255b[3] = '0';

	if(b5)
		do8255b[4] = '1';
	else
		do8255b[4] = '0';

	if(b6)
		do8255b[5] = '1';
	else
		do8255b[5] = '0';

	if(b7)
		do8255b[6] = '1';
	else
		do8255b[6] = '0';

	if(b8)
		do8255b[7] = '1';
	else
		do8255b[7] = '0';

	if(c1)
		do8255c[0] = '1';
	else
		do8255c[0] = '0';

	if(c2)
		do8255c[1] = '1';
	else
		do8255c[1] = '0';

	if(c3)
		do8255c[2] = '1';
	else
		do8255c[2] = '0';

	if(c4)
		do8255c[3] = '1';
	else
		do8255c[3] = '0';

	if(c5)
		do8255c[4] = '1';
	else
		do8255c[4] = '0';

	if(c6)
		do8255c[5] = '1';
	else
		do8255c[5] = '0';

	if(c7)
		do8255c[6] = '1';
	else
		do8255c[6] = '0';

	if(c8)
		do8255c[7] = '1';
	else
		do8255c[7] = '0';
	do8255a[8] = 0;
	do8255b[8] = 0;
	do8255c[8] = 0;
}


void showvaultall() reentrant
{
	switch(buttonselect)
	{
	case 0:
		myprintf(SAD1,TA_LEFT,40,2,  "%4u:%6.3fV",adtemp[0],result[0]);	
		break;
	case 1:
		myprintf(SAD1,TA_LEFT,40,22, "%4u:%6.3fV",adtemp[1],result[1]);	
		break;
	case 2:
		myprintf(SAD1,TA_LEFT,40,42, "%4u:%6.3fV",adtemp[2],result[2]);	
		break;
	case 3:
		myprintf(SAD1,TA_LEFT,40,62, "%4u:%6.3fV",adtemp[3],result[3]);	
		break;
	case 4:
		myprintf(SAD1,TA_LEFT,40,82, "%4u:%6.3fV",adtemp[4],result[4]);	
		break;
	case 5:
		myprintf(SAD1,TA_LEFT,40,102,"%4u:%6.3fV",adtemp[5],result[5]);	
		break;
	case 6:
		myprintf(SAD1,TA_LEFT,40,122,"%4u:%6.3fV",adtemp[6],result[6]);	
		break;
	case 7:
		myprintf(SAD1,TA_LEFT,40,142,"%4u:%6.3fV",adtemp[7],result[7]);
		break;
	case 16:
		myprintf(SAD1,TA_LEFT,216,102,"%02XH:%c",(unsigned int)rs485in,rs485in);
		break;
	case 18:
		myprintf(SAD1,TA_LEFT,216,142, "%4u:%6.3fV",da667,vda667);	
		break;
	case 19:
		getstr8255();
		myprintf(SAD1,TA_LEFT,240,162,"%8s",do8255a);
		break;
	case 20:
		getstr8255();
		myprintf(SAD1,TA_LEFT,240,182,"%8s",do8255b);
		break;
	case 21:
		getstr8255();
		myprintf(SAD1,TA_LEFT,240,202,"%8s",do8255c);
		break;
	case 8:
	case 9:
	case 10:
		EA = 0;
		COM8255=0x9B;
		tem8255a = A8255;
		tem8255b = B8255;
		tem8255c = C8255;		
		EA = 1;
		getstr8255();
		myprintf(SAD1,TA_LEFT,72,162,"%s",do8255a);
		myprintf(SAD1,TA_LEFT,72,182,"%s",do8255b);
		myprintf(SAD1,TA_LEFT,72,202,"%s",do8255c);
		break;
	default:
		break;
	}
}
void showselect(unsigned char select,unsigned char fill)
{
	if(fill == 0)
		rectangle(SAD2,6+(select/11)*160,2+(select%11)*20,150+(select/11)*160,19+(select%11)*20,0);
	else
		rectangle(SAD2,6+(select/11)*160,2+(select%11)*20,150+(select/11)*160,19+(select%11)*20,1);
}

void doinput() reentrant
{
	unsigned char h,l;
	switch(buttonselect)
	{
	case 17://RS485out
		if(getstr(216,122,8))
		{
			h = 0;
			while(disstr[h] != 0)
			{
				rs485out = disstr[h];
				bout485 = 1;
				RS485 = 0;			
				TI = 1;
				OSTimeDly(2);
				h++;
			}			
		}
		break;
	case 18://AD667
		if(getstrnumber(216,142,4))
		{
			da667 = (unsigned int)atoi(disstr);		
			if(da667>4095)
				da667 = 4095;
			l=da667%16;
			l=l*16;
			h=(unsigned char)(da667/16);
			LDA667=l;
			HDA667=h;
			vda667 = da667;
			vda667 /=409.5;
		}
		break;
	case 19:
		if(getstr(240,162,8))
		{
			tem8255a = 0;
			for(h=0;h<8;h++)
			{
				tem8255a *= 2;	
				if(disstr[7-h] == '1')
					tem8255a |= 0x01;							
			}
			EA = 0;
			COM8255=0x80;
			A8255 = tem8255a;
			B8255 = tem8255b;
			C8255 = tem8255c;		
			EA = 1;
		}
		break;
	case 20:
		if(getstr(240,182,8))
		{
			tem8255b = 0;
			for(h=0;h<8;h++)
			{
				tem8255b *= 2;
				if(disstr[7-h] == '1')
					tem8255b |= 0x01;
			}
			EA = 0;
			COM8255=0x80;
			A8255 = tem8255a;
			B8255 = tem8255b;
			C8255 = tem8255c;		
			EA = 1;
		}
		break;
	case 21:
		if(getstr(240,202,8))
		{
			tem8255c = 0;
			for(h=0;h<8;h++)
			{
				tem8255c *= 2;	
				if(disstr[7-h] == '1')
					tem8255c |= 0x01;							
			}
			EA = 0;
			COM8255=0x80;
			A8255 = tem8255a;
			B8255 = tem8255b;
			C8255 = tem8255c;		
			EA = 1;
		}
		break;
	default:
		setsysini();
		showmain1();
		break;
	}
}

//电磁接触器检测
void main1test(void *lcnpdata) reentrant
{
	unsigned char curstaut = 0;
	
	buttonselect=0;
	lcnpdata = lcnpdata;
	showmain1();
	showselect(buttonselect,1);
	while(1)
	{
		OSSemPend(KeySem,10,&lcnerr);
		if(lcnerr==OS_NO_ERR)
		{
			switch(key)
			{			
			case INPUT://系统参数	
				doinput();
				showselect(buttonselect,1);
				break;	
			case DOWN:
				showselect(buttonselect,0);
				if(buttonselect < 21)
				{
					buttonselect++;
				}
				else
				{
					buttonselect=0;
				}
				showselect(buttonselect,1);
				break;
			case UP:
				showselect(buttonselect,0);
				if(buttonselect > 0)
					buttonselect--;
				else
					buttonselect=21;
				showselect(buttonselect,1);
				break;
			case LEFT:
			case RIGHT:
				showselect(buttonselect,0);
				if(buttonselect > 10)
					buttonselect-=11;
				else
					buttonselect+=11;
				showselect(buttonselect,1);
				break;
			default:
				break;
			}
		}
		getcurvault();	
		showvaultall();
		if(bSecond)
		{
			bSecond=0;	
			myprintf(SAD1,TA_LEFT,216,2,  "%5u",count8253[0][0]);	
			myprintf(SAD1,TA_LEFT,216,22, "%5u",count8253[1][0]);	
			myprintf(SAD1,TA_LEFT,216,42, "%5u",count8253[2][0]);	
			myprintf(SAD1,TA_LEFT,216,62, "%5u",count8253[3][0]);	
#if OS_TASK_STAT_EN	
			myprintf(SAD1,TA_LEFT,295,2,"%3u",(unsigned int)OSCPUUsage);	
#endif
			readtimer(disstr,0);
			myprintf(SAD1,TA_LEFT,254,223,disstr);
		}
	}
}

void getcurvault() reentrant
{
	if(buttonselect>7)
		return;
	getad();
	adtemp[adtemp1] = advaut;
	switch(buttonselect)
	{
	case 1:
		ADNUMBER1=0;
		ADNUMBER2=1;
		ADNUMBER3=1;
		adtemp1=1;				
		break;
	case 2:
		ADNUMBER1=1;
		ADNUMBER2=0;
		ADNUMBER3=1;
		adtemp1=2;
		break;
	case 3:
		ADNUMBER1=0;
		ADNUMBER2=0;
		ADNUMBER3=1;
		adtemp1=3;
		break;
	case 4:
		ADNUMBER1=1;
		ADNUMBER2=1;
		ADNUMBER3=0;
		adtemp1=4;
		break;
	case 5:
		ADNUMBER1=0;
		ADNUMBER2=1;
		ADNUMBER3=0;
		adtemp1=5;
		break;
	case 6:
		ADNUMBER1=1;
		ADNUMBER2=0;
		ADNUMBER3=0;
		adtemp1=6;
		break;
	case 7:
		ADNUMBER1=0;
		ADNUMBER2=0;
		ADNUMBER3=0;
		adtemp1=7;
		break;
	case 0:
		ADNUMBER1=1;
		ADNUMBER2=1;
		ADNUMBER3=1;
		adtemp1=0;
		break;
	default:
		break;
	}
	result[adtemp1] = adtemp[adtemp1];
	result[adtemp1] /= 409.5;	//10V
}

void main (void) 
{
	OSInit();    
    InitTimer2();
    OSTaskCreate(sys_init, (void *)0, &sys_init_Stk[0],1);
    OSStart();
}

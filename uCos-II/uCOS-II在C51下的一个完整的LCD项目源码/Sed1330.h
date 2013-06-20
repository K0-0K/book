//以下程序用于SED1330控制的液晶显示模块,显示点阵为320X240
#include "stdarg.h"

#define CR        39                                   /*定义显示窗口的长度,以字符为单位*/
#define TCR       45                                   /*定义LCD的时间常数.这个时间常数应该尽量小(场频就大),防止转换时出现闪烁*/
#define LF        239                                  /*定义LCD的行数*/
#define APL       40                                   /*定义LCD一行所占缓冲区的字节数,低8位*/
#define APH       0                                    /*定义LCD一行所占缓冲区的字节数,高8位*/

#define SAD1      0X0000                               /*定义显示一区首址*/
#define SAD1L     0X00                                 /*定义显示一区首址低8位*/
#define SAD1H     0X00                                 /*定义显示一区首址高8位*/
#define SL1       239                                  /*定义显示一区所占显示屏幕的行数*/

#define SAD2      0X2800                               /*定义显示二区首址*/
#define SAD2L     0X00                                 /*定义显示二区首址低8位*/
#define SAD2H     0X28                                 /*定义显示二区首址高8位*/
#define SL2       239                                  /*定义显示二区所占显示屏幕的行数*/

#define SAD3      0X5000                               /*定义显示三区首址*/
#define SAD3L     0X00                                 /*定义显示三区首址低8位*/
#define SAD3H     0X50                                 /*定义显示三区首址高8位*/

#define SAD4      0X2800                               /*定义显示四区首址*/
#define SAD4L     0X00                                 /*定义显示四区首址低8位*/
#define SAD4H     0X28                                 /*定义显示四区首址高8位*/

#define TA_LEFT		0									//左对齐方式
#define TA_CENTER	1									//中心对齐方式
///////////////////////
void wrstr(unsigned int sad,unsigned int hnum,unsigned char vnum,unsigned char* str);
void wrdot(unsigned int sad,unsigned int hnum,unsigned char vnum);
void initlcd();
void rectangle(unsigned int sad,unsigned int strhnum,unsigned char strvnum,unsigned int endhnum,unsigned char endvnum,unsigned char fill);
void wrchat(unsigned int sad,unsigned int hnum,unsigned char vnum,unsigned char chatnum);
void liney(unsigned int sad,unsigned int x,unsigned char y1,unsigned char y2);
void linex(unsigned int sad,unsigned char y,unsigned int x1,unsigned int x2);
//////////////////////
/*设定三个显示区域为:第一显示区   0000H----27FFH  (10K容量)
                     第二显示区   2800H----4FFFH  (10K容量)
                     第三显示区   5000H----77FFH  (10K容量)*/
const unsigned char  LCDSYSINIT[8]={0X30,0X87,0X07,CR,TCR,LF,APL,APH};                          /*主LCD0的SYS设置参数*/
const unsigned char  LCDSCRINIT[10]={SAD1L,SAD1H,SL1,SAD2L,SAD2H,SL2,SAD3L,SAD3H,SAD4L,SAD4H};  /*主LCD0的SCR设置参数*/


/***********************************************************
*   程序名称: INITLCD                                     *
*   程序用途: 将SED1330 初始化成3层图形方式               *
*   输入参数: 无                                          *
*														  *
*                                                         *
***********************************************************/
void initlcd()           
{
	xdata unsigned char counter;
	
	LCDORDER=0X40;							/*送入SYS命令*/
	for(counter=0;counter<=7;counter++)
	{LCDDATA=LCDSYSINIT[counter];}			/*送入8个SYSTERM数据*/
	
	LCDORDER=0X44;							/*送入SCROLL命令*/
	for(counter=0;counter<=9;counter++)
	{LCDDATA=LCDSCRINIT[counter];}			/*送入十个SCROLL数据*/
	
	LCDORDER=0X5A;							/*送入HDOTSCR命令*/
	LCDDATA=0X00;							/*送入HDOTSCR数据*/
	
	LCDORDER=0X4F;
	
	LCDORDER=0X5B;							/*送入OVLAY命令*/
	LCDDATA=0X1D;							/*送入OVLAY数据,显示方式==(L1*L2)+L3 */
	
	LCDORDER=0X59;							/*开关显示*/
	LCDDATA=0X40;							/*开启一.二四.三, 关闭光标*/
	//LCDDATA=0X04|0x10|0x40;							/*开启一.二四.三, 关闭光标*/
	
	
	LCDORDER=0X5D;							/*设置光标*/
	LCDDATA=0X07;							/*水平点数为7*/
	LCDDATA=0X87;							/*垂直点数为7,阴影显示*/	
}
/***********************************************************
*   程序名称: WRDOT()                                     *
*   程序用途: 在指定层的指定位置写点.                     *
*   输入参数: sad    指定层的内存地址                     *
*             hnum   点横坐标                             *
*             vnum   点纵坐标                             *
*                                                         *
***********************************************************/
void wrdot(unsigned int sad,unsigned int hnum,unsigned char vnum)
{
	unsigned char flag;
    union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
    curadd.add=vnum*(APL)+hnum/8+sad;
    flag=hnum-(hnum/8)*8;
    flag=(0x80)>>flag;
	
    LCDORDER=0X46;                       /*设置光标地址*/
    LCDDATA=curadd.addhalf.addlo;
    LCDDATA=curadd.addhalf.addhi;
	
    LCDORDER=0X43;                       /*取出该点所在地址的数据*/ 
    flag=flag|(LCDORDER);
	
    LCDORDER=0X46;
    LCDDATA=curadd.addhalf.addlo;
    LCDDATA=curadd.addhalf.addhi;
	
    LCDORDER=0X42;
    LCDDATA=flag;	
}                    
/***********************************************************
*   程序名称: WRDOT()                                     *
*   程序用途: 在指定层的指定位置清点.                     *
*   输入参数: sad    指定层的内存地址                     *
*             hnum   点横坐标                             *
*             vnum   点纵坐标                             *
*                                                         *
***********************************************************/
/*void clsdot(unsigned int sad,unsigned int hnum,unsigned char vnum)
{
	xdata unsigned char flag;
    xdata union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
    curadd.add=vnum*(APL)+hnum/8+sad;
    flag=hnum-(hnum/8)*8;
    flag=(0x80)>>flag;
	flag=~flag;
	
    LCDORDER=0X46;                       //设置光标地址
    LCDDATA=curadd.addhalf.addlo;
    LCDDATA=curadd.addhalf.addhi;
	
    LCDORDER=0X43;                       //取出该点所在地址的数据
    flag=flag&(LCDORDER);
	
    LCDORDER=0X46;
    LCDDATA=curadd.addhalf.addlo;
    LCDDATA=curadd.addhalf.addhi;
	
    LCDORDER=0X42;
    LCDDATA=flag;	
}           */
/***********************************************************
*   程序名称: CLSSED()                                     *
*   程序用途: 清空显示器			                       *
*   输入参数: 无										   *
*								                           *
*								                           *
*                                                          *
************************************************************/
void clssed(unsigned int sad)
{
	xdata union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
    curadd.add=sad;
    LCDORDER=0X46;                       /*设置光标地址*/
    LCDDATA=curadd.addhalf.addlo;
    LCDDATA=curadd.addhalf.addhi;
	LCDORDER=0X4C;
	LCDORDER=0X42;
	sad=0;
	while(sad<9600)
	{
		LCDDATA=0X00;
		sad++;
	}
}           

/***********************************************************
*   程序名称: LINEX()                                      *
*   程序用途: 作水平线  			                       *
*   输入参数: sad    指定层的内存地址                      *
*			  y位置										   *
*			  x1起点									   * 
*			  x2终点			                           *
*								                           *
*                                                          *
************************************************************/
void linex(unsigned int sad,unsigned char y,unsigned int x1,unsigned int x2)
{
	do{
		wrdot(sad,x1,y);
		x1++;
	}while(x1<=x2);
}    

/***********************************************************
*   程序名称: LINEY()                                      *
*   程序用途: 作垂直线  			                       *
*   输入参数: sad    指定层的内存地址                      *
*			  x位置										   *
*			  y1起点									   * 
*			  y2终点			                           *
*								                           *
*                                                          *
************************************************************/
void liney(unsigned int sad,unsigned int x,unsigned char y1,unsigned char y2)
{
	do{
		wrdot(sad,x,y1);
		y1++;
	}while(y1<=y2);
}    
/***********************************************************
*   程序名称: LINE()                                       *
*   程序用途: 作垂直线  			                       *
*   输入参数: sad    指定层的内存地址                      *
*			  x1起点									   *
*			  x2终点									   *
*			  y1起点									   * 
*			  y2终点			                           *
*								                           *
*                                                          *
************************************************************/
/*void line(unsigned int sad,unsigned int x1,unsigned int x2,unsigned char y1,unsigned char y2)
{
	unsigned char tem,tem1;
	if(y2>y1)
	{
		tem=y2-y1;
		for(tem1=0;tem1<=tem/2;tem1++)
		{
			wrdot(sad,x1,y1+tem1);
			wrdot(sad,x2,y1+tem/2+tem1+1);
		}
	}
	else
	{
		tem=y1-y2;
		for(tem1=0;tem1<=tem/2;tem1++)
			wrdot(sad,x1,y1-tem1);
		for(tem1=1;tem1<=tem/2;tem1++)
			wrdot(sad,x2,y1-tem/2-tem1);
	}
}    */

/***********************************************************
*   程序名称: WRCHAT()                                    *
*   程序用途: 在指定层的指定位置写ASCII字符               *
*   输入参数: sad    指定层的内存地址                     *
*             hnum   点横坐标                             *
*             vnum   点纵坐标                             *
*             chatnum字符的序号值                         *
***********************************************************/
void wrchatasc(unsigned int sad,unsigned int hnum,unsigned char vnum,unsigned char chatnum)
{
	unsigned char buff[16];                   /*缓存,用于读写ASCII字符的16*16点阵*/
	unsigned char counter;
	unsigned char flag1;
	unsigned char flag2;
	unsigned char offset;
	union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
	curadd.add=vnum*(APL)+hnum/8+sad;      /*计算出首址*/
	offset=hnum-(hnum/8)*8;               /*计算出字符内的偏移量*/
	
	LCDORDER=0X46;                         /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	
	/*以下写字符的左部*/
	LCDORDER=0X4F;                         /*确定光标移动方向为向下*/
	
	flag1=(0XFF)<<(8-offset);
	flag2=~flag1;
	
		
	LCDORDER=0X43;                         /*由光标处开始,向下读入16个数据*/                                        
	for(counter=0;counter<16;counter++)
    {  /*由于LCDORDER的操作是一次写,一次读,C51可能在读操作时优化成直接使用写的数据,
		而不进行读操作,所以加一个无用指令*/
		buff[counter]=(LCDORDER&flag1)|(ASC_MSK[chatnum][counter]>>offset);
    }
	
	LCDORDER=0X46;                         /*再次确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	LCDORDER=0X42;
	for(counter=0;counter<16;counter++)
    {
		(LCDDATA)=buff[counter];
	}
	

	curadd.add=curadd.add+1;         /*地址加1,移到右部*/
	
	LCDORDER=0X4F;                   /*确定光标移动方向为向下*/
	
	LCDORDER=0X46;                   /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	flag1=(0XFF)>>offset;
	flag2=~flag1;
	flag1=0XFF>>offset;
				
	LCDORDER=0X43;                   /*由光标处开始,向下读入16个数据*/
	for(counter=0;counter<16;counter++)
	{
		buff[counter]=(LCDORDER&flag1)|(ASC_MSK[chatnum][counter]<<(8-offset));
	}
	
	LCDORDER=0X46;                   /*再次确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	LCDORDER=0X42;
	for(counter=0;counter<16;counter++)
	{
		(LCDDATA)=buff[counter];
	}
}
/***********************************************************
*   程序名称: WRCHAT()                                    *
*   程序用途: 在指定层的指定位置写汉字字符                    *
*   输入参数: sad    指定层的内存地址                     *
*             hnum   点横坐标                             *
*             vnum   点纵坐标                             *
*             chatnum字符的序号值                         *
***********************************************************/
void wrchat(unsigned int sad,unsigned int hnum,unsigned char vnum,unsigned char chatnum)
{
	unsigned char buff[16];                   /*缓存,用于读写ASCII字符的16*16点阵*/
	unsigned char counter;
	unsigned char flag1;
	unsigned char flag2;
	unsigned char offset;
	union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
	curadd.add=vnum*(APL)+hnum/8+sad;      /*计算出首址*/
	offset=hnum-(hnum/8)*8;               /*计算出字符内的偏移量*/
	
	LCDORDER=0X46;                         /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	
	/*以下写字符的左部*/
	LCDORDER=0X4F;                         /*确定光标移动方向为向下*/
	
	flag1=(0XFF)<<(8-offset);
	flag2=~flag1;
	
		
	LCDORDER=0X43;                         /*由光标处开始,向下读入16个数据*/                                        
	for(counter=0;counter<16;counter++)
    	buff[counter]=(LCDORDER&flag1)|(GB_16[chatnum].Msk[counter]>>offset);
    
	LCDORDER=0X46;                         /*再次确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	LCDORDER=0X42;
	for(counter=0;counter<16;counter++)
    	LCDDATA=buff[counter];
	

    /*以下写字符的右部*/
	curadd.add=curadd.add+1;         /*地址加1,移到右部*/
	
	LCDORDER=0X4F;                   /*确定光标移动方向为向下*/
	
	LCDORDER=0X46;                   /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	flag1=(0XFF)>>offset;
	flag2=~flag1;
	flag1=0XFF>>offset;
				
	LCDORDER=0X43;                   /*由光标处开始,向下读入16个数据*/
	for(counter=0;counter<16;counter++)
		buff[counter]=(LCDORDER&flag1)|(GB_16[chatnum].Msk[counter]<<(8-offset));
	
	LCDORDER=0X46;                   /*再次确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	LCDORDER=0X42;
	for(counter=0;counter<16;counter++)
		LCDDATA=buff[counter];
	
////////////////////////////////////////////////
	LCDORDER=0X46;                         /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	
	/*以下写字符的左部*/
	LCDORDER=0X4F;                         /*确定光标移动方向为向下*/
	
	flag1=(0XFF)<<(8-offset);
	flag2=~flag1;	
		
	LCDORDER=0X43;                         /*由光标处开始,向下读入16个数据*/                                        
	for(counter=0;counter<16;counter++)
    	buff[counter]=(LCDORDER&flag1)|(GB_16[chatnum].Msk[counter+16]>>offset);
    
	LCDORDER=0X46;                         /*再次确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	LCDORDER=0X42;
	for(counter=0;counter<16;counter++)
		LCDDATA=buff[counter];
	
	
	curadd.add=curadd.add+1;         /*地址加1,移到右部*/
	
	LCDORDER=0X4F;                   /*确定光标移动方向为向下*/
	
	LCDORDER=0X46;                   /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	flag1=(0XFF)>>offset;
	flag2=~flag1;
	flag1=0XFF>>offset;
				
	LCDORDER=0X43;                   /*由光标处开始,向下读入10个数据*/
	for(counter=0;counter<16;counter++)
		buff[counter]=(LCDORDER&flag1)|(GB_16[chatnum].Msk[counter+16]<<(8-offset));
	
	LCDORDER=0X46;                   /*再次确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	LCDORDER=0X42;
	for(counter=0;counter<16;counter++)
		LCDDATA=buff[counter];
}
void myprintf(unsigned int sad,unsigned char bcenter,unsigned int hnum,unsigned char vnum,unsigned char *fmt, ...)
{
	va_list arg_ptr;
	unsigned char tmpBuf[64];				// LCD显示数据缓冲区
	unsigned char i,uLen,j;
	unsigned char c1,c2;
	
	va_start(arg_ptr,fmt);
	uLen=(unsigned char)vsprintf(tmpBuf,fmt,arg_ptr);
	va_end(arg_ptr);
	
	if(bcenter)
		hnum=hnum-(uLen*4);
	i=0;
	while(i<uLen)
	{
		c1 = tmpBuf[i];
		c2 = tmpBuf[i+1];
		if(c1<128)//ASCII
		{
			if(c1<31)
			{
				if(c1==13||c1==10)
				{					
					i++;
					if(vnum<224)
						vnum+=16;
					else
						vnum=0;
					hnum=0;
					continue;
				}		
				c1=31;
			}
			wrchatasc(sad,hnum,vnum,(c1-31));
			hnum+=8;
		}
		else
		{	// 中文
			for(j=0;j<sizeof(GB_16)/sizeof(GB_16[0]);j++)
			{
				if(c1==GB_16[j].Index[0]&&c2==GB_16[j].Index[1])
				{
					wrchat(sad,hnum,vnum,j);
					break;
				}
			}			
			hnum+=16;
			i++;
		}
		i++;
		if(hnum>312)
		{
			hnum=0;
			if(vnum<224)
				vnum+=17;
			else
				vnum=0;
		}		
	}
}

#if Number361
void wrnumber(unsigned int sad,unsigned char hnum,unsigned char vnum,unsigned char chatnum)
{
	unsigned char counter;
	union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}curadd;
	curadd.add=vnum*(APL)+hnum+sad;      /*计算出首址*/
	LCDORDER=0X46;                         /*确定光标地址*/
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	
	
	LCDORDER=0X4F;                         /*确定光标移动方向为向下*/
	LCDORDER=0X42;
	for(counter=0;counter<36;counter++)
    	LCDDATA=mynumbermsk[chatnum][counter];
    //以下写字符的右部
	curadd.add++;	
	LCDORDER=0X46;                   
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	LCDORDER=0X42;
	for(counter=0;counter<36;counter++)
    	LCDDATA=mynumbermsk[chatnum][counter+36];
	curadd.add++;	
	LCDORDER=0X46;                   
	LCDDATA=curadd.addhalf.addlo;
	LCDDATA=curadd.addhalf.addhi;
	LCDORDER=0X42;
	for(counter=0;counter<36;counter++)
    	LCDDATA=mynumbermsk[chatnum][counter+72];
    
}

void mynuprintf(unsigned int sad,unsigned char hnum,unsigned char vnum,unsigned char *fmt, ...)
{
	va_list arg_ptr;
	unsigned char tmpBuf[16];				// LCD显示数据缓冲区
	unsigned char i,uLen;
	
	va_start(arg_ptr,fmt);
	uLen=(unsigned char)vsprintf(tmpBuf,fmt,arg_ptr);
	va_end(arg_ptr);
	
	i=0;
	hnum=hnum-(uLen*3);
	while(i<uLen)
	{
		if(tmpBuf[i]<58&&tmpBuf[i]>47)
		{
			wrnumber(sad,hnum,vnum,(tmpBuf[i]-47));
			hnum+=3;
		}
		else
		{
			if(tmpBuf[i]=='.')
			{
				wrnumber(sad,hnum,vnum,0);
				hnum+=1;
			}
			else
			{
				if(tmpBuf[i]=='-')
					wrnumber(sad,hnum,vnum,12);
				else
					wrnumber(sad,hnum,vnum,11);
				hnum+=3;
			}			
		}
		i++;		
	}
}

#endif


/*void rectangl(unsigned int sad,unsigned int strhnum,unsigned char strvnum,unsigned int endhnum,unsigned char endvnum)
{
	linex(sad,strvnum,strhnum,endhnum);
	linex(sad,endvnum,strhnum,endhnum);
	liney(sad,strhnum,strvnum,endvnum);
	liney(sad,endhnum,strvnum,endvnum);
}*/
/***********************************************************
*   程序名称: RECTANGLE()                                 *
*   程序用途: 在指定层的指定位置填充矩形                  *
*   输入参数: sad       指定层的内存起始地址              *
*             strhnum   起始点横坐标                      *
*             strvnum   起始点纵坐标                      *
*             endrhnum  结束点横坐标                      *
*             endvnum   结束点纵坐标                      *
*             fill      填充类型(1:填充1;0填充0)          *
***********************************************************/
void rectangle(unsigned int sad,unsigned int strhnum,unsigned char strvnum,unsigned int endhnum,unsigned char endvnum,unsigned char fill)
{
	xdata unsigned char count1;
	xdata unsigned char count2;
	xdata LCD_BUFF[240];                          /*屏幕一列的缓冲*/ 
	xdata unsigned char stroffset;
	xdata unsigned char endoffset;
	
	xdata unsigned char flag_str;                 /*用于屏蔽起始列的数值*/
	xdata unsigned char flag_end;                 /*用于屏蔽结束列的数值*/ 
	
	xdata union {unsigned int add;
	struct {unsigned char addhi;unsigned char addlo;}addhalf;
	}stradd,endadd;
	
	
	if(fill==0){fill=0;}                           /*根据要写1或者写1*/
	else{fill=0XFF;}
	
	stradd.add=strvnum*(APL)+strhnum/8+sad;        /*计算出水平起始点的地址*/
	endadd.add=strvnum*(APL)+endhnum/8+sad;        /*计算出水平结束点的地址*/
	
	stroffset=strhnum-(strhnum/8)*8;               /*计算出水平起始点字符内的偏移量*/
	endoffset=endhnum-(endhnum/8)*8;               /*计算出水平结束点字符内的偏移量*/ 
	
	flag_str=0XFF<<(8-stroffset);               
	flag_end=0XFF>>(endoffset+1);    
	
	if(stradd.add==endadd.add)                     /*如果水平的起始点的地址和结束点的地址相同*/
	{
		flag_str=flag_str|flag_end;
	}                /*起始结束在同一字节内,头部尾部不能改写*/
	
	
	/*+++++++++++++++读写起始列的屏幕数据+++++++++++++++*/
	LCDORDER=0X46;                                 /*设定光标地址*/
	LCDDATA=stradd.addhalf.addlo;
	LCDDATA=stradd.addhalf.addhi;                  /*确定光标地址*/
	
	
	LCDORDER=0X4F;                                 /*确定光标移动方向为向下*/
	
	LCDORDER=0X43;                                 /*准备读*/
	for(count1=0;count1<=(endvnum-strvnum);count1++)
    {LCD_BUFF[count1]=LCDORDER;} 
	
	/*+++++++++++++++处理起始列的屏幕数据+++++++++++++++*/ 
	for(count1=0;count1<=(endvnum-strvnum);count1++) 
    {LCD_BUFF[count1]=( LCD_BUFF[count1] & flag_str ) | ( fill & (~flag_str) );}  
	/*++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
	
	
	/*+++++++++++++++返回起始列的屏幕数据+++++++++++++++*/
	LCDORDER=0X46;                                 /*设定光标地址*/
	LCDDATA=stradd.addhalf.addlo;
	LCDDATA=stradd.addhalf.addhi;                  /*确定光标地址*/
	
	
	LCDORDER=0X4F;                                 /*确定光标移动方向为向下*/
	
	LCDORDER=0X42;                                 /*准备写*/
	for(count1=0;count1<=(endvnum-strvnum);count1++) 
    {LCDDATA=LCD_BUFF[count1];} 
	/*+++++++++++++++结束列的屏幕数据改写+++++++++++++++*/
	
	if(stradd.add<endadd.add)                      /*如果水平的起始点的地址和结束点的地址不相同*/ 
	{
		/*+++++++++++++++读写结束列的屏幕数据+++++++++++++++*/
		LCDORDER=0X46;                             /*设定光标地址*/
		LCDDATA=endadd.addhalf.addlo;
		LCDDATA=endadd.addhalf.addhi;              /*确定光标地址*/
		
		
		LCDORDER=0X4F;                             /*确定光标移动方向为向下*/
		
		LCDORDER=0X43;                             /*准备读*/
		for(count1=0;count1<=(endvnum-strvnum);count1++)
		{LCD_BUFF[count1]=LCDORDER;} 
		
		/*+++++++++++++++处理结束列的屏幕数据+++++++++++++++*/ 
		for(count1=0;count1<=(endvnum-strvnum);count1++)
		{LCD_BUFF[count1]=( LCD_BUFF[count1] & flag_end ) | ( fill & (~flag_end) );}  
		
		/*+++++++++++++++返回结束列的屏幕数据+++++++++++++++*/
		LCDORDER=0X46;                               /*设定光标地址*/
		LCDDATA=endadd.addhalf.addlo;
		LCDDATA=endadd.addhalf.addhi;                /*确定光标地址*/
		
		
		LCDORDER=0X4F;                               /*确定光标移动方向为向下*/
		
		LCDORDER=0X42;                               /*准备写*/
		for(count1=0;count1<=(endvnum-strvnum);count1++)
		{LCDDATA=LCD_BUFF[count1];}    	   
		
	}
	
	/*+++++++++++++++中间列的屏幕数据改写+++++++++++++++*/
	if((endadd.add-stradd.add)>=2 )               /*结束地址大于起始地址2个以上,说明有中间列*/ 
	{
		stradd.add=stradd.add+1;                    /*水平起始地址加1*/
		
		for(count2=( (strhnum/8)+1);count2<=((endhnum/8)-1);count2++)
		{
			/*+++++++++++++++处理中间列的屏幕数据+++++++++++++++*/ 
			for(count1=0;count1<=(endvnum-strvnum);count1++)
			{LCD_BUFF[count1]=fill;}   
			
			/*+++++++++++++++返回中间列的屏幕数据+++++++++++++++*/
			LCDORDER=0X46;                           /*设定光标地址*/
			LCDDATA=stradd.addhalf.addlo;
			LCDDATA=stradd.addhalf.addhi;            /*确定光标地址*/
			
			LCDORDER=0X4F;                           /*确定光标移动方向为向下*/
			
			LCDORDER=0X42;                           /*准备写*/
			for(count1=0;count1<=(endvnum-strvnum);count1++)
			{
				LCDDATA=LCD_BUFF[count1];
			}
			stradd.add=stradd.add+1;                 /*水平起始地址加1*/
		}   
	}   
}  
//////////////////////////////ENDSED1330////////////// 

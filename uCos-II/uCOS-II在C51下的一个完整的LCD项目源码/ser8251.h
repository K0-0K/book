void ser8251init(unsigned char tbaud) reentrant
{
	unsigned int tem = 0;
	unsigned char cursernu;
	switch(tbaud)
	{
	case 1://2400
		SERCOM8253=0x36;
		SERA8253=0x30;
		SERA8253=0x00;
		break;
	case 2://4800
		SERCOM8253=0x36;
		SERA8253=0x24;
		SERA8253=0x00;
		break;
	case 3://9600
		SERCOM8253=0x36;
		SERA8253=0x0C;
		SERA8253=0x00;
		break;
	default://9600
		SERCOM8253=0x36;
		SERA8253=0x0C;
		SERA8253=0x00;
		break;
	}	
	SERCOM8251=0xFF;
	DelayMs(50);
	SERCOM8251=0x4E;
	SERCOM8251=0x14;
	cursernu = 0;
	while(cursernu<0xFE)
	{
		cursernu++;
		tem = SERCOM8251;
		SERCOM8251=0xFF;
		DelayMs(50);
		SERCOM8251=0x4E;
		SERCOM8251=0x14;
		if(tem==0x85)
			break;
	}
	IT0=0;
	EX0 = 1;
}

void get8251ser(void) reentrant
{
	rs232in=SERDAT8251;
}



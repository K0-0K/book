/*
*********************************************************************************************************
*                                          PC SUPPORT FUNCTIONS
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : PC.H
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               CONSTANTS
*                                    COLOR ATTRIBUTES FOR VGA MONITOR
*
* Description: These #defines are used in the PC_Disp???() functions.  The 'color' argument in these
*              function MUST specify a 'foreground' color, a 'background' and whether the display will
*              blink or not.  If you don't specify a background color, BLACK is assumed.  You would 
*              specify a color combination as follows:
*
*              PC_DispChar(0, 0, 'A', DISP_FGND_WHITE + DISP_BGND_BLUE + DISP_BLINK);
*
*              To have the ASCII character 'A' blink with a white letter on a blue background.
*********************************************************************************************************
*/
#define DISP_FGND_BLACK           0x00
#define DISP_FGND_BLUE            0x01
#define DISP_FGND_GREEN           0x02
#define DISP_FGND_CYAN            0x03
#define DISP_FGND_RED             0x04
#define DISP_FGND_PURPLE          0x05
#define DISP_FGND_BROWN           0x06
#define DISP_FGND_LIGHT_GRAY      0x07
#define DISP_FGND_DARK_GRAY       0x08
#define DISP_FGND_LIGHT_BLUE      0x09
#define DISP_FGND_LIGHT_GREEN     0x0A
#define DISP_FGND_LIGHT_CYAN      0x0B
#define DISP_FGND_LIGHT_RED       0x0C
#define DISP_FGND_LIGHT_PURPLE    0x0D
#define DISP_FGND_YELLOW          0x0E
#define DISP_FGND_WHITE           0x0F

#define DISP_BGND_BLACK           0x00
#define DISP_BGND_BLUE            0x10
#define DISP_BGND_GREEN           0x20
#define DISP_BGND_CYAN            0x30
#define DISP_BGND_RED             0x40
#define DISP_BGND_PURPLE          0x50
#define DISP_BGND_BROWN           0x60
#define DISP_BGND_LIGHT_GRAY      0x70

#define DISP_BLINK                0x80

/*$PAGE*/
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void    PC_DispChar(INT8U x, INT8U y, INT8U c, INT8U color);
void    PC_DispClrCol(INT8U x, INT8U bgnd_color);
void    PC_DispClrRow(INT8U y, INT8U bgnd_color);
void    PC_DispClrScr(INT8U bgnd_color);
void    PC_DispStr(INT8U x, INT8U y, INT8U *s, INT8U color);

void    PC_DOSReturn(void);
void    PC_DOSSaveReturn(void);

void    PC_ElapsedInit(void);
void    PC_ElapsedStart(void);
INT16U  PC_ElapsedStop(void);

void    PC_GetDateTime(char *s);
BOOLEAN PC_GetKey(INT16S *c);

void    PC_SetTickRate(INT16U freq);

void   *PC_VectGet(INT8U vect);
void    PC_VectSet(INT8U vect, void (*isr)(void));

/* ************************************
*《精通Windows API》 
* 示例代码
* display.c
* 10.1  系统信息
**************************************/
#include <windows.h>
#include <stdio.h>

void main()
{
   BOOL fResult;
   int aMouseInfo[3];
 
   fResult = GetSystemMetrics(SM_MOUSEPRESENT); 
 
   if (fResult == 0) 
      printf("No mouse installed.\n"); 
   else 
   { 
      printf("Mouse installed.\n");

      // Determine whether the buttons are swapped. 

      fResult = GetSystemMetrics(SM_SWAPBUTTON); 
 
      if (fResult == 0) 
         printf("Buttons not swapped.\n"); 
      else printf("Buttons swapped.\n");
 
      // Get the mouse speed and the threshold values. 
 
      fResult = SystemParametersInfo(
         SPI_GETMOUSE,  // get mouse information 
         0,             // not used 
         &aMouseInfo,   // holds mouse information 
         0);            // not used 

      if( fResult )
      { 
         printf("Speed: %d\n", aMouseInfo[2]); 
         printf("Threshold (x,y): %d,%d\n", 
            aMouseInfo[0], aMouseInfo[1]); 
      }
   } 
}

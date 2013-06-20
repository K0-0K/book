/* ************************************
*《精通Windows API》 
* 示例代码
* global.c
* 5.3.3  使用全局和局部函数分配和释放内存
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>

/*************************************
* int main(void)
* 功能	演示Global*函数的使用
*
* 参数	无
*
**************************************/
int main(void)
{	
	LPVOID lpMem;	//内存地址
	HGLOBAL hMem;	//内存句柄
	SIZE_T sizeMem;	//内存大小
	UINT uFlags;	//属性

	//分配内存
	lpMem = (LPVOID)GlobalAlloc(GPTR,1000);
	//将数据复制到内存中
	lstrcpy(lpMem,"this is a string");	
	//获得内存属性，打印
	uFlags = GlobalFlags(lpMem);
	printf("内存中的内容：\"%s\"，内存地址：0x%.8x，内存属性：%u\n",
		lpMem,lpMem,uFlags);
	//释放
	GlobalFree(lpMem);
	//分配内存，获取信息
	hMem = GlobalAlloc(GMEM_MOVEABLE,1000);
	sizeMem = GlobalSize(hMem);
	uFlags = GlobalFlags(hMem);
	//打印信息
	printf("内存大小：%d，内存句柄：0x%.8x，内存属性：%u\n",
		sizeMem,hMem,uFlags);
	//锁定
	lpMem = GlobalLock(hMem);
	//释放
	GlobalFree(hMem);
	return 0;
}
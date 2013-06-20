/* ************************************
*《精通Windows API》 
* 示例代码
* virtual.c
* 5.4.2  分配和释放可读可写的虚拟内存页面
**************************************/

/* 头文件　*/
#include <windows.h>
#include <stdio.h>

/*************************************
* int main(void)
* 功能	演示虚拟内存的使用
*
* 参数	未使用
**************************************/
int main(void)
{	
	SIZE_T sizeVirtual = 4000;		//大小
	LPVOID lpRound = (LPVOID)0x100000FF;	//地址
	MEMORY_BASIC_INFORMATION  mbi;	//内存信息

	//分配内存，直接分配已提交的内存
	LPVOID lpAddress = VirtualAlloc(
		lpRound,sizeVirtual,
		MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE
		);
	if(lpAddress == NULL)
	{
		printf("VirtualAlloc error: %d\n",GetLastError());
		return 1;
	}
	printf("Alloc:MEM_COMMIT|MEM_RESERVE\n");
	//复制数据到内存中
	CopyMemory(lpAddress,"hello",lstrlen("hello"));
	printf("分配、复制成功，地址：0x%.8x，内容：%s\n",
		lpAddress,lpAddress);
	//获取内存信息并打印
	VirtualQuery(lpAddress,&mbi,sizeof(mbi));
	printf("使用VirtualQuery获得的信息：\n"
		"BaseAddress:0x%.8x\tAllocationBase:0x%.8x\t"
		"AllocationProtect:0x%.8x\tRegionSize:%u\t"
		"State:0x%.8x\tProtect:0x%.8x\tType:0x%.8x\n",
		mbi.BaseAddress,mbi.AllocationBase,
		mbi.AllocationProtect,mbi.RegionSize,
		mbi.State,mbi.Protect,mbi.Type
		);

	////设置为READ-ONLY属性
	//if(!VirtualProtect(lpAddress,0,PAGE_READONLY,NULL))
	//{
	//	printf("VirtualProtect error: %d",GetLastError());
	//	return 1;
	//}
	////测试READ-ONLY属性,异常
	//CopyMemory(lpAddress,"read only",lstrlen("read only"));
	//printf(lpAddress);
	////获取内存信息并打印
	//VirtualQuery(lpAddress,&mbi,sizeof(mbi));
	//printf("使用VirtualQuery获得的信息：\n"
	//	"BaseAddress:0x%.8x\tAllocationBase:0x%.8x\t"
	//	"AllocationProtect:0x%.8x\tRegionSize:%d\t"
	//	"State:0x%.8x\tProtect:0x%.8x\tType:0x%.8x\n",
	//	mbi.BaseAddress,mbi.AllocationBase,
	//	mbi.AllocationProtect,mbi.RegionSize,
	//	mbi.State,mbi.Protect,mbi.Type
	//	);

	//DECOMMIT释放，页面将变为保留状态
	printf("Free: DECOMMIT\n");
	if(!VirtualFree(lpRound,sizeVirtual,MEM_DECOMMIT))
	{
		printf("VirtualFree error: %d",GetLastError());
		return 1;
	}
	//获取内存信息并打印
	VirtualQuery(lpAddress,&mbi,sizeof(mbi));
	printf("使用VirtualQuery获得的信息：\n"
		"BaseAddress:0x%.8x\tAllocationBase:0x%.8x\t"
		"AllocationProtect:0x%.8x\tRegionSize:%u\t"
		"State:0x%.8x\tProtect:0x%.8x\tType:0x%.8x\n",
		mbi.BaseAddress,mbi.AllocationBase,
		mbi.AllocationProtect,mbi.RegionSize,
		mbi.State,mbi.Protect,mbi.Type
		);
	//释放内存
	printf("Free:RELEASE\n");
	if(!VirtualFree(lpAddress,0,MEM_RELEASE))
	{
		printf("VirtualFree error: %d",GetLastError());
		return 1;
	}
	return 0;
}
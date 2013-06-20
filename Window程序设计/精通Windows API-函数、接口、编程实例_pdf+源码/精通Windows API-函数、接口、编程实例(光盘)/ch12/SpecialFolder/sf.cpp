/* ************************************
*《精通Windows API》 
* 示例代码
* sf.c
* 12.1  Shell目录管理接口
**************************************/
/* 头文件　*/
#include <Windows.h>
#include <shlobj.h>
#include <stdio.h>

/* 函数申明　*/
DWORD ListFileInRecycleBin();
VOID GetSpecialFolder();

/*************************************
* int main()
* 功能	调用相关函数
*
* 参数	未使用
**************************************/
int main()
{
	GetSpecialFolder();
	ListFileInRecycleBin();
}
/*************************************
* VOID GetSpecialFolder()
* 功能	获取并显示特殊目录
*
* 参数	未使用
**************************************/
VOID GetSpecialFolder()
{
	// 获取我的文档的路径
	CHAR szMyDocument[MAX_PATH];// My Document的路径
	// 使用SHGetSpecialFolderPath获取特殊目录路径
	SHGetSpecialFolderPath(NULL,szMyDocument,CSIDL_PERSONAL,FALSE);
	// 获取桌面的路径
	CHAR szDesktop[MAX_PATH];	//DeskTop的路径
	LPITEMIDLIST pidl = NULL;
	LPMALLOC pMalloc = NULL;
	// 分配
	SHGetMalloc(&pMalloc);
	// 使用SHGetFolderLocation、SHGetPathFromIDList可以获取任意目录的路径
	SHGetFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, &pidl);
	SHGetPathFromIDList(pidl,szDesktop);
	// 释放
	pMalloc->Free(pidl);
	pMalloc->Release();
	// 显示结果
	printf("My Document:\t %s\n",szMyDocument);
	printf("DeskTop:\t %s\n",szDesktop);
}
/*************************************
* VOID GetSpecialFolder()
* 功能	遍历并显示回收站中的文件
*
* 参数	未使用
**************************************/
DWORD ListFileInRecycleBin()
{
	CHAR pszPath[MAX_PATH];		// 保存路径
	// IShellFolder接口
	IShellFolder *pisf = NULL;
	IShellFolder *pisfRecBin = NULL;
	// 获取“根”目录，桌面
	SHGetDesktopFolder(&pisfRecBin);
	
	IEnumIDList *peidl = NULL;	// 对象遍历接口
	LPITEMIDLIST pidlBin = NULL;
	LPITEMIDLIST idlCurrent = NULL;

	LPMALLOC pMalloc = NULL;
	// 分配
	SHGetMalloc(&pMalloc);
	// 回收站位置
	SHGetFolderLocation(NULL, CSIDL_BITBUCKET, NULL, 0, &pidlBin);
	// 绑定回收站对象
	pisfRecBin->BindToObject(pidlBin,NULL,IID_IShellFolder,(void **) &pisf);
	// 列举回收站中的对象,得到IEnumIDList接口，包括SHCONTF_FOLDERS、
	// SHCONTF_NONFOLDERS、SHCONTF_INCLUDEHIDDEN类型的对象
	pisf->EnumObjects(NULL,
		SHCONTF_FOLDERS | SHCONTF_NONFOLDERS |SHCONTF_INCLUDEHIDDEN,
		&peidl);

	STRRET strret;
	ULONG uFetched;

	HANDLE hOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
	printf("\nFiles In Recycle Bin:\n");

	while(1)
	{
		// 遍历IEnumIDList对象，idlCurrent为当前对象
		if(peidl->Next(1,&idlCurrent,&uFetched) == S_FALSE)
			break;
		// 获取回收站当前对象当前的路径，这里没有输出结果，读者可自行修改
		SHGetPathFromIDList(idlCurrent,  pszPath);
		// DisplayName，删除前的路径
		pisf->GetDisplayNameOf(idlCurrent,SHGDN_NORMAL,&strret);
		// 显示，printf可能会造成字符编码不正确。
		WriteConsoleW(hOutPut,L"\t",1,NULL,NULL);
		WriteConsoleW(hOutPut,strret.pOleStr,lstrlenW(strret.pOleStr),NULL,NULL);
		WriteConsoleW(hOutPut,L"\n",1,NULL,NULL);
	}
	// 释放资源
	pMalloc->Free(pidlBin);
	pMalloc->Free(strret.pOleStr);
	pMalloc->Release();
	peidl->Release();
	pisf->Release();
	return 0;	
}

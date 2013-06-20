#include <Windows.h>
//#include <shobjidl.h>
#include <shlobj.h>

//#pragma comment (lib , "shell32.lib");
DWORD ListFileInRecycleBin(HWND hwnd);
int WinMain(
			HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine,
			int nCmdShow
			)
{

	CHAR szMyDocument[MAX_PATH];	// My Document的路径
	CHAR szDesktop[MAX_PATH];	//DeskTop的路径
	
	LPITEMIDLIST pidl = NULL;
	LPMALLOC pMalloc = NULL;
	// 第一种方法获取特殊目录路径
	SHGetSpecialFolderPath(NULL,szMyDocument,CSIDL_PERSONAL,FALSE);

	SHGetMalloc(&pMalloc);
	// 第二种访求获取特殊目录路径
	SHGetFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, &pidl);
	SHGetPathFromIDList(pidl,szDesktop);

	pMalloc->Free(pidl);
	pMalloc->Release();

	ListFileInRecycleBin(NULL);
}

DWORD ListFileInRecycleBin(HWND hwndShowResult)
{
	CHAR pszPath[MAX_PATH];

	IShellFolder *pisf = NULL;
	IShellFolder *pisfDesktop = NULL;

	SHGetDesktopFolder(&pisfDesktop);

	IEnumIDList *peidl = NULL;

	LPITEMIDLIST pidlBin = NULL;

	LPITEMIDLIST idlCurrent = NULL;
	BROWSEINFO bi = {0};
	LPMALLOC pMalloc = NULL;

	STRRET strret;
	ULONG uFetched;

	SHGetMalloc(&pMalloc);

	SHGetFolderLocation(NULL, CSIDL_BITBUCKET, NULL, 0, &pidlBin);

	//REFIID riid;

	pisfDesktop->BindToObject(pidlBin,NULL,IID_IShellFolder,(void **) &pisf);

	pisf->EnumObjects(NULL,
		SHCONTF_FOLDERS | SHCONTF_NONFOLDERS |SHCONTF_INCLUDEHIDDEN,
		&peidl);



	while(1)
	{
		if(peidl->Next(1,&idlCurrent,&uFetched) == S_FALSE)
			break;
		SHGetPathFromIDList(idlCurrent,  pszPath);
		pisf->GetDisplayNameOf(idlCurrent,SHGDN_NORMAL,&strret);
	}

	pMalloc->Free(pidlBin);
	pMalloc->Free(strret.pOleStr);
	pMalloc->Release();

	peidl->Release();
	pisf->Release();
	return 0;	
}


DWORD Browse(HWND hwndShowResult) 
{
	CHAR szRecycleBin[MAX_PATH];

	LPITEMIDLIST pidlBin = NULL;

	LPITEMIDLIST pidlSelected = NULL;
	BROWSEINFO bi = {0};
	LPMALLOC pMalloc = NULL;

	SHGetMalloc(&pMalloc);

	SHGetFolderLocation(NULL, CSIDL_PERSONAL, NULL, 0, &pidlBin);
	SHGetPathFromIDList(pidlBin,szRecycleBin);

	bi.hwndOwner = hwndShowResult;
	bi.pidlRoot = pidlBin;
	bi.pszDisplayName = "pszDisplayName";
	bi.lpszTitle = "Choose a folder";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.lParam = 0;

	pidlSelected = SHBrowseForFolder(&bi);

	pMalloc->Free(pidlBin);
	pMalloc->Release();

	//	BOOL SHGetPathFromIDList(          LPCITEMIDLIST pidl,
	//    LPTSTR pszPath
	//);

	return 0;
}
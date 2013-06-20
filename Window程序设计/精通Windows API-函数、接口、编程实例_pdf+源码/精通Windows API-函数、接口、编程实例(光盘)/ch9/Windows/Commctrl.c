/* ************************************
*《精通Windows API》 
* 示例代码
* MainWindow.c
* 9.3-9.6  图形用户界面
**************************************/
/* 头文件 */
#include <windows.h>
#include <commctrl.h>
#include <Winuser.h>
#include <shellapi.h>
#include "resource.h"
#include "Window.h"
/* 全局变量 */
HINSTANCE hinst;
HWND hwndMain;
HWND hwndTreeView;
HWND hwndListView;
HWND hwndEdit;
HMENU hMenuMain;
HMENU hPopup;
// 在界面上显示的文本
LPSTR lpszLatin =  "Lorem ipsum dolor sit amet, consectetur "
		"adipisicing elit, sed do eiusmod tempor " 
		"incididunt ut labore et dolore magna " 
		"aliqua. Ut enim ad minim veniam, quis " 
		"nostrud exercitation ullamco laboris nisi " 
		"ut aliquip ex ea commodo consequat. Duis " 
		"aute irure dolor in reprehenderit in " 
		"voluptate velit esse cillum dolore eu " 
		"fugiat nulla pariatur. Excepteur sint " 
		"occaecat cupidatat non proident, sunt " 
		"in culpa qui officia deserunt mollit " 
		"anim id est laborum."; 

/* ************************************
* HWND CreateTreeView(HWND hwndParent, LPSTR szWindowName)
* 功能	创建一个Tree View控件，并调用InitTreeViewItems增加若干个节点
* 参数	hwndParent，父窗口句柄
*			szWindowName，窗口标题
* 返回值	创建的窗口句柄
**************************************/
HWND CreateTreeView(HWND hwndParent, LPSTR szWindowName)
{
	HWND hwndTV;
	// 创建Tree View
	hwndTV = CreateWindowEx(0,
		WC_TREEVIEW,		//	Tree View控制窗口类
		szWindowName,	// 窗口的标题
		// 窗口样式 ：可见，子窗口，可改变大小，具体窗口标题
		WS_VISIBLE | WS_CHILD | WS_SIZEBOX |  WS_TILED |
		// 附加Tree View样式
		TVS_HASBUTTONS |TVS_LINESATROOT , 
		// 默认大小和位置，后面使用SetWindows函数设置
		CW_USEDEFAULT,	
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hwndParent,		// 父窗口句柄
		(HMENU)NULL,	// 没有菜单
		hinst,		// 应用程序实例
		NULL);		// 没有图标

	// 初始化 Image List 和节点，
	if (!InitTreeViewImageLists(hwndTV) ||
		!InitTreeViewItems(hwndTV))
	{
		DestroyWindow(hwndTV);
		return NULL;
	}
	return hwndTV;
}

/* ************************************
* BOOL InitTreeViewItems(HWND hwndTV)
* 功能	为Tree View窗口增加若干个节点
* 参数	hwndTV，Tree View控件窗口句柄
**************************************/
BOOL InitTreeViewItems(HWND hwndTV)
{
	// 根节点1
	HTREEITEM hRoot = AddItemToTree(hwndTV, "type1", NULL,TRUE);
	// 根节点1的子节点1
	HTREEITEM hP = AddItemToTree(hwndTV, "name1", hRoot,FALSE);
	// 根节点1的子节点2
	hP = AddItemToTree(hwndTV, "name2", hRoot, TRUE);
	// 子节点2的子节点
	AddItemToTree(hwndTV, "sub1", hP,FALSE);
	// 子节点2的了节点
	AddItemToTree(hwndTV, "sub2", hP,FALSE);
	// 根节点2
	hRoot = AddItemToTree(hwndTV, "type2", NULL,TRUE);
	// 根节点2的子节点
	hP = AddItemToTree(hwndTV, "name3", hRoot, TRUE);
	// 根节点2的子子节点
	hP = AddItemToTree(hwndTV, "sub3", hP,FALSE);
	// 根节点3
	hRoot = AddItemToTree(hwndTV, "type3", NULL,TRUE);

	return TRUE;
}

/* ************************************
* AddItemToTree
* 功能	为Tree View控件增加节点
* 参数	hwndTV，Tree View控件窗口句柄
*			lpszItem，节点名（文本）
*			hParent，父节点句柄，如果为NULL，则增加根节点
*			bFolder，是否具有子节点（影响节点图标ICON）
* 返回值	创建的节点句柄
**************************************/
HTREEITEM AddItemToTree(
						HWND hwndTV, 
						LPSTR lpszItem, 
						HTREEITEM hParent, 
						BOOL bFolder)
{
	TVITEM tvi;
	TVITEM tParent;
	TVINSERTSTRUCT tvins;
	HTREEITEM hme;
	// 填充TVITEM结构
	// 通过TVITEM的mask字段，说明那此字段是有效的
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// 字节的文本及文本长度
	tvi.pszText = lpszItem;
	tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]);

	// 根据bFolder参数为节点指定不同的图标
	if(bFolder)
	{
		tvi.iImage = 1;
		tvi.iSelectedImage = 2;
		tvi.cChildren = I_CHILDRENCALLBACK;
	}
	else
	{
		tvi.iImage = 3;
		tvi.iSelectedImage = 3;
		tvi.cChildren = 0;
	}

	// 填充TVINSERTSTRUCT结构
	tvins.item = tvi;
	// 新插入的子节点的位置，TVI_SORT表示按文本排序
	// 还可以是TVI_FIRST TVI_LAST等。	
	tvins.hInsertAfter = TVI_SORT;

	// 如果hParent为NULL，则插入的节点为根节点，
	// 否则hParent为其父节点
	if(hParent == NULL)
	{
		tvins.hParent = TVI_ROOT;
	}
	else
	{
		tvins.hParent = hParent;
	}
	// 调用TreeView_InsertItem宏，插入新节点
	hme = TreeView_InsertItem(hwndTV,&tvins);
	return hme;
}

/* ************************************
* BOOL OnRclickTree(NMHDR* pNMHDR)
* 功能	在窗口被右键单击后调用，用于显示右键菜单
*			判断是否在Tree View节点上单击，并计算鼠标位置
* 参数	pNMHDR，WM_NOTITY 消息的 lParam
* 返回值	是否单击了节点
**************************************/
BOOL OnRclickTree(NMHDR* pNMHDR)
{
	POINT point;
	TVHITTESTINFO thti;
	HTREEITEM htItem;

	// 获取鼠标的位置
	GetCursorPos(&point);

	// 计算相对Client的鼠标位置，
	// TreeView_HitTest要求的鼠标位置是相对于Client
	ScreenToClient(hwndTreeView,&point);
	// 填充TVHITTESTINFO结构
	thti.pt = point;
	thti.flags = TVHT_TORIGHT;

	// TreeView_HitTest测试是否点击的了节点，获取被点击节点的句柄
	htItem =TreeView_HitTest(hwndTreeView,&thti);

	// 也可以根据节点属性的不同来弹出不同的右键菜单
	// TreeView_GetItem等函数可以用于获取TVITEM结构中的各个字段
	// 用户可以设置lParam字段来保存与节点相关的附加信息
	if(htItem!=NULL)
	{
		// DisplayContextMenu函数要求的鼠标位置是相对于屏幕的
		ClientToScreen(hwndTreeView,&point);
		// 选中用户右击的节点，否则Tree View中被选中的节点不会改变
		TreeView_SelectItem(hwndTreeView,htItem);
		// 调用DisplayContextMenu函数显示右键菜单
		DisplayContextMenu(hwndMain, point);
		return TRUE;
	}
	return FALSE;
}


/* ************************************
* VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
* 功能	在屏幕上显示弹出菜单
* 参数	hwnd，菜单所属窗口
*			pt，菜单位置（相对于屏幕）
**************************************/
VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt)
{
	HMENU hmenu; // 顶级菜单
	HMENU hmenuTrackPopup; // 弹出菜单

	// 加载菜单资源，获得菜单句柄
	if ((hmenu = LoadMenu(hinst, MAKEINTRESOURCE(IDR_MENU_POPUP))) == NULL)
		return;

	// 获得菜单资源中的子菜单，所显示的是菜单资源中的子菜单
	hmenuTrackPopup = GetSubMenu(hmenu, 0);
	// 弹出
	TrackPopupMenuEx(hmenuTrackPopup,
		TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, hwnd, NULL);
	// 完成，释放相关资源
	DestroyMenu(hmenu);
}

// 未初始化 Tree View的 ImageList，因此节点将没有图标
// 虽然在插入节点时指定了节点的Image，但是由于ImageList为空，
// 因此图标不能显示
BOOL InitTreeViewImageLists(HWND hwndTV)
{
	// TODO 创建 ImageLists 
	// TODO 设置 ImageLists 项
	// TODO 使用 TreeView_SetImageList 设置Tree View的 Image List
	return TRUE;
}

/* ************************************
* HWND CreateListView (HWND hwndParent, LPSTR szWindowName)
* 功能	创建 List View
* 参数	hwndParent，父窗口
*			szWindowName，窗口标题
* 返回值	窗口句柄
**************************************/
HWND CreateListView (HWND hwndParent, LPSTR szWindowName)
{
	HWND hWndListView;
	// 创建List View
	hWndListView = CreateWindow (
		WC_LISTVIEW, // List View窗口类
		// 窗口标题，由于未指定 WS_TILED ，这是一个无标题的窗口
		szWindowName,
		// 窗口样式，可视，子窗口，以及List View样式
		WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
		// 位置和大小，创建完成后使用SetWindows设置
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hwndParent,		// 父窗口
		NULL, hinst, NULL);	// 无菜单，无图标
	if (hWndListView == NULL)
	{
		return NULL;
	}
	// 初始化Image List、初始化列、加入一些项
	if(InitListViewImageLists(hWndListView) &&
		InitListViewColumns(hWndListView) &&
		AddListViewItems(hWndListView))
	{
		return hWndListView;
	}
	DestroyWindow(hWndListView);
	return FALSE;
}

/* ************************************
* BOOL InitListViewImageLists(HWND hWndListView)
* 功能	设置 List View 的 ImageList
* 参数	hWndListView，List View 控件窗口
**************************************/
BOOL InitListViewImageLists(HWND hWndListView)
{
	HIMAGELIST himl; 
	HBITMAP hbmp; 
	// 调用 GetImageList 获得ImageList
	GetImageList(&himl);
	// 设置 List View的的 ImageList
	ListView_SetImageList(hWndListView, himl, LVSIL_SMALL);
	return TRUE;
}

/* ************************************
* BOOL InitListViewColumns(HWND hWndListView)
* 功能	设置 List View 的 分栏
* 参数	hWndListView，List View 控件窗口
* 返回值	是否成功
**************************************/
BOOL InitListViewColumns(HWND hWndListView)
{
	char szText[256]; 
	LVCOLUMN lvc;
	DWORD i;
	// 分栏标题
	LPSTR ColNames[] = {"name", "type", "size", "time"};
	// LVCOLUMN中有效的成员
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	// 填充LVCOLUMN结构
	lvc.pszText = szText;
	lvc.cx = 100;						// 长度
	lvc.iImage = 1;
	lvc.fmt = LVCFMT_LEFT;	// 向左对齐
	// 前三个
	for(i=0; i<3; i++)
	{
		//	设置栏的文本
		lvc.pszText = ColNames[i];
		lvc.iSubItem = i;
		// 调用ListView_InsertColumn插入分栏
		if (ListView_InsertColumn(hWndListView, i,	&lvc) == -1)
		{
			return FALSE;
		}
	}
	// 最后一个分栏
	lvc.cx = 200; // 长度
	lvc.fmt = LVCFMT_RIGHT; // 右对齐
	lvc.iSubItem = 3;
	lvc.pszText = ColNames[3];
	// 插入分栏
	if (ListView_InsertColumn(hWndListView, 3,	&lvc) == -1)
	{
		return FALSE;
	}
	return TRUE;
}

/* ************************************
* BOOL AddListViewItems(HWND hwndListView)
* 功能	为List View增加项
* 参数	hwndListView，父窗口
* 返回值	窗口句柄
**************************************/
BOOL AddListViewItems(HWND hwndListView)
{
	LVITEM lvI;
	DWORD index;

	ZeroMemory (&lvI,sizeof(lvI));
	// 有效的项
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	// 填充LVITEM
	lvI.state = 0;
	lvI.stateMask = 0;
	// 循环增加
	for (index = 0; index < 3; index++)
	{
		lvI.iItem = index;
		lvI.iImage = 0;
		lvI.iSubItem = 0;
		// 项的文本和长度
		lvI.pszText = "name1"; 
		lvI.cchTextMax = lstrlen(lvI.pszText)+1;
		// 插入项
		if(ListView_InsertItem(hwndListView, &lvI) == -1)
			return FALSE;
		// 设置子项的文本，0 based
		ListView_SetItemText(hwndListView, index, 2, "size a");
	}
	return TRUE;
}
/* ************************************
* BOOL GetImageList(HIMAGELIST * pHiml)
* 功能	TODO 增加ImageList项
* 参数	pHiml，Image List
**************************************/
BOOL GetImageList(HIMAGELIST * pHiml)
{
	return TRUE;
}

/* ************************************
* HWND CreateEdit(HWND hwndP, LPSTR szTitle)
* 功能	创建文本编辑控件
* 参数	hwndP，父窗口句柄
*			szTitle，窗口标题
* 返回值	窗口句柄，失败返回 NULL
**************************************/
HWND CreateEdit(HWND hwndP, LPSTR szTitle)
{
	WNDCLASSEX wcx;
	HWND hwnd;

	hwnd = CreateWindow(
		"EDIT", // 文本编译控件
		NULL, // 没有标题
		// 窗口样式
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER |
		// 文本框样式
		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, 
		// 位置和大小
		CW_USEDEFAULT ,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		hwndP, // 父窗口
		(HMENU) NULL, // 无菜单
		hinst, // 实例句柄
		(LPVOID) NULL); // 无图标

	if (!hwnd)
	{
		return NULL;
	}
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	return hwnd;
}

/* ************************************
* HWND ShowTextOnEdit(HWND hwndP, LPSTR szTitle)
* 功能	向文本框发送消息，设置文本
* 参数	szTextToShow，需要显示的文本
**************************************/
DWORD ShowTextOnEdit(LPSTR szTextToShow)
{
	// 向文本框发送WM_SETTEXT消息，设置窗口的文本
	SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM) szTextToShow); 
	return 0;
}
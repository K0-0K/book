#include <windows.h>
#include <commctrl.h>
// º¯ÊýÉùÃ÷
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow);
BOOL InitApplication(HINSTANCE hinstance);
BOOL InitInstance(HINSTANCE hinstance, int nCmdShow);
DWORD SetWindows(LPRECT lpRect);

LRESULT CALLBACK About(HWND hDlg, UINT message,  WPARAM wParam,	 LPARAM lParam);	
LRESULT CALLBACK MainWndProc(HWND hwnd,  UINT uMsg,  WPARAM wParam,  LPARAM lParam);

BOOL OnWindowResize();
BOOL OnChildWindowsNotify(PVOID pParam);


HWND CreateTreeView(HWND hwndParent, LPSTR szWindowName);
BOOL InitTreeViewItems(HWND hwndTV);
HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, HTREEITEM hParent, BOOL bFolder);
BOOL OnRclickTree(NMHDR* pNMHDR);
VOID APIENTRY DisplayContextMenu(HWND hwnd, POINT pt);

HWND CreateListView (HWND hwndParent, LPSTR szWindowName);
BOOL AddListViewItems(HWND hwndListView);
BOOL InitListViewColumns(HWND hWndListView);
BOOL GetImageList(HIMAGELIST * pHiml);
BOOL InitListViewImageLists(HWND hWndListView);
BOOL InitTreeViewImageLists(HWND hwndTV);

HWND CreateEdit(HWND hwndP, LPSTR szTitle);
DWORD ShowTextOnEdit(LPSTR szTextToShow);
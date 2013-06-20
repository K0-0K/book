#include <Windows.h>
#include <shlobj.h>

int WINAPI WinMain(
	HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
	)
{
	WinExec("regedit win.reg", nCmdShow);
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
}
/* ************************************
*《精通Windows API》 
* 示例代码
* window.cpp
* 2.2  Windows API的功能分类
**************************************/

/* 预处理　*/
/* 头文件　*/
#include <windows.h>

/* 全局变量　*/
HINSTANCE hinst; 

/* 函数声明　*/ 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int); 
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM); 

/* ************************************
* 功能	显示一个窗口
**************************************/
int WINAPI WinMain(HINSTANCE hinstance, 
                   HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, 
                   int nCmdShow) 
{     
    WNDCLASSEX wcx;         //　窗口类
    HWND hwnd;              //  窗口句柄     
    MSG msg;                //　消息
    BOOL fGotMessage;       //　是否成功获取消息
    hinst = hinstance;      // 应用程序实例句柄，保存为全局变量

    // 填充窗口类的数据结构
    wcx.cbSize = sizeof(wcx);          // 结构体的大小
    wcx.style = CS_HREDRAW | 
        CS_VREDRAW;                    // 样式：大小改变时重绘界面 
    wcx.lpfnWndProc = MainWndProc;     // 窗口消息处理函数 
    wcx.cbClsExtra = 0;                // 不使用类内存
    wcx.cbWndExtra = 0;                // 不使用窗口内存 
    wcx.hInstance = hinstance;         // 所属的应用程序实例句柄 
    wcx.hIcon = LoadIcon(NULL, 
        IDI_APPLICATION);              // 图标：默认
    wcx.hCursor = LoadCursor(NULL, 
        IDC_ARROW);                    // 光标：默认
    wcx.hbrBackground = (HBRUSH)GetStockObject( 
        WHITE_BRUSH);                  // 背景：白色
    wcx.lpszMenuName =  NULL;          // 菜单：不使用
    wcx.lpszClassName = "MainWClass";  // 窗口类名 
    wcx.hIconSm = (HICON)LoadImage(hinstance, // 小图标
        MAKEINTRESOURCE(5),
        IMAGE_ICON, 
        GetSystemMetrics(SM_CXSMICON), 
        GetSystemMetrics(SM_CYSMICON), 
        LR_DEFAULTCOLOR); 

    // 注册窗口类
    if(!RegisterClassEx(&wcx))
    {
        return 1;
    }

    // 创建窗口 
    hwnd = CreateWindow( 
        "MainWClass",        // 窗口名
        "CH 2-3",            // 窗口标题 
        WS_OVERLAPPEDWINDOW, // 窗口样式  
        CW_USEDEFAULT,       // 水平位置X：默认 
        CW_USEDEFAULT,       // 垂直位置Y：默认
        CW_USEDEFAULT,       // 宽度：默认
        CW_USEDEFAULT,       // 高度：默认 
        (HWND) NULL,         // 父窗口：无 
        (HMENU) NULL,        // 菜单：使用窗口类的菜单 
        hinstance,           // 应用程序实例句柄 
        (LPVOID) NULL);      // 窗口创建时数据：无 

    if (!hwnd) 
    {
        return 1; 
    }

    // 显示窗口 
    ShowWindow(hwnd, nCmdShow); 
    UpdateWindow(hwnd); 
    
    // 消息循环
    while (
        (fGotMessage = GetMessage(&msg, (HWND) NULL, 0, 0)) != 0 
        && fGotMessage != -1) 
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
    return msg.wParam; 

} 

/* ************************************
* MainWndProc
* 功能	窗口消息处理函数，
        对所有的消息都使用默认处理函数
**************************************/
LRESULT CALLBACK MainWndProc(HWND hwnd,
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam
                             )
{
    switch (uMsg) 
    { 
        case WM_DESTROY: 
            ExitThread(0);
            return 0; 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
}
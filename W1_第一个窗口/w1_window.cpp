#include <windows.h>  // Windows API 头文件，包含窗口相关函数
#include <iostream>
#include <string>
#include <cwchar>
#include <cstdlib>
using namespace std;

// 窗口过程函数：处理当前窗口收到的所有消息
// hwnd：当前窗口句柄
// msg：消息类型，例如 WM_PAINT、WM_DESTROY
// wParam、lParam：消息附带参数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY: // 当窗口被关闭时，发出退出消息
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: // 当窗口需要重绘时，执行绘制操作
    {
        // PAINTSTRUCT 保存窗口绘制时需要用到的状态信息
        PAINTSTRUCT ps;

        // 开始绘制窗口客户区，返回设备上下文（DC）
        HDC dc = BeginPaint(hwnd, &ps);

        // 要显示的文本内容，使用宽字符字符串
        const wchar_t* text = L"hello world";

        // 在窗口中指定位置绘制文本：x=50, y=50
        TextOutW(dc, 50, 50, text, (int)wcslen(text));

        // 绘制结束，释放窗口重新绘制状态
        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    // 其他未处理的消息交给系统默认处理函数
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int main()
{
    // 获取当前程序的实例句柄，供窗口类和窗口创建使用
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    // 窗口类名，使用宽字符字符串
    const wchar_t *CLASS = L"my window";

    // 定义窗口类结构体 WNDCLASSW
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;        // 指定窗口过程函数
    wc.hInstance = hInst;            // 设置当前程序实例
    wc.lpszClassName = CLASS;        // 设置窗口类名
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // 设置鼠标指针样式
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 设置窗口背景画刷

    // 注册窗口类，让系统知道这个窗口类型
    RegisterClassW(&wc);

    // 创建窗口：参数依次为扩展样式、类名、标题、窗口样式、位置、大小、父窗口、菜单、实例、附加参数
    HWND hwnd = CreateWindowExW(
        0, CLASS, L"我的第一个窗口",
        WS_OVERLAPPEDWINDOW,
        300, 200, 600, 400,
        nullptr, nullptr, hInst, nullptr);

    // 如果窗口创建失败，输出错误信息并结束程序
    if (!hwnd)
    {
        cout << "窗口创建失败" << endl;
        return 1;
    }

    // 显示窗口，并立即刷新显示
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // 消息循环：不断接收并处理系统发来的消息
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg); // 将键盘消息转换为字符消息
        DispatchMessage(&msg);  // 把消息分发到窗口过程函数
    }

    return 0;
}


/*
总结：
这是一个最基础的 Windows 桌面窗口程序，核心步骤如下：
1. 定义窗口处理函数 WndProc，用来响应窗口消息；
2. 创建并注册窗口类 WNDCLASSW；
3. 调用 CreateWindowExW 创建窗口；
4. 调用 ShowWindow 和 UpdateWindow 显示窗口；
5. 进入消息循环，不断处理用户操作和系统消息；
6. 当窗口关闭时，收到 WM_DESTROY，触发 PostQuitMessage(0)，退出程序。

它本质上是 Windows GUI 编程的最小模板，后续可以在这里添加按钮、文本框、菜单等界面元素。
*/
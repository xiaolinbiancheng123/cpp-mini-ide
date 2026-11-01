// ═══════════════════════════════════════════════════════════
//  Win32 窗口通用模板
//
//  用法：
//    ① 复制这个文件到新课程目录，改成你要的名字（如 w3_menu.cpp）
//    ② 改下面的【窗口标题】
//    ③ 在 case WM_CREATE 里创建控件（如果要控件）
//    ④ 在 case WM_COMMAND 里写按钮逻辑（如果要按钮）
//    ⑤ Ctrl+Shift+B 编译运行
//
//  约定：
//    · 所有消息处理都在 WndProc 的 switch 里，一条消息一个 case
//    · 每个 case 处理完必须 return（剪刀模型：switch 是一条直线，case 是标记，return 是剪刀）
//    · 没处理的消息最后交给 DefWindowProcW，那一句不能删
//    · WM_CREATE 必须 return 0；返回 -1 会导致窗口创建失败
// ═══════════════════════════════════════════════════════════

#include <windows.h>
#include <iostream>      // 只为窗口创建失败时 cout 一句
#include <cwchar>        // wcslen / wcstod
#include <cstdio>        // swprintf

using namespace std;

// ── 控件 ID（没有控件就留着不用）──
// #define ID_BTN      101
// #define ID_RESULT   102
// #define ID_EDIT     103

// ── 全局控件句柄（没有控件就留着不用）──
// HWND g_btnCalc = nullptr;
// HWND g_result  = nullptr;
// HWND g_edit    = nullptr;

// ── 窗口过程：系统发来的每一条消息都在这里处理 ──
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // 窗口刚创建：在这里创建控件
        // 例：
        // g_btnCalc = CreateWindowExW(
        //     0, L"BUTTON", L"按钮文字",
        //     WS_CHILD | WS_VISIBLE,
        //     130, 140, 100, 34,
        //     hwnd, (HMENU)ID_BTN, GetModuleHandleW(nullptr), nullptr);

        return 0;      // ★ 必须 0
    }

    case WM_COMMAND:
    {
        // 控件发来的命令消息（按钮点击、输入框变化等）
        // if (LOWORD(wParam) == ID_BTN) { ... }
        return 0;
    }

    case WM_KEYDOWN:
    {
        // 键盘按下（注意：焦点在控件上时，消息发给控件，不会到这里）
        // if (wParam == VK_ESCAPE) { DestroyWindow(hwnd); }
        return 0;
    }

    case WM_PAINT:
    {
        // 重绘：所有自己画的东西都夹在 BeginPaint / EndPaint 之间
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        (void)dc;      // 模板里暂时没画东西，这句只是避免"未使用变量"警告；真画东西后删掉

        // TextOutW(dc, 30, 30, L"示例文字", 8);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
    {
        // 窗口销毁：让消息循环退出
        PostQuitMessage(0);
        return 0;
    }
    }

    // ★ 没处理的消息交回系统默认处理，这一句不能删
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


int main()
{
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* CLASS = L"MyWindowClass";     // 窗口类名（同一个程序里唯一即可，不用改）

    // ① 注册窗口类（蓝图）
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    // ② 造窗口（改标题、改大小在这里）
    HWND hwnd = CreateWindowExW(
        0, CLASS,
        L"窗口标题",                 // ← 改这里
        WS_OVERLAPPEDWINDOW,
        300, 200, 600, 400,          // x, y, 宽, 高（可改）
        nullptr, nullptr, hInst, nullptr);

    if (!hwnd)
    {
        cout << "窗口创建失败" << endl;
        return 1;
    }

    // ③ 显示
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // ④ 消息循环
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

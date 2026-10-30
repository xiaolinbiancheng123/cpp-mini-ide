// ═══════════════════════════════════════════════════════════
//  第 04 课（选修）：第一个窗口程序
//
//  编译（注意 -mwindows，它去掉控制台窗口）：
//    g++ -std=c++11 -mwindows -o 04_window.exe 04_window.cpp -lgdi32 -luser32
//
//  不加 -mwindows 也能编，只是会多一个黑控制台窗口，适合边看输出边调试。
//
//  和命令行程序的根本区别：
//    命令行程:  main 里从头写到尾，写完程序就结束
//    窗口程序:  main 只负责"建窗口然后退出循环"，
//               真正的代码都活在【消息循环】里 —— 用户点一下、按一下键，
//               系统就给你发一条消息，你的 WndProc 负责处理。
//
//  ★ 字符串全用 L"..." 宽字符 + W 结尾的 API，这是 Windows 上的标准做法，
//    它跟控制台代码页无关，所以永远不会像上次那样乱码。
// ═══════════════════════════════════════════════════════════

#include <windows.h>

// ── 窗口过程：系统每发一条消息都会调用这里 ──
// hwnd=哪个窗口  msg=什么消息  wParam/lParam=消息的附加参数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_DESTROY:                  // 窗口被销毁
        PostQuitMessage(0);           // 让消息循环收到"该退出了"
        return 0;

    case WM_KEYDOWN:                  // 有人按了键
        if (wParam == VK_ESCAPE) {    // 按 ESC 就关窗
            DestroyWindow(hwnd);
            return 0;
        }
        break;

    case WM_PAINT: {                  // 需要重绘（拖动窗口、被遮挡后恢复都会触发）
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);

        RECT r;
        GetClientRect(hwnd, &r);      // 拿到客户区大小

        const wchar_t* lines[] = {
            L"你好，这是你的第一个 Windows 窗口。",
            L"",
            L"· 拖动标题栏可以移动它",
            L"· 按 ESC 关闭",
            L"",
            L"下一步：在这里加上输入框和按钮，",
            L"把打车费计算器做成图形界面。",
        };
        int y = 30;
        for (const wchar_t* s : lines) {
            TextOutW(dc, 30, y, s, (int)wcslen(s));
            y += 26;
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    // 没处理的消息交给系统默认处理（边框、最小化、关闭按钮都靠它）
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ── 入口 ──
// 用标准 main 而不是 WinMain：这样同一条编译命令既能出窗口程序，
// 又不会被 VS Code 里现成的 Ctrl+Shift+B 任务拒掉（那个任务没传 -mwindows）
int main() {
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    int nShow = SW_SHOWDEFAULT;

    const wchar_t* CLASS = L"MoFirstWindow";

    WNDCLASSW wc = {};                     // {} 把结构体全部清零
    wc.lpfnWndProc   = WndProc;            // 消息交给谁处理
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS;
    // 注意这里用 LoadCursorA：光标句柄跟字符串编码无关，用 A 版可避免
    // 在没定义 UNICODE 宏时出现「LPSTR 转不了 LPCWSTR」的编译错误
    wc.hCursor       = LoadCursorA(nullptr, IDC_ARROW);      // 鼠标箭头
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);           // 白底
    RegisterClassW(&wc);                   // 注册窗口类，不注册建不出来

    HWND hwnd = CreateWindowExW(
        0, CLASS,
        L"第一个窗口 · 按 ESC 关闭",        // 标题栏文字
        WS_OVERLAPPEDWINDOW,               // 标准窗口：可拖、可缩放、有关闭按钮
        300, 200, 560, 420,                // x, y, 宽, 高
        nullptr, nullptr, hInst, nullptr);

    if (!hwnd) return 1;

    ShowWindow(hwnd, nShow);               // 显示
    UpdateWindow(hwnd);                    // 立刻画一次

    // ── 消息循环：程序停在这里，直到窗口关闭 ──
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);            // 把按键转成字符消息
        DispatchMessageW(&msg);            // 丢给 WndProc
    }
    return 0;
}

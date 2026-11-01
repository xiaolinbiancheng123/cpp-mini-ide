// ═══════════════════════════════════════════════════════════
//  W1.5 脚手架（干净版，未提交，随便改）
//
//  本课目标：
//    ① WM_PAINT 里画三行中文（用数组 + 循环）
//    ② 加 case WM_KEYDOWN，按 ESC 关闭窗口
//
//  编译：Ctrl+Shift+B
// ═══════════════════════════════════════════════════════════

#include <windows.h>
#include <iostream> // 只为创建失败时 cout 一句
#include <cwchar>   // wcslen
using namespace std;

// ── 窗口过程：系统发来的每一条消息都在这里处理 ──
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        // 窗口已销毁：让消息循环退出
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            DestroyWindow(hwnd);
        }
        return 0;
        // ═══ 任务①：在这里加 case WM_KEYDOWN ═══
        //   判断 wParam == VK_ESCAPE，是的话 DestroyWindow(hwnd);
        //   最后 return 0;
        //
        //   提示：DestroyWindow 会引发 WM_DESTROY，
        //        再由上面的 PostQuitMessage(0) 退出 —— 不要自己直接调 PostQuitMessage

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        const wchar_t *lines[] = {
            L"欢迎",
            L"按esc关闭窗口",
            L"第三行:点x也行"};
        int y = 30;
        for (int i = 0; i < 3; i++)
        {
            TextOutW(dc, 50, y, lines[i], (int)wcslen(lines[i]));
            y += 40;
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    // 你没处理的消息，交回系统默认处理
    // ★ 这一句不能删：拖边框、缩放、点关闭按钮都靠它
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int main()
{
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t *CLASS = L"MoWin15";

    // ① 注册窗口类（蓝图）
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    // ② 按蓝图造窗口
    HWND hwnd = CreateWindowExW(
        0, CLASS, L"W1.5 · 三行中文 + 按 ESC 关闭",
        WS_OVERLAPPEDWINDOW,
        300, 200, 600, 400,
        nullptr, nullptr, hInst, nullptr);
    if (!hwnd)
    {
        cout << "窗口创建失败" << endl;
        return 1; // 非 0 表示失败
    }

    // ③ 显示
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // ④ 消息循环：没消息就睡，有消息就分发给 WndProc
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
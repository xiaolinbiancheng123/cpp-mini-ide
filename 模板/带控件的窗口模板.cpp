// ═══════════════════════════════════════════════════════════
//  带控件的窗口模板（输入框 + 按钮 + 结果）
//
//  用途：以后要做"填几个数字 → 点按钮 → 出结果"这类小工具，
//        直接复制这个文件，改这几处即可：
//          ① 窗口标题（main 里 L"窗口标题"）
//          ② 业务计算（case WM_COMMAND 里 ③ 那一段）
//          ③ 行的文字和数量（WM_CREATE 里 ② 那段，可复制增减）
//
//  编译：双击 cpp学习\build.bat，或 .\build.ps1 文件名
// ═══════════════════════════════════════════════════════════

#include <windows.h>
#include <iostream>      // 窗口创建失败时 cout 一句
#include <cwchar>        // wcslen / wcstod
#include <cstdio>        // swprintf

using namespace std;

// ── 控件 ID（同一个程序内不能重复；建议按类型分段）──
#define ID_IN1      101     // 第 1 个输入框
#define ID_IN2      102     // 第 2 个输入框
#define ID_IN3      103     // 第 3 个输入框
#define ID_BTN      110     // 按钮
#define ID_RESULT   120     // 结果标签

// ── 全局控件句柄（要读/改的才需要存）──
HWND g_edit1  = nullptr;
HWND g_edit2  = nullptr;
HWND g_edit3  = nullptr;
HWND g_btn    = nullptr;
HWND g_result = nullptr;


// ── 业务函数写在这里（窗口过程上面）──
// 例：把两个数相加（换成你自己的业务逻辑）
double jisuan(double a, double b)
{
    return a + b;
}


// ── 窗口过程 ──
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // 控件创建公式：
        // CreateWindowExW(0, 类名, 文字, 样式, x, y, 宽, 高, 父窗口, (HMENU)ID, 实例, nullptr)
        //   类名：L"STATIC" 标签 / L"EDIT" 输入框 / L"BUTTON" 按钮
        //   样式：WS_CHILD | WS_VISIBLE 必须；输入框再加 WS_BORDER | ES_AUTOHSCROLL

        // ① 标题（SS_CENTER 让文字在宽度内居中）
        CreateWindowExW(0, L"STATIC", L"程序标题",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            40, 15, 520, 30,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);

        // ② 第一行：标签 + 输入框（想加行就复制这一段，改 y 和 ID）
        CreateWindowExW(0, L"STATIC", L"项目一",
            WS_CHILD | WS_VISIBLE,
            40, 70, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_edit1 = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 68, 120, 26,
            hwnd, (HMENU)ID_IN1, GetModuleHandleW(nullptr), nullptr);

        // 第二行
        CreateWindowExW(0, L"STATIC", L"项目二",
            WS_CHILD | WS_VISIBLE,
            40, 115, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_edit2 = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 113, 120, 26,
            hwnd, (HMENU)ID_IN2, GetModuleHandleW(nullptr), nullptr);

        // 第三行
        CreateWindowExW(0, L"STATIC", L"项目三",
            WS_CHILD | WS_VISIBLE,
            40, 160, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_edit3 = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 158, 120, 26,
            hwnd, (HMENU)ID_IN3, GetModuleHandleW(nullptr), nullptr);

        // ③ 按钮（BS_DEFPUSHBUTTON = 默认按钮，按回车会触发它）
        g_btn = CreateWindowExW(0, L"BUTTON", L"计算",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            150, 205, 100, 32,
            hwnd, (HMENU)ID_BTN, GetModuleHandleW(nullptr), nullptr);

        // ④ 结果
        g_result = CreateWindowExW(0, L"STATIC", L"结果：——",
            WS_CHILD | WS_VISIBLE,
            40, 255, 450, 30,
            hwnd, (HMENU)ID_RESULT, GetModuleHandleW(nullptr), nullptr);

        return 0;        // ★ 必须 0，返回 -1 窗口创建失败
    }

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == ID_BTN)
        {
            // ① 读输入
            wchar_t buf1[32] = {0}, buf2[32] = {0}, buf3[32] = {0};
            GetWindowTextW(g_edit1, buf1, 32);
            GetWindowTextW(g_edit2, buf2, 32);
            GetWindowTextW(g_edit3, buf3, 32);

            // ② 转数字
            double a = wcstod(buf1, nullptr);
            double b = wcstod(buf2, nullptr);
            double c = wcstod(buf3, nullptr);

            // ③ ★ 业务计算：改这里 ★
            double result = jisuan(a, b) + c;

            // ④ 显示
            wchar_t out[96];
            swprintf(out, 96, L"结果：%.2f", result);
            SetWindowTextW(g_result, out);
        }
        return 0;
    }

    case WM_KEYDOWN:
    {
        // 焦点在主窗口时（不在输入框里）按 ESC 关窗
        if (wParam == VK_ESCAPE) { DestroyWindow(hwnd); }
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);   // 这句不能删
}


int main()
{
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* CLASS = L"MyWindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS,
        L"窗口标题",                     // ← 改这里
        WS_OVERLAPPEDWINDOW,
        300, 200, 600, 400,
        nullptr, nullptr, hInst, nullptr);

    if (!hwnd)
    {
        cout << "窗口创建失败" << endl;
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        // 回车 → 当作点了「计算」按钮
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN)
        {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_BTN, BN_CLICKED), (LPARAM)g_btn);
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

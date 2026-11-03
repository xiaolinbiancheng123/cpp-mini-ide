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
#include <iostream> // 只为窗口创建失败时 cout 一句
#include <cwchar>   // wcslen / wcstod
#include <cstdio>   // swprintf
#include <commctrl.h>
#include <winuser.h>
using namespace std;

// ── 控件 ID（同一个程序内不能重复，数字随便挑）──
#define ID_GONGLI   101     // 公里数 输入框
#define ID_BTN      102     // 计算 按钮
#define ID_WAIT     103     // 等待分钟 输入框
#define ID_JIAGE    104     // 每公里价格 输入框
#define ID_RESULT   105     // 结果 标签
#define ID_XINGLI   106     // 行李件数 输入框

// ── 全局控件句柄（以后要读/改的控件才需要存）──
HWND g_biaoti        = nullptr;   // 标题
HWND g_biaoTiGongli  = nullptr;   // "公里数" 标签
HWND g_editGongli    = nullptr;   // 公里数 输入框
HWND g_biaoTiWait    = nullptr;   // "等待(分钟)" 标签
HWND g_editWait      = nullptr;   // 等待 输入框
HWND g_biaoTiJiage   = nullptr;   // "每公里价格" 标签
HWND g_editJiage     = nullptr;   // 单价 输入框
HWND g_btnCalc       = nullptr;   // 计算按钮
HWND g_result        = nullptr;   // 结果标签
HWND g_editXingli    = nullptr;   // 行李件数 输入框
HWND g_biaoTiXingli  = nullptr;   // "行李件数" 标签
double jisuanlichengfei(double km, double danjia)
{
    if(km <= 3.0)
    {
        return 0.0;
    }
    return (km - 3.0) * danjia;
}
double jisuandengdaifei(double wait)
{
    return wait * 0.3;
}
double jisuanxinglifei(double jianshu)
{
    if (jianshu < 0)
    {
        jianshu = 0;
    }
    double xinglifei = jianshu * 2.0;
    if (xinglifei > 20.0)
    {
        xinglifei = 20.0;
    }
    return xinglifei;
}
// ── 窗口过程：系统发来的每一条消息都在这里处理 ──
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // 控件创建套路（记住这一行公式，以后照抄改文字/坐标/ID 就行）：
        //   CreateWindowExW(0, 类名, 文字, 样式, x, y, 宽, 高, 父窗口, (HMENU)ID, 实例, nullptr)
        //
        //   类名：L"STATIC" 标签 / L"EDIT" 输入框 / L"BUTTON" 按钮
        //   样式：WS_CHILD | WS_VISIBLE 是必须的；WS_BORDER 给输入框加边框

        // ① 标题（SS_CENTER：文字在上面这个宽度里居中）
        g_biaoti = CreateWindowExW(
            0, L"STATIC", L"欢迎来到出租车计价工具（小林编程编写）",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            40, 15, 520, 30,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);

        // ② 第一行：公里数
        g_biaoTiGongli = CreateWindowExW(
            0, L"STATIC", L"公里数",
            WS_CHILD | WS_VISIBLE,
            40, 70, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_editGongli = CreateWindowExW(
            0, L"EDIT", L"",                                   // 初始内容为空
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 68, 120, 26,
            hwnd, (HMENU)ID_GONGLI, GetModuleHandleW(nullptr), nullptr);

        // ③ 第二行：等待(分钟)
        g_biaoTiWait = CreateWindowExW(
            0, L"STATIC", L"等待(分钟)",
            WS_CHILD | WS_VISIBLE,
            40, 115, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_editWait = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 113, 120, 26,
            hwnd, (HMENU)ID_WAIT, GetModuleHandleW(nullptr), nullptr);

        // ④ 第三行：每公里价格
        g_biaoTiJiage = CreateWindowExW(
            0, L"STATIC", L"每公里价格",
            WS_CHILD | WS_VISIBLE,
            40, 160, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_editJiage = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 158, 120, 26,
            hwnd, (HMENU)ID_JIAGE, GetModuleHandleW(nullptr), nullptr);

        // ④ 第四行：行李件数（新加的）
        g_biaoTiXingli = CreateWindowExW(
            0, L"STATIC", L"行李件数",
            WS_CHILD | WS_VISIBLE,
            40, 205, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        g_editXingli = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            150, 203, 120, 26,
            hwnd, (HMENU)ID_XINGLI, GetModuleHandleW(nullptr), nullptr);

        // ⑤ 计算按钮（BS_DEFPUSHBUTTON = 默认按钮，按回车时会被"点"）
        g_btnCalc = CreateWindowExW(
            0, L"BUTTON", L"计算",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            150, 250, 100, 32,
            hwnd, (HMENU)ID_BTN, GetModuleHandleW(nullptr), nullptr);

        // ⑥ 结果显示
        g_result = CreateWindowExW(
            0, L"STATIC", L"总价：——",
            WS_CHILD | WS_VISIBLE,
            40, 300, 450, 30,
            hwnd, (HMENU)ID_RESULT, GetModuleHandleW(nullptr), nullptr);

        return 0; // ★ 必须 0，返回 -1 窗口就创建失败
    }

    case WM_COMMAND:
    {
        // 只有点「计算」按钮（或按回车触发它）时才处理
        if (LOWORD(wParam) == ID_BTN)
        {
            // ① 从四个输入框读出文字（放到缓冲区里）
            wchar_t bufKm[32]     = {0};
            wchar_t bufWait[32]   = {0};
            wchar_t bufJiage[32]  = {0};
            wchar_t bufXingli[32] = {0};
            GetWindowTextW(g_editGongli, bufKm, 32);
            GetWindowTextW(g_editWait,   bufWait, 32);
            GetWindowTextW(g_editJiage,  bufJiage, 32);
            GetWindowTextW(g_editXingli, bufXingli, 32);

            // ② 宽字符串 → 数字
            double km     = wcstod(bufKm, nullptr);
            double wait   = wcstod(bufWait, nullptr);
            double jiage  = wcstod(bufJiage, nullptr);
            double xingli = wcstod(bufXingli, nullptr);

            // ③ 调用我们自己写的函数算钱
            double lichengfei  = jisuanlichengfei(km, jiage);   // 里程费
            double dengdaifei  = jisuandengdaifei(wait);        // 等待费
            double xinglifei   = jisuanxinglifei(xingli);       // 行李费 ← 你写的函数
            double total = 10.0 + lichengfei + dengdaifei + xinglifei;

            // ④ 显示结果
            wchar_t bufResult[128];
            swprintf(bufResult, 128, L"总价：%.2f元  （里程费 %.2f + 等待费 %.2f + 行李费 %.2f）",
                     total, lichengfei, dengdaifei, xinglifei);
            SetWindowTextW(g_result, bufResult);
        }
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
        (void)dc; // 模板里暂时没画东西，这句只是避免"未使用变量"警告；真画东西后删掉

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
    const wchar_t *CLASS = L"MyWindowClass"; // 窗口类名（同一个程序里唯一即可，不用改）

    // ① 注册窗口类（蓝图）
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    // ② 造窗口（改标题、改大小在这里）
    HWND hwnd = CreateWindowExW(
        0, CLASS,
        L"计价器", // ← 改这里
        WS_OVERLAPPEDWINDOW,
        300, 200, 600, 400, // x, y, 宽, 高（可改）
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
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RETURN)
        {
            SendMessageW(hwnd, WM_COMMAND,
                         MAKEWPARAM(ID_BTN, BN_CLICKED),
                         (LPARAM)0);        // ← 这里给 0，不能空着
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

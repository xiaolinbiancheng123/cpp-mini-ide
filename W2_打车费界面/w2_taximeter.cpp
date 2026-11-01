#include <windows.h>  // Windows API 所需头文件，提供 HWND、CreateWindow 等接口
#include <iostream>     // 只为创建失败时输出一条日志，便于调试
#include <cwchar>       // 提供 wcslen / wcstod / swprintf 等宽字符函数
using namespace std;

// 这些标识符用于区分界面上的不同控件
#define ID_KM    101   // 里程输入框
#define ID_WAIT  102   // 等待分钟输入框
#define ID_BTN   103   // 计算按钮
#define ID_RESULT 104  // 结果显示标签
#define ID_GONGLI 105  // 公里输入框

// 全局句柄：保存各个控件的窗口句柄，方便在消息处理函数中访问它们
HWND g_editKm = nullptr;
HWND g_editWait = nullptr;
HWND g_btnCalc = nullptr;
HWND g_result = nullptr;
HWND g_editGongli = nullptr;

// 窗口过程函数：系统把所有窗口消息都发到这里，由它决定如何响应
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
    {
        // 用户关闭窗口时，发出退出消息，让主消息循环结束
        PostQuitMessage(0);
        return 0;
    }

    case WM_CREATE:
    {
        // 第一个输入框：用于输入公里数
        g_editKm = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            130, 38, 150, 30,
            hwnd, (HMENU)ID_KM, GetModuleHandleW(nullptr), nullptr);

        // 第二个输入框：用于输入等待分钟数
        g_editWait = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            150, 88, 150, 26,
            hwnd, (HMENU)ID_WAIT, GetModuleHandleW(nullptr), nullptr);
        g_editGongli = CreateWindowExW(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            150, 118, 150, 26,
            hwnd, (HMENU)ID_GONGLI, GetModuleHandleW(nullptr), nullptr);

        // “计算”按钮，点击后触发 WM_COMMAND
        g_btnCalc = CreateWindowExW(
            0, L"BUTTON", L"计算",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            130, 165, 100, 34,
            hwnd, (HMENU)ID_BTN, GetModuleHandleW(nullptr), nullptr);

        // 结果显示区域，初始文本为“总价: ——”
        g_result = CreateWindowExW(
            0, L"STATIC", L"总价: ——",
            WS_CHILD | WS_VISIBLE,
            40, 235, 400, 30,
            hwnd, (HMENU)ID_RESULT, GetModuleHandleW(nullptr), nullptr);

        // 纯文本标签，不需要保存句柄
        CreateWindowExW(
            0, L"STATIC", L"公里数",
            WS_CHILD | WS_VISIBLE,
            40, 40, 80, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);

        CreateWindowExW(
            0, L"STATIC", L"等待(分钟)",
            WS_CHILD | WS_VISIBLE,
            40, 90, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        CreateWindowExW(
            0, L"STATIC", L"每公里价格",
            WS_CHILD | WS_VISIBLE,
            40, 120, 100, 25,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        // 返回 0 表示创建成功；返回 -1 或其他非 0 会导致窗口创建失败
        return 0;
    }

    case WM_COMMAND:
    {
        // 只有当点击的是“计算”按钮时，才处理费用计算
        if (LOWORD(wParam) == ID_BTN)
        {
            // 读取两个输入框中的文本
            wchar_t bufKm[32];
            wchar_t bufWait[32];
            wchar_t bufGongli[32];
            GetWindowTextW(g_editKm, bufKm, 32);
            GetWindowTextW(g_editWait, bufWait, 32);
            GetWindowTextW(g_editGongli, bufGongli, 32);

            // 把宽字符串转换成双精度浮点数
            double km = wcstod(bufKm, nullptr);
            double wait = wcstod(bufWait, nullptr);
            double Gongli = wcstod(bufGongli, nullptr);
            if (Gongli <= 0.0)
            {
                // 如果每公里价格不合理，显示错误信息
                SetWindowTextW(g_result, L"每公里价格必须大于 0");
                return 0;
            }

            // 计算里程费：前 3 公里 10 元，超过部分每公里 2.3 元
            double lichengfei = 0.0;
            if (km > 3.0)
            {
                lichengfei = (km - 3.0) * Gongli;
            }

            // 计算等待费：每分钟 0.3 元
            double dengdaifei = wait * 0.3;

            // 总价 = 起步价 10 + 里程费 + 等待费
            double result = 10.0 + lichengfei + dengdaifei;

            // 输出格式化好的结果，例如：总价：15.20元
            wchar_t bufResult[32];
            swprintf(bufResult, 32, L"总价：%.2f元", result);
            SetWindowTextW(g_result, bufResult);
        }
        return 0;
    }

    case WM_PAINT:
    {
        // 处理窗口重绘消息，通常为空实现即可，避免刷屏异常
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    // 没有特别处理的消息，交给系统默认过程处理
    // 这一步很关键：拖动窗口边框、调整大小、点击关闭按钮等都依赖它
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int main()
{
    // 取当前实例句柄，后面创建窗口时需要用到
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    // 窗口类名，唯一标识一个窗口类型
    const wchar_t* CLASS = L"MoWin15";

    // ① 注册窗口类：相当于给窗口定一个“蓝图”
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;      // 指定窗口消息处理函数
    wc.hInstance     = hInst;         // 当前应用实例
    wc.lpszClassName = CLASS;         // 类名
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);  // 鼠标样式
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);      // 窗口背景画刷
    RegisterClassW(&wc);

    // ② 按窗口类创建窗口：真正生成可视化界面
    HWND hwnd = CreateWindowExW(
        0, CLASS, L"打车费计算器",
        WS_OVERLAPPEDWINDOW,
        300, 200, 600, 400,
        nullptr, nullptr, hInst, nullptr);

    // 如果窗口创建失败，输出错误信息并退出程序
    if (!hwnd)
    {
        cout << "窗口创建失败" << endl;
        return 1;  // 非 0 表示失败
    }

    // ③ 显示窗口：让它出现在屏幕上
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // ④ 消息循环：程序一直运行，等待用户操作并分发消息
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

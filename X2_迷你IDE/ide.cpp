// ═══════════════════════════════════════════════════════════
//  X2 迷你 IDE
//
//  界面部分（我写的）：代码编辑区 + 输出区 + 编译/清空按钮
//  编译部分（你写的）：wstring bianyi(wstring daima)  ← 找到下面的 TODO
//
//  编译：双击 cpp学习\build.bat，或 .\build.ps1 ide
// ═══════════════════════════════════════════════════════════

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cwchar>
#include <cstdio>
#include <shellapi.h>   // ShellExecuteW：像双击一样启动程序
#include <commdlg.h>    // GetOpenFileNameW / GetSaveFileNameW：系统文件对话框

using namespace std;

// ── 控件 ID ──
#define ID_BTN_COMPILE  101
#define ID_BTN_CLEAR    102
#define ID_STATIC_STATUS 103
#define ID_EDIT_CODE    104
#define ID_EDIT_OUTPUT  105
#define ID_BTN_OPEN     106
#define ID_BTN_SAVE     107

// ── 全局控件句柄 ──
HWND g_btnCompile = nullptr;
HWND g_btnClear   = nullptr;
HWND g_btnOpen    = nullptr;
HWND g_btnSave    = nullptr;
HWND g_stStatus   = nullptr;
HWND g_editCode   = nullptr;
HWND g_editOutput = nullptr;

// ── 当前打开的 .cpp 文件路径（空 = 还没保存过）──
wstring g_dangqianWenjian = L"";

// ── 临时文件路径（目录已由外部建好；你的编译函数里直接用这三个）──
static const wchar_t* F_CPP = L"C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\ide\\test.cpp";
static const wchar_t* F_EXE = L"C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\ide\\test.exe";
static const wchar_t* F_LOG = L"C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\ide\\log.txt";

// ══════════════════════════════════════════════════════════
//  编码转换小帮手（我写好了，你直接用）
//    宽字符串(wstring)  ⇄  UTF-8 字符串(string)   —— 写文件用
//    GBK 字符串(string) →  宽字符串(wstring)      —— g++ 的错误输出是 GBK，显示要用
// ══════════════════════════════════════════════════════════

// 宽字符串 → UTF-8
string WToUtf8(const wstring& ws)
{
    if (ws.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
    string s(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), &s[0], len, nullptr, nullptr);
    return s;
}

// GBK → 宽字符串
wstring GbkToW(const string& s)
{
    if (s.empty()) return L"";
    int len = MultiByteToWideChar(936, 0, s.c_str(), (int)s.size(), nullptr, 0);
    wstring ws(len, 0);
    MultiByteToWideChar(936, 0, s.c_str(), (int)s.size(), &ws[0], len);
    return ws;
}

// 宽字符串 → GBK（★ 专门用来喂给 ofstream / system：MinGW 的窄字符文件 API 和 cmd 都按 ANSI(GBK) 解释路径）
string WToGbk(const wstring& ws)
{
    if (ws.empty()) return "";
    int len = WideCharToMultiByte(936, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
    string s(len, 0);
    WideCharToMultiByte(936, 0, ws.c_str(), (int)ws.size(), &s[0], len, nullptr, nullptr);
    return s;
}

// UTF-8 → 宽字符串
wstring Utf8ToW(const string& s)
{
    if (s.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    wstring ws(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &ws[0], len);
    return ws;
}

// ══════════════════════════════════════════════════════════
//  文件读写（用 _wfopen：直接吃宽字符路径，绕开中英文编码问题）
// ══════════════════════════════════════════════════════════

// 读整个文件，返回 UTF-8 文本（失败返回空）
string DuQuWenjian(const wstring& lujing)
{
    FILE* fp = _wfopen(lujing.c_str(), L"rb");
    if (!fp) return "";
    string neirong;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
    {
        neirong.append(buf, n);
    }
    fclose(fp);
    return neirong;
}

// 把 UTF-8 文本写入文件（成功返回 true）
bool XieWenjian(const wstring& lujing, const string& neirong)
{
    FILE* fp = _wfopen(lujing.c_str(), L"wb");
    if (!fp) return false;
    if (!neirong.empty()) fwrite(neirong.data(), 1, neirong.size(), fp);
    fclose(fp);
    return true;
}

// 更新窗口标题：迷你 IDE —— 文件名
void GengxinBiaoti(HWND hwnd)
{
    wstring biaoti = L"迷你 IDE —— ";
    if (g_dangqianWenjian.empty())
    {
        biaoti += L"未命名.cpp";
    }
    else
    {
        // 只取文件名部分（最后一个反斜杠之后）
        size_t pos = g_dangqianWenjian.find_last_of(L"\\/");
        biaoti += (pos == wstring::npos) ? g_dangqianWenjian : g_dangqianWenjian.substr(pos + 1);
    }
    SetWindowTextW(hwnd, biaoti.c_str());
}

// 把编辑区里的全部文字取出来
wstring QuBianjiQi(HWND hEdit)
{
    int chang = GetWindowTextLengthW(hEdit);
    wstring s(chang, L'\0');
    if (chang > 0) GetWindowTextW(hEdit, &s[0], chang + 1);
    return s;
}

// ══════════════════════════════════════════════════════════
//  ★★★ 这是你的作业：把代码编译成 exe，返回"给人看的结果" ★★★
//
//  参数 daima  : 编辑区里的完整代码（宽字符串）
//  返回        : 成功 → L"[成功] ..."
//                失败 → L"[失败] " + g++ 的错误文本
//
//  三步（讲义 X2-1 里有例子）：
//    ① 写文件：ofstream out("路径");  out << WToUtf8(daima);  out.close();
//    ② 编译  ：string cmd = "cmd /c \"g++ " + 源 + " -o " + exe + " > " + log + " 2>&1\"";
//              int r = system(cmd.c_str());
//    ③ 读日志：ifstream in("路径"); getline 逐行读成 string；
//              再 GbkToW(...) 转成宽字符串返回
//              多行之间用 \r\n 连接
// ══════════════════════════════════════════════════════════
wstring bianyi(wstring daima)
{
    // 路径转 GBK —— ofstream / ifstream 的窄字符版本按 ANSI(GBK) 解释路径
    string cpp = WToGbk(F_CPP);
    string exe = WToGbk(F_EXE);
    string log = WToGbk(F_LOG);

    // ★ 本机特殊问题：g++ 写不进系统 %TEMP%（权限被拒）。
    //   所以编译前把 TMP/TEMP 环境变量指到工作区，子进程 g++ 就会用它。
    const wchar_t* TMPDIR = L"C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\tmp";
    CreateDirectoryW(TMPDIR, nullptr);           // 已存在会失败，无所谓
    SetEnvironmentVariableW(L"TMP",  TMPDIR);
    SetEnvironmentVariableW(L"TEMP", TMPDIR);

    // ① 写源码文件：内容用 UTF-8（GCC 默认按 UTF-8 读源码）
    ofstream out(cpp.c_str());
    if (!out) { return wstring(L"[失败] 无法创建源文件：") + F_CPP; }
    out << WToUtf8(daima);
    out.close();

    // ② 调 g++ 编译
    //    ★ 这里必须用 _wsystem（宽字符版 system）：
    //      普通 system() 会把命令行按 ANSI 编码传给 cmd，中文路径会乱码；
    //      _wsystem 传宽字符，cmd 能正确识别中文路径。
    wstring gpp = L"C:\\Users\\89520\\Desktop\\音乐\\Dev-C++\\Dev-Cpp\\MinGW64\\bin\\g++.exe";
    // ★ 首尾各多包一个引号：_wsystem 内部会拼成 "cmd /c <命令>"，
    //   而 cmd 的规则是"首尾都是引号时剥掉最外层"，这样里面的引号才能保留。
    wstring wcmd = L"\"\"" + gpp + L"\" -std=c++11 -o \"" + F_EXE + L"\" \"" + F_CPP +
                   L"\" > \"" + F_LOG + L"\" 2>&1\"";
    int r = _wsystem(wcmd.c_str());

    // ③ 读回 g++ 的输出（GBK），拼成宽字符串
    string jieguo = "";
    ifstream in(log.c_str());
    string line;
    while (getline(in, line)) { jieguo += line; jieguo += "\r\n"; }
    in.close();

    if (r == 0)
    {
        return wstring(L"[成功] 编译成功（") + F_EXE + L"）\r\n" + GbkToW(jieguo);
    }
    return wstring(L"[失败] g++ 输出：\r\n") + GbkToW(jieguo);
}


// ── 窗口过程 ──
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // ① 顶部按钮：编译运行
        g_btnCompile = CreateWindowExW(0, L"BUTTON", L"▶ 编译运行",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            10, 8, 120, 32,
            hwnd, (HMENU)ID_BTN_COMPILE, GetModuleHandleW(nullptr), nullptr);

        // ② 顶部按钮：打开
        g_btnOpen = CreateWindowExW(0, L"BUTTON", L"打开",
            WS_CHILD | WS_VISIBLE,
            138, 8, 80, 32,
            hwnd, (HMENU)ID_BTN_OPEN, GetModuleHandleW(nullptr), nullptr);

        // ③ 顶部按钮：保存
        g_btnSave = CreateWindowExW(0, L"BUTTON", L"保存",
            WS_CHILD | WS_VISIBLE,
            226, 8, 80, 32,
            hwnd, (HMENU)ID_BTN_SAVE, GetModuleHandleW(nullptr), nullptr);

        // ④ 顶部按钮：清空输出
        g_btnClear = CreateWindowExW(0, L"BUTTON", L"清空",
            WS_CHILD | WS_VISIBLE,
            314, 8, 80, 32,
            hwnd, (HMENU)ID_BTN_CLEAR, GetModuleHandleW(nullptr), nullptr);

        // ⑤ 状态文字
        g_stStatus = CreateWindowExW(0, L"STATIC", L"状态：就绪",
            WS_CHILD | WS_VISIBLE,
            404, 14, 190, 25,
            hwnd, (HMENU)ID_STATIC_STATUS, GetModuleHandleW(nullptr), nullptr);

        // ④ 代码编辑区（多行 + 滚动）
        g_editCode = CreateWindowExW(0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL
                | ES_MULTILINE | ES_WANTRETURN | WS_HSCROLL | ES_AUTOHSCROLL,
            10, 48, 560, 200,
            hwnd, (HMENU)ID_EDIT_CODE, GetModuleHandleW(nullptr), nullptr);

        // ⑤ 输出区（多行 + 只读）
        g_editOutput = CreateWindowExW(0, L"EDIT", L"（编译输出会显示在这里）",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL
                | ES_MULTILINE | ES_READONLY,
            10, 255, 560, 95,
            hwnd, (HMENU)ID_EDIT_OUTPUT, GetModuleHandleW(nullptr), nullptr);

        // 默认给一段示例代码，方便直接点编译
        SetWindowTextW(g_editCode,
            L"#include <iostream>\r\n"
            L"using namespace std;\r\n"
            L"\r\n"
            L"int main()\r\n"
            L"{\r\n"
            L"    cout << \"hello from my IDE\" << endl;\r\n"
            L"    return 0;\r\n"
            L"}\r\n");

        GengxinBiaoti(hwnd);   // 标题显示"未命名.cpp"

        return 0;
    }

    case WM_COMMAND:
    {
        // 编译运行
        if (LOWORD(wParam) == ID_BTN_COMPILE)
        {
            // 1) 取出编辑区里的全部代码
            wstring daima = QuBianjiQi(g_editCode);

            // 2) 调编译函数（你写的）
            SetWindowTextW(g_stStatus, L"状态：正在编译...");
            wstring jieguo = bianyi(daima);

            // 3) 显示结果
            SetWindowTextW(g_editOutput, jieguo.c_str());
            SetWindowTextW(g_stStatus, (jieguo.find(L"[成功]") != wstring::npos)
                                        ? L"状态：编译成功" : L"状态：编译结束（见下方输出）");

            // 4) ★ v1：编译成功后自动运行生成的 exe
            //    ShellExecuteW = "像在资源管理器里双击一样"启动程序，不会卡住 IDE
            //    返回值 > 32 表示成功（这是 ShellExecute 的老规矩）
            if (jieguo.find(L"[成功]") != wstring::npos)
            {
                HINSTANCE hRun = ShellExecuteW(nullptr, L"open", F_EXE, nullptr, nullptr, SW_SHOWNORMAL);
                if ((INT_PTR)hRun > 32)
                {
                    SetWindowTextW(g_stStatus, L"状态：编译成功，已启动程序");
                }
                else
                {
                    SetWindowTextW(g_stStatus, L"状态：编译成功，但启动失败");
                }
            }
        }
        // 清空输出
        else if (LOWORD(wParam) == ID_BTN_CLEAR)
        {
            SetWindowTextW(g_editOutput, L"");
            SetWindowTextW(g_stStatus, L"状态：就绪");
        }
        // ★ 打开 .cpp 文件
        else if (LOWORD(wParam) == ID_BTN_OPEN)
        {
            wchar_t lujing[MAX_PATH] = L"";
            OPENFILENAMEW ofn = {};
            ofn.lStructSize  = sizeof(ofn);
            ofn.hwndOwner    = hwnd;
            ofn.lpstrFilter  = L"C++ 源文件 (*.cpp)\0*.cpp\0文本文件 (*.txt)\0*.txt\0所有文件 (*.*)\0*.*\0";
            ofn.lpstrFile    = lujing;
            ofn.nMaxFile     = MAX_PATH;
            ofn.lpstrDefExt  = L"cpp";
            ofn.lpstrTitle   = L"打开 C++ 源文件";
            ofn.Flags        = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileNameW(&ofn))
            {
                string neirong = DuQuWenjian(lujing);
                if (neirong.empty())
                {
                    SetWindowTextW(g_stStatus, L"状态：文件是空的或读取失败");
                }
                SetWindowTextW(g_editCode, Utf8ToW(neirong).c_str());
                g_dangqianWenjian = lujing;
                GengxinBiaoti(hwnd);
                SetWindowTextW(g_stStatus, L"状态：文件已打开");
            }
        }
        // ★ 保存 .cpp 文件（已有路径直接存，没有就弹"另存为"）
        else if (LOWORD(wParam) == ID_BTN_SAVE)
        {
            wstring lujing = g_dangqianWenjian;
            if (lujing.empty())
            {
                wchar_t buf[MAX_PATH] = L"未命名.cpp";
                OPENFILENAMEW ofn = {};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner   = hwnd;
                ofn.lpstrFilter = L"C++ 源文件 (*.cpp)\0*.cpp\0所有文件 (*.*)\0*.*\0";
                ofn.lpstrFile   = buf;
                ofn.nMaxFile    = MAX_PATH;
                ofn.lpstrDefExt = L"cpp";
                ofn.lpstrTitle  = L"保存 C++ 源文件";
                ofn.Flags       = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
                if (!GetSaveFileNameW(&ofn)) return 0;   // 用户取消
                lujing = buf;
            }

            wstring daima = QuBianjiQi(g_editCode);
            if (XieWenjian(lujing, WToUtf8(daima)))
            {
                g_dangqianWenjian = lujing;
                GengxinBiaoti(hwnd);
                SetWindowTextW(g_stStatus, L"状态：已保存");
            }
            else
            {
                SetWindowTextW(g_stStatus, L"状态：保存失败（文件被占用？）");
            }
        }
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


int main()
{
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* CLASS = L"MiniIdeWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS, L"迷你 IDE —— 我的第一个编译器外壳",
        WS_OVERLAPPEDWINDOW,
        200, 100, 620, 430,
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
        // ★ 快捷键：Ctrl+S 保存，Ctrl+O 打开（在消息循环里拦，编辑框不会吃掉这两个组合键）
        if (msg.message == WM_KEYDOWN && (GetKeyState(VK_CONTROL) & 0x8000))
        {
            if (msg.wParam == 'S')
            {
                SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_BTN_SAVE, BN_CLICKED), 0);
                continue;
            }
            if (msg.wParam == 'O')
            {
                SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ID_BTN_OPEN, BN_CLICKED), 0);
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

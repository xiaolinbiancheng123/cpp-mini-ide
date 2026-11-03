# X2 讲义（一）：做一个小 IDE —— VS 的原理

> **目标**：做一个窗口程序，左边写代码，点「编译运行」就能编译并显示错误。
>
> **关键认知：VS 自己不编译代码！** VS 调用 MSVC 编译器；我们调用 g++。
> IDE 负责的是：**编辑 → 保存成文件 → 调用编译器 → 把结果显示出来**。

---

## 一、IDE 到底在干什么

```
你敲代码（编辑区）
   ↓ 点「编译运行」
① 把编辑区的内容写成 .cpp 文件
   ↓
② 调用编译器：g++ 代码.cpp -o 程序.exe
   ↓
③ 接住编译器的输出（错误/警告）
   ↓
④ 显示在输出区（成功就运行 exe）
```

**★ 我们 v0 只做这四步。VS 复杂的地方全在"编辑体验"（高亮/补全/跳转）和"调试器"，不在这条主线。**

---

## 二、v0 的界面（我来搭）

```
┌──────────────────────────────────────────────┐
│ [编译运行]  [清空]        状态：就绪          │  ← 工具条
├──────────────────────────────────────────────┤
│                                              │
│   代码编辑区（多行文本框，可滚动）              │
│                                              │
├──────────────────────────────────────────────┤
│   输出区（只读多行文本框，显示编译错误/成功）    │
└──────────────────────────────────────────────┘
```

**控件清单：**

| 控件 | 类名/样式 | 作用 |
|---|---|---|
| 编译按钮 | `BUTTON` + `BS_DEFPUSHBUTTON` | 触发编译 |
| 清空按钮 | `BUTTON` | 清空输出区 |
| 代码编辑区 | `EDIT` + `ES_MULTILINE \| WS_VSCROLL \| ES_AUTOVSCROLL` | 写代码 |
| 输出区 | 同上 + `ES_READONLY` | 显示结果 |

**★ 多行编辑框就是你已经在控件速查里见过的：**

```cpp
CreateWindowExW(0, L"EDIT", L"",
    WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL
        | ES_MULTILINE | ES_AUTOVSCROLL,      // ← 多行 + 滚动
    10, 50, 560, 200,
    hwnd, (HMENU)ID_CODE, GetModuleHandleW(nullptr), nullptr);
```

**★★ 只读输出区再加 `ES_READONLY`（用户不能改，但程序能写）。**

---

## 三、你要写的函数：`bianyi`

```cpp
// 功能：把源码字符串编译成 exe，返回"给人看的结果文字"
// 参数：daima —— 编辑区里的完整代码
// 返回：成功 → "编译成功！"
//       失败 → g++ 输出的错误文本
string bianyi(string daima);
```

**内部三步：**

### ① 写文件：`ofstream`

```cpp
#include <fstream>
using namespace std;

ofstream out("C:\\temp\\test.cpp");   // 打开/创建文件（路径写死一个临时位置）
out << daima;                          // 写入字符串
out.close();                           // 关闭（一定要关，否则内容可能没落盘）
```

**★ 类似 `cout`，只不过输出的目标是文件。**
**★★ 路径里的反斜杠要写两个：`"C:\\temp\\x.cpp"`。**

### ② 调用编译器：`system()`

```cpp
#include <cstdlib>

// system 会打开 cmd 执行这条命令，等它跑完
int fanhuizhi = system("cmd /c \"g++ C:\\temp\\test.cpp -o C:\\temp\\test.exe > C:\\temp\\log.txt 2>&1\"");

// 返回值 0 = 成功，非 0 = 编译失败
```

**★ `>` 是重定向：把 g++ 的正常输出写进 log.txt。**
**★ `2>&1` 把错误输出也合并进去——这样错误信息都能从 log.txt 读到。**
**★★ `cmd /c` 是"执行完就退出"。**

### ③ 读错误：`ifstream`

```cpp
#include <fstream>
#include <string>

ifstream in("C:\\temp\\log.txt");
string line, jieguo = "";
while (getline(in, line))          // 一行一行读
{
    jieguo += line;
    jieguo += "\r\n";              // 多行文本框里换行用 \r\n
}
in.close();
return jieguo;                     // 返回给界面显示
```

**★ `getline(in, line)` 读一整行（到换行符为止）。**
**★★ 多行编辑框里换行必须 `\r\n`（只有 `\n` 可能不换行）。**

---

## 四、完整骨架（结构看懂，细节你写）

```cpp
string bianyi(string daima)
{
    // 临时文件路径（写死一个目录，先不管清理）
    string wenjian = "C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\ide\\test.cpp";
    string chengxu = "C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\ide\\test.exe";
    string rizhi   = "C:\\Users\\89520\\Desktop\\音乐\\cpp学习\\output\\ide\\log.txt";

    // ① 写源码
    ofstream out(wenjian.c_str());
    out << daima;
    out.close();

    // ② 编译（命令拼出来）
    string mingling = "cmd /c \"g++ " + wenjian + " -o " + chengxu + " > " + rizhi + " 2>&1\"";
    int r = system(mingling.c_str());

    // ③ 读日志
    string jieguo = "";
    ifstream in(rizhi.c_str());
    string line;
    while (getline(in, line)) { jieguo += line; jieguo += "\r\n"; }
    in.close();

    if (r == 0) { return "[成功] " + jieguo; }
    return "[失败] " + jieguo;
}
```

**★ 注意 `out`/`in`/`system` 都要 `.c_str()`：C++ 的 string 要转成 C 风格字符串（`const char*`）才能给这些老 API 用。这是我们第一次碰"两种字符串"的转换。**

---

## 五、你还需要知道的两个坑

### ① 中文乱码

```
g++ 的错误输出是 GBK（中文系统编码）
编辑区的文字是 UTF-8
```

**★ v0 先不管——**报错行里的中文路径会乱码，但英文错误信息是准的**。等 v1 再修（用 `chcp 65001` 或转码）。**

### ② 正在运行的 exe 不能覆盖

**★ 如果上次编译出的 exe 还在运行，g++ 会报 `cannot open output file`——**先关掉程序再编译**。**

---

## 六、本课任务

### 任务 A（热身）：多行文本框

```
☐ 新建 X2_迷你IDE\ide.cpp（复制 模板\带控件的窗口模板.cpp）
☐ 改成：一个大号多行编辑框（代码区）+ 一个只读多行框（输出区）+ 两个按钮
☐ 编译零警告，窗口能正常显示
```

### 任务 B（核心）：写 `bianyi` 函数

```
☐ 按上面的骨架写出来
☐ 先不接界面：在 main 里直接测试
      cout << bianyi("int main(){ return 0; }");
      → 应该输出 "[成功]"
      cout << bianyi("int main(){ int x = ; }");
      → 应该输出 g++ 的错误
```

**★ 任务 B 单独测通，再接到按钮上（接线我来）。**

---

## 七、验收标准

| 测试 | 期望 |
|---|---|
| **正确代码** | 返回文字含 "成功" 或 "error" 不出现 |
| **语法错误代码** | 返回文字包含 `error:` 和行号 |
| **空代码** | g++ 报 "undefined reference to main"（也是错误，正常） |
| **中文注释的代码** | 能编译成功（注释不影响） |
| **点编译按钮** | 输出区显示结果（接线后） |

---

## 八、下一步（v1 预告）

```
v1：点"成功"后自动运行生成的 exe
v2：打开/保存 .cpp 文件
v3：语法高亮（这个难，是 VS 的核心体验）
v4：更友好的错误显示（点错误跳转到对应行）
```

**★ v0 做出来你就有一个"能写代码、能编译、能看错误"的小 IDE 了——这就是 VS 的主干。**

---

## ★ v1 已实现（2026-11-03）

| 功能 | 用到的 API |
|---|---|
| 编译成功后自动运行 exe | ShellExecuteW（像双击一样启动，不阻塞 IDE） |
| 打开 .cpp | GetOpenFileNameW（系统打开对话框）+ _wfopen 读文件 |
| 保存 .cpp | GetSaveFileNameW（系统另存为）+ _wfopen 写文件 |
| 快捷键 | 消息循环里拦 Ctrl+S / Ctrl+O |

**★ 关键收获：_wfopen 直接吃宽字符路径，绕开中英文编码问题；系统对话框用 OPENFILENAMEW 结构体配置。**

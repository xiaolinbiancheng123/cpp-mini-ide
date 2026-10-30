// ═══════════════════════════════════════════════════════════
//  第 01 课：第一个程序
//
//  编译（在 PowerShell 里）：
//    C:\Users\89520\Desktop\音乐\Dev-C++\Dev-Cpp\MinGW64\bin\g++.exe -std=c++11 -o 01_hello.exe 01_hello.cpp
//  运行：
//    .\01_hello.exe
//
//  ═══ 一个 C++ 程序由三部分组成 ═══
//    ① 预处理指令（#include）—— 把别人写好的工具箱搬进来
//    ② 主函数 main()      —— 程序唯一的入口，从它开始跑
//    ③ 语句               —— 每条以分号 ; 结尾
// ═══════════════════════════════════════════════════════════

#include <iostream>   // iostream = input/output stream，提供 cout（输出）和 cin（输入）
#include <string>     // string，用来存放一串字符

// 三个命名空间里的东西就不用每次写 std:: 了。
// 注意：这是新手写法；大项目里一般写 std::cout，避免名字撞车。
using namespace std;

int main() {                      // int 表示这个函数结束时返回一个整数给系统
                                  // 0 = 一切正常

    // ── 1. 最简单的输出 ──
    cout << "你好，C++！" << endl;
    // cout      → console output，控制台输出
    // <<        → 把右边的东西"流"进 cout，可以连着写
    // endl      → end line，换行并立刻刷新输出

    // ── 2. 变量：先声明类型，再起名字，再赋值 ──
    int    age    = 18;           // 整数
    double height = 1.75;         // 小数（双精度）
    char   grade  = 'A';          // 单个字符，用单引号
    string name   = "龙虾";       // 字符串，用双引号
    bool   isOk   = true;         // 布尔：true / false

    cout << "名字: " << name   << endl;
    cout << "年龄: " << age    << endl;
    cout << "身高: " << height << endl;
    cout << "等级: " << grade  << endl;
    cout << "状态: " << isOk   << endl;   // 打印出来是 1 (1=真, 0=假)

    // ── 3. 算术 ──
    int a = 7, b = 2;
    cout << "7 + 2 = " << a + b << endl;
    cout << "7 - 2 = " << a - b << endl;
    cout << "7 * 2 = " << a * b << endl;
    cout << "7 / 2 = " << a / b << endl;        // 整数除法 → 3，不是 3.5
    cout << "7 % 2 = " << a % b << endl;        // 取余数 → 1
    cout << "7.0 / 2 = " << 7.0 / b << endl;    // 有一个是小数，结果就是小数

    return 0;                     // 告诉系统：我正常结束了
}

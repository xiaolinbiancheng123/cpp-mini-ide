// ═══════════════════════════════════════════════════════════
//  第 02 课：循环、判断、输入
//
//  编译运行：
//    g++ -std=c++11 -o 02_for.exe 02_for.cpp
//    chcp 65001     ← 先切到 UTF-8，否则中文会乱码
//    .\02_for.exe
// ═══════════════════════════════════════════════════════════

#include <iostream>
using namespace std;

int main() {

    // ── 1. for 循环：控制台打印 1 到 5 ──
    //    格式：for (起点; 继续的条件; 每轮做什么)
    cout << "--- 数到 5 ---" << endl;
    for (int i = 1; i <= 5; i++) {
        cout << i << " ";
    }
    cout << endl;

    // ── 2. 累加：1+2+3+...+100 ──
    int sum = 0;                       // 累加器，必须先归零
    for (int i = 1; i <= 100; i++) {
        sum += i;                      // 等价于 sum = sum + i
    }
    cout << "1+2+...+100 = " << sum << endl;

    // ── 3. 嵌套循环：九九乘法表 ──
    //    外层 i 管行，内层 j 管列
    cout << "--- 九九乘法表 ---" << endl;
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= i; j++) {          // 每行只打到 i 为止
            cout << j << "x" << i << "=" << i * j << "\t";   // \t = 制表符，对齐用
        }
        cout << endl;                           // 一行打完换行
    }

    // ── 4. if / else if / else：按分数判等级 ──
    cout << "--- 判等级 ---" << endl;
    int score = 87;
    if (score >= 90) {
        cout << score << " → 优" << endl;
    } else if (score >= 80) {
        cout << score << " → 良" << endl;
    } else if (score >= 60) {
        cout << score << " → 及格" << endl;
    } else {
        cout << score << " → 不及格" << endl;
    }

    // ── 5. while 循环 + 从键盘读输入 ──
    //    cin >> x 会等你敲一个数再回车；敲 0 就退出
    cout << "--- 求和器：输入数字累加，输 0 结束 ---" << endl;
    int x = 0, total = 0, count = 0;
    while (cin >> x) {                 // 读失败（比如输入了字母）就跳出
        if (x == 0) break;             // break = 立刻跳出循环
        total += x;
        count++;
        cout << "  已累加 " << count << " 个数，当前和 = " << total << endl;
    }
    cout << "结束。共 " << count << " 个数，总和 " << total << endl;

    return 0;
}

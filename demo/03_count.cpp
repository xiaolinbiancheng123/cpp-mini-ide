// ═══════════════════════════════════════════════════════════
//  第 03 课（项目一 · 第 1 步）：读文件 + 正确数中文字数
//
//  编译：
//    g++ -std=c++11 -o 03_count.exe 03_count.cpp
//  运行（把小说路径当参数传进去）：
//    03_count.exe "C:\...\朕重生后，只想做他爹_全本.txt"
//
//  ★ 这一课只有一个核心知识点，但它是中文处理最经典的坑：
//    UTF-8 里一个汉字占 3 个字节，所以 string.size() 得到的是【字节数】，不是【字数】。
//    "好" 的 size() 是 3，但它是 1 个字。
//    要数字数，必须自己按 UTF-8 的编码规则逐字节判断。
// ═══════════════════════════════════════════════════════════

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "用法: 03_count.exe <txt文件路径>" << endl;
        return 1;
    }
    string path = argv[1];

    // ifstream = input file stream。默认文本模式，会自动处理 Windows 的 CRLF
    ifstream in(path.c_str());
    if (!in.is_open()) {
        cerr << "打不开文件: " << path << endl;
        return 1;
    }

    string line;
    long long lineCount  = 0;   // 行数
    long long byteCount  = 0;   // 字节数
    long long charCount  = 0;   // 字符数（按 UTF-8 码点算）
    long long hanCount   = 0;   // 汉字数（3 字节的那些）
    long long maxLineLen = 0;   // 最长一行有多少字节
    int       maxLineNo  = 0;

    while (getline(in, line)) {           // getline 每次读一行，读到结尾自动去掉换行符
        lineCount++;
        byteCount += (long long)line.size() + 1;   // +1 是那被去掉的换行符

        // ── 逐字节判断这个字符占几字节 ──
        for (size_t i = 0; i < line.size(); ) {
            unsigned char c = (unsigned char)line[i];
            int step;
            if      ((c & 0x80) == 0x00) step = 1;   // 0xxxxxxx  ASCII，1 字节
            else if ((c & 0xE0) == 0xC0) step = 2;   // 110xxxxx  2 字节
            else if ((c & 0xF0) == 0xE0) { step = 3; hanCount++; }  // 1110xxxx 3 字节，多为汉字
            else                         step = 4;   // 11110xxx  4 字节，emoji 等

            charCount++;
            i += step;                                // 跳过这个字符占的所有字节
        }

        if ((long long)line.size() > maxLineLen) {
            maxLineLen = (long long)line.size();
            maxLineNo  = (int)lineCount;
        }
    }
    in.close();

    cout << "──── 文件体检 ────" << endl;
    cout << "文件      : " << path << endl;
    cout << "字节数    : " << byteCount << "  (约 "
         << (byteCount / 1024.0 / 1024.0) << " MB)" << endl;
    cout << "行数      : " << lineCount << endl;
    cout << "字符数    : " << charCount << "  ← 这才是真正的字数" << endl;
    cout << "其中汉字  : " << hanCount << endl;
    cout << "最长行    : 第 " << maxLineNo << " 行, " << maxLineLen << " 字节" << endl;
    cout << "平均每行  : " << (lineCount ? (double)charCount / lineCount : 0) << " 字" << endl;
    return 0;
}

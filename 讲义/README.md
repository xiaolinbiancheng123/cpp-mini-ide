# 讲义目录

每一节课的完整内容都存成文件，**不用再翻聊天记录**。

| 文件 | 内容 | 状态 |
|---|---|---|
| [术语表.md](术语表.md) | **所有术语的人话解释**，看不懂的词先查这里 | 持续补充 |
| [L0-1_变量与类型.md](L0-1_变量与类型.md) | 变量、类型、运算、cout/cin、整数除法陷阱、Windows 双击坑 | ✅ 已学 |
| [L0-2_循环与数组.md](L0-2_循环与数组.md) | for / while、break/continue、数组、越界、累加与打擂台 | 进行中 |
| L0-3_函数与结构体.md | 函数（参数/返回值/作用域）、结构体（打包数据）、函数指针 | 待学 |
| L1-1_文件读写.md | `ifstream`、`getline`、UTF-8 字节 vs 字数 | 待学 |
| L1-2_字符串处理.md | `substr`、`find`、`npos`、`vector`、切章 | 待学 |

## 学习方式

1. 先看讲义，理解每个语法**能干什么**
2. 再做 `L0_x_xxx\` 文件夹里的任务（要求写在该目录的 `要求.txt`）
3. 编译验收，报错就贴出来
4. 通过了就 `git commit`

## 头文件速查

| 头文件 | 提供 |
|---|---|
| `<iostream>` | `cout` `cin` `cerr` `endl` |
| `<string>` | `string` 类型及其 `.size()` `.substr()` `.find()` |
| `<fstream>` | `ifstream` 读文件、`ofstream` 写文件 |
| `<vector>` | 可变长数组 |
| `<cstdlib>` | `system()`（用于 chcp / pause） |
| `<conio.h>` | `_getch()` 按任意键 |
| `<windows.h>` | `Sleep()`（毫秒）、窗口 API |
| `<thread>` `<chrono>` | `sleep_for(seconds(n))` 跨平台秒级睡眠 |

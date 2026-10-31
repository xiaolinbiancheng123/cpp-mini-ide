#include<iostream>
#include<vector>
#include<string>
#include<iomanip>
using namespace std;
int main() {
    vector<string> names;
    vector<int>    scores;
    string name;
    int    score;

    // ① 读入
    while (cin >> name >> score) {
        names.push_back(name);
        scores.push_back(score);
    }

    // ② 空检查 —— 必须在碰 scores[0] 之前
    if (scores.empty()) {
        cout << "没有数据" << endl;
        return 0;                    // ← 关键：直接结束，别往下走
    }

    // ③ 统计变量声明在【循环外面】
    int total = 0;
    int maxS  = scores[0];
    int minS  = scores[0];

    // ④ 一个循环：打印 + 统计
    for (size_t i = 0; i < names.size(); ++i) {
        cout << names[i] << " " << scores[i] << endl;

        total += scores[i];                  // 累加
        if (scores[i] > maxS) maxS = scores[i];   // 打擂台（大）
        if (scores[i] < minS) minS = scores[i];   // 打擂台（小）
    }

    // ⑤ 循环结束，变量还活着，这时才算平均、才输出
    double avg = total / (double)scores.size();

    cout << "人数: " << scores.size() << endl;
    cout << "总分: " << total << endl;
    cout << "平均: " << fixed << setprecision(4) << avg << endl;
    cout << "最高: " << maxS << endl;
    cout << "最低: " << minS << endl;

    return 0;
}
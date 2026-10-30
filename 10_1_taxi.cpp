#include <iostream>
#include <string>
using namespace std;


int main() {
    string name ;
    double jiage = 0;
    double gonglishu = 0;
    int shijian = 0;
    double dengdaifei = 0;


    cout << "请输入公里数" << endl;
    cin >> gonglishu;
    cout << "请输入等待时间（低速时间）" << endl;
    cin >> shijian;
    if(gonglishu < 3){
        jiage = jiage + 10;
        dengdaifei = shijian * 0.3;
        gonglishu = 0; //因为公里数不参与计算，所以在最后算钱的时候会算错，覆写为0
    }else{
        jiage = jiage + 10;
        gonglishu = gonglishu - 3;
        gonglishu = gonglishu * 2.3;
        dengdaifei = shijian * 0.3;   
    }//但是else分支中公里数（gonglishu)这个变量参与计算，所以不覆写为0
    jiage = jiage + gonglishu + dengdaifei;
    cout << "单价为:";
    cout << jiage;
    return 0;
}
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
using namespace std;

// 控制面板类
class ControlPanel {
private:
    string correctPassword;  // 正确密码
    string inputPassword;    // 输入密码
    int errorCount;          // 错误次数
    bool isLocked;           // 是否锁定
    time_t lockStartTime;    // 锁定开始时间

    // 状态枚举
    enum State {
        WAITING_FOR_PASSWORD,  // 等待输入密码
        SYSTEM_FUNCTION_SELECT,// 系统功能选择
        LOCKED                 // 锁定状态
    };

    State currentState;

public:
    ControlPanel() {
        correctPassword = "1234";  // 默认密码
        errorCount = 0;
        isLocked = false;
        currentState = WAITING_FOR_PASSWORD;
    }

    // 输入字符处理
    void inputChar(char c) {
        if (currentState != WAITING_FOR_PASSWORD) {
            cout << "当前状态无法输入密码" << endl;
            return;
        }

        if (inputPassword.length() < 4) {
            inputPassword += c;
            cout << "输入密码: " << string(inputPassword.length(), '*') << endl;

            // 密码长度达到4位，验证密码
            if (inputPassword.length() == 4) {
                verifyPassword();
            }
        }
    }

    // 验证密码
    void verifyPassword() {
        if (inputPassword == correctPassword) {
            cout << "密码正确，进入系统功能选择" << endl;
            currentState = SYSTEM_FUNCTION_SELECT;
            inputPassword.clear();
            errorCount = 0;
        } else {
            errorCount++;
            cout << "密码错误，剩余次数: " << 3 - errorCount << endl;
            inputPassword.clear();

            if (errorCount >= 3) {
                lockControlPanel();
            }
        }
    }

    // 锁定控制面板
    void lockControlPanel() {
        currentState = LOCKED;
        isLocked = true;
        lockStartTime = time(nullptr);
        cout << "密码错误3次，控制面板锁定120秒" << endl;

        // 启动定时器，120秒后解锁
        thread([this]() {
            this_thread::sleep_for(chrono::seconds(120));
            unlockControlPanel();
        }).detach();
    }

    // 解锁控制面板
    void unlockControlPanel() {
        currentState = WAITING_FOR_PASSWORD;
        isLocked = false;
        errorCount = 0;
        cout << "控制面板已解锁，请输入密码" << endl;
    }

    // 系统功能选择
    void selectFunction(int func) {
        if (currentState != SYSTEM_FUNCTION_SELECT) {
            cout << "当前状态无法选择功能" << endl;
            return;
        }

        cout << "选择功能: " << func << "（此处可扩展具体功能实现）" << endl;
        // 模拟功能执行后返回等待密码状态
        currentState = WAITING_FOR_PASSWORD;
        cout << "功能执行完毕，返回等待密码状态" << endl;
    }

    // 获取当前状态
    string getCurrentState() {
        switch (currentState) {
            case WAITING_FOR_PASSWORD:
                return "等待输入密码";
            case SYSTEM_FUNCTION_SELECT:
                return "系统功能选择";
            case LOCKED:
                return "锁定状态";
            default:
                return "未知状态";
        }
    }
};

// 测试代码
int main() {
    ControlPanel panel;

    cout << "=== SafeHome控制面板 ===" << endl;
    cout << "当前状态: " << panel.getCurrentState() << endl;
    cout << "请输入4位密码: " << endl;

    // 模拟输入密码（正确密码1234）
    panel.inputChar('1');
    panel.inputChar('2');
    panel.inputChar('3');
    panel.inputChar('5');  // 错误密码

    panel.inputChar('1');
    panel.inputChar('2');
    panel.inputChar('3');
    panel.inputChar('6');  // 错误密码

    panel.inputChar('1');
    panel.inputChar('2');
    panel.inputChar('3');
    panel.inputChar('7');  // 错误密码，锁定

    // 等待锁定时间（实际测试时可注释此行，直接测试锁定状态）
    this_thread::sleep_for(chrono::seconds(120));

    // 锁定后输入
    panel.inputChar('1');
    panel.inputChar('2');
    panel.inputChar('3');
    panel.inputChar('4');  // 正确密码，进入功能选择

    // 选择功能
    panel.selectFunction(1);  // 选择功能1

    return 0;
}
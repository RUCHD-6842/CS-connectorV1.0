#include <iostream>
#include <windows.h>
#include <thread>
#include <atomic>
#include <csignal>

const int CLICK_INTERVAL_MS = 10;

std::atomic<bool> running(false);

void Clicker() {
    while (running) {
        for (int i = 0; i < 10; ++i) {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &input, sizeof(INPUT));

            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(INPUT));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(CLICK_INTERVAL_MS));
    }
}

bool DisplayDisclaimer() {
    std::string input;
    while (true) {
        std::cout << "欢迎使用CS connectorV1.0\n";
        std::cout << "\n请阅读用户条约\n";
        std::cout << "1.请勿使用此连点器进行违反法律的活动\n";
        std::cout << "2.请勿传播连点器源码(如您拥有)\n";
        std::cout << "3.此连点器为左键连点器\n";
        std::cout << "4.V1.0永久免费，如果您购买了，那请您尝试退款\n";
        std::cout << "请同意条约(Y/N): ";
        std::cin >> input;

        if (input == "Y" || input == "y") {
            system("cls"); // 清屏
            return true;
        }
        else if (input == "N" || input == "n") {
            return false;
        }
        else {
            std::cout << "\n\n输入无效\n\n\n";
        }
    }
}

void SignalHandler(int signal) {
    std::cerr << "程序已崩溃" << std::endl;
    Sleep(2000); // 等待2秒
    exit(signal);
}

void CheckKeyPresses(std::thread& clickerThread) {
    HWND consoleWindow = GetConsoleWindow();
    EnableMenuItem(GetSystemMenu(consoleWindow, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

    while (true) {
        try {
            if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('M') & 0x8000) {
                if (running) {
                    running = false;
                    if (clickerThread.joinable()) {
                        clickerThread.join();
                    }
                    std::cout << "已停止连点" << std::endl;
                }
                else {
                    running = true;
                    clickerThread = std::thread(Clicker);
                    clickerThread.detach();
                    std::cout << "已启动连点" << std::endl;
                }
                Sleep(300);
            }

            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                running = false;
                if (clickerThread.joinable()) {
                    clickerThread.join();
                }
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        catch (...) {
            SignalHandler(SIGABRT);
        }
    }

    std::cout << "正在退出" << std::endl;
    Sleep(2000); // 等待2秒
}

int main() {
    // 注册信号处理函数
    std::signal(SIGSEGV, SignalHandler); // 捕获段错误
    std::signal(SIGABRT, SignalHandler); // 捕获异常终止

    if (!DisplayDisclaimer()) {
        std::cout << "正在退出" << std::endl;
        Sleep(2000); // 等待2秒
        return 0;
    }

    std::cout << "按下'Ctrl + M'键启动/停止连点器，按下'ESC'键退出程序" << std::endl;

    std::thread clickerThread;

    CheckKeyPresses(clickerThread);

    return 0;
}
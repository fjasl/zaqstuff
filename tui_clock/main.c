#include "renderer.h"
#include "timeDeal.h"
#include <signal.h>
#include <stdbool.h>
#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif

volatile sig_atomic_t keep_running = 1;

// 信号处理函数，让程序在按下 Ctrl+C 时可以优雅退出并恢复光标
void handle_sigint(int sig) {
  (void)sig; // 消除 unused parameter 警告
  keep_running = 0;
}

int main(void) {
  // 注册信号处理，捕获 Ctrl+C，避免用户终端光标永久消失
  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);

  init_renderer();

  // 主循环：每秒 30 帧刷新
  while (keep_running) {
    struct tm *now = getNowTime();

    // 渲染一帧
    render_frame(now->tm_hour, now->tm_min, now->tm_sec);

    // 休眠大约 33 毫秒 ≈ 30 帧每秒
    sleep_ms(33);
  }

  // 退出前打扫战场
  cleanup_renderer();
  return 0;
}
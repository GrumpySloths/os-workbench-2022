#include <stdio.h>

void drawRectangle(int percentage) {
    int i;
    int num_chars = percentage / 2; // 每2%对应一个字符

    // 绘制绿色部分
    printf("\033[0;42m"); // 设置背景颜色为绿色
    for (i = 0; i < num_chars; i++) {
        printf(" ");
    }

    // 绘制红色部分
    printf("\033[0;41m"); // 设置背景颜色为红色
    for (i = num_chars; i < 50; i++) {
        printf(" ");
    }

    // 恢复默认颜色
    printf("\033[0m\n");
}

int main() {
    int syscall_percentage = 30; // 假设系统调用占总时间的百分比为30%
    int a = 3.2;
    printf("a=%d\n", a);
    drawRectangle(syscall_percentage);

    return 0;
}

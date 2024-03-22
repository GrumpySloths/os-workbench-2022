#include <stdio.h>

int main() {
    FILE *fp;
    char path[1035];

    // 打开一个进程并写入模式执行Python脚本

    //构建循环，交互式的从终端读取输入，将输入写入到Python脚本中
    while (1) {
        fp = popen("python plugin.py", "w");
        if (fp == NULL) {
            printf("Failed to run command\n");
            return 1;
        }
        
        printf("Enter a message: ");
        fgets(path, sizeof(path), stdin);
        fprintf(fp, "%s", path);

        pclose(fp);
    }

    // 向Python脚本写入数据
    fprintf(fp, "Hello from C program! int a=3;");

    // 关闭文件流

    return 0;
}

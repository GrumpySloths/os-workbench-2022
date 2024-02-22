#include <stdio.h>
#include <time.h>
#include<unistd.h>
typedef struct __canvas_t { 
    int row; /*画布行数*/
    int col; /*画布列数*/
    int cur_x;
    int cur_y; /*光标位置*/
    int color; /*当前画布背景色*/
} canvas_t;
void draw(canvas_t*canvas,int percent){
    int cur_row = canvas->row - canvas->cur_x + 2;
    int cur_col = canvas->col - canvas->cur_y + 1;
    if(cur_row<=cur_col/2){
        // printf("行优先模式填充\n");
        /*行优先填充模式*/
        printf("\033[%dm", canvas->color); //切换画布颜色
        int col_draw =canvas->col* percent / ((float)cur_row / canvas->row * 100);
        int tmp_x = canvas->cur_x, tmp_y = canvas->cur_y;
        for (int i = 0; i < cur_row; i++) {
            /*切换cur位置*/
            printf("\033[%d;%dH",tmp_x,tmp_y);
            for (int j = 0; j < col_draw;j++){
                printf(" ");
            }
            tmp_x++;
        }
        printf("\033[m"); //将画布颜色重新切换为默认
        // 更新canvas
        canvas->color++;
        canvas->cur_y += col_draw;
    }else{
        /*列优先填充模式*/
        // printf("列优先模式填充\n");
        printf("\033[%dm", canvas->color);  // 切换画布颜色
        int row_draw = canvas->row*percent / ((float)cur_col / canvas->col * 100);
        int tmp_x = canvas->cur_x, tmp_y = canvas->cur_y;
        for (int i = 0; i < row_draw; i++) {
            /*切换cur位置*/
            printf("\033[%d;%dH",tmp_x,tmp_y);
            for (int j = 0; j < cur_col;j++){
                printf(" ");
            }
            tmp_x++;
        }
        printf("\033[m"); //将画布颜色重新切换为默认
        // 更新canvas
        canvas->color++;
        canvas->cur_x += row_draw; 
    }
}
int main(void) {
    canvas_t*canvas = &(struct __canvas_t){
        .row = 26, .col = 50, .cur_x = 2, .cur_y = 1, .color = 41};
    draw(canvas, 40);
    draw(canvas, 18);
    draw(canvas, 9);
    draw(canvas, 5);
    draw(canvas, 4);
    // fflush(stdout);

    // while (1) {
    //     printf("\033[2;1H");
    //     printf("\033[2J");
    //     for (i = 0; i < n;i++){
    //         for (j = 0; j < n;j++){
    //             printf("\033[41m  \033[m");
    //             // printf("\033[41m\033[m");
    //         }
    //         printf("\n");
    //     }
    //     sleep(1);
    //     n+=4;
    //     if(n>=30)
    //         break;
    // }
    return 0;
}
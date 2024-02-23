#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include<regex.h>
#include<assert.h>
#include<time.h>
#define MAX_LINE 512
#define MAX_NAME 100

typedef struct __record_t{
    /*记录系统调用以及相应消耗时间*/
    char *names[MAX_NAME]; /*记录系统调用的名称*/
    float times[MAX_NAME]; /*记录对应系统调用所消耗的总时间*/
    int counter;
    float time_total;/*总的syscall调用时间*/
} record_t;
typedef struct __canvas_t { 
    int row; /*画布行数*/
    int col; /*画布列数*/
    int cur_x;
    int cur_y; /*光标位置*/
    int color; /*当前画布背景色*/
} canvas_t;

void draw(canvas_t*canvas,int percent,char*name){
    int len_name = strlen(name);
    int ptr_name = 0; //名字
    char pct_str[8];
    sprintf(pct_str, "(%d%%)", percent);
    int len_pct = strlen(pct_str);
    int ptr_pct = 0;//percent
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
                if(i==cur_row/2&&j>=(col_draw-len_name)/2&&ptr_name<len_name){
                    printf("%c", name[ptr_name]);
                    ptr_name++;
                }else if(i==(cur_row/2+1)&&j>=(col_draw-len_pct)/2&&ptr_pct<len_pct){
                    printf("%c", pct_str[ptr_pct]);
                    ptr_pct++;
                }else
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
                if(i==row_draw/2&&j>=(cur_col-len_name)/2&&ptr_name<len_name){
                    printf("%c", name[ptr_name]);
                    ptr_name++;
                }else if(i==(row_draw/2+1)&&j>=(cur_col-len_pct)/2&&ptr_pct<len_pct){
                    printf("%c", pct_str[ptr_pct]);
                    ptr_pct++;
                }else
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

int readline(int fd, char *buffer, int max_length) {
    int bytes_read = 0;
    int bytes_read_total = 0;
    char ch;

    while (bytes_read_total < max_length - 1) {
        if (read(fd, &ch, 1) == 1) {
            buffer[bytes_read_total++] = ch;
            if (ch == '\n') {
                break;
            }
        } else {
            break;
        }
    }

    buffer[bytes_read_total] = '\0';

    return bytes_read_total;
}

void find_top5(record_t *rec,canvas_t*canvas){
    int i, j;
    float temp_time;
    char *temp_name;
    
    // 对times进行降序排序，并同时调整names数组的顺序
    for(i = 0; i < rec->counter; i++){
        for(j = i+1; j < rec->counter; j++){
            if(rec->times[i] < rec->times[j]){
                // 交换times数组中的元素
                temp_time = rec->times[i];
                rec->times[i] = rec->times[j];
                rec->times[j] = temp_time;
                
                // 交换names数组中的元素
                temp_name = rec->names[i];
                rec->names[i] = rec->names[j];
                rec->names[j] = temp_name;
            }
        }
    }
    
    // 输出耗时最多的top5系统调用及其相应的名称
    // printf("Top 5 System Calls by Execution Time:\n");
    for(i = 0; i < 5 && i < rec->counter; i++){
        printf("%d. Name: %s, Time: %f percent:%.1f%%\n", i+1, rec->names[i], rec->times[i],rec->times[i]/rec->time_total*100);
        int percent = rec->times[i] / rec->time_total * 100;
        // draw(canvas, percent,rec->names[i]);
    }
}

int syscall_record(record_t*record,char *name, float time) {
    assert(time >= 0);
    for (int i = 0; i < record->counter;i++){
        if(strncmp(name,record->names[i],strlen(name))==0){
            record->times[i] += time;
            return 0;
        }
    }
    record->names[record->counter] = name;
    record->times[record->counter] = time;
    record->counter++;
    return 0;
}
int main(int argc, char *argv[]) {
    assert(argc >= 2);
    canvas_t*canvas = &(struct __canvas_t){
        .row = 26, .col = 50, .cur_x = 2, .cur_y = 1, .color = 41};
    record_t *record = (record_t *)malloc(sizeof(record_t));
    record->counter = 0;
    char *exec_argv[20] = {
        "strace",
        "-T",
        NULL,
    };
    for (int i = 1; i < argc;i++){
        exec_argv[i + 1] = argv[i];
    }
    exec_argv[argc + 1] = NULL;
    char *exec_envp[] = {
        "PATH=/bin",
        NULL,
    };
    int pipefd[2];
    pid_t cpid;
    char buffer[MAX_LINE];
    time_t cur;

    regex_t regex_1;
    regex_t regex_2;
    regmatch_t matches1[2];
    regmatch_t matches2[2];
    char *pattern_1 = "([^\\(]+)\\(";  // 匹配"("前的内容
    char *pattern_2 = "<([^>]*)>";     // 匹配尖括号<>内的内容
    if (regcomp(&regex_1, pattern_1, REG_EXTENDED) != 0) {
        fprintf(stderr, "Failed to compile regex_1\n");
        return 1;
  }
  if (regcomp(&regex_2, pattern_2, REG_EXTENDED) != 0) {
      fprintf(stderr, "Failed to compile regex_2\n");
      return 1;
  }
  
  if (pipe(pipefd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
  }

  cpid = fork();
  if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
  }

  if (cpid != 0) {      //parent read pipe
      close(pipefd[1]);          /* Close unused write end */
      close(2); //close stderr
      cur = time(NULL);
      while (readline(pipefd[0], buffer, MAX_LINE) > 0) {
          if (strncmp(buffer, "exit_group", strlen("exit_group")) == 0)
              break;
          char *name = "";
          char time_read[15];
          // "("前内容匹配
          if (regexec(&regex_1, buffer, 2, matches1, 0) == 0) {
              size_t str_length = matches1[1].rm_eo - matches1[1].rm_so;
              name = (char *)malloc(str_length + 1);
              strncpy(name, buffer + matches1[1].rm_so, str_length);
              name[str_length] = '\0';
              // printf("%s", name);
              // putchar(' ');
          } else {
              fprintf(stderr,"No match for reg_1\n");
          }
          //<>内内容匹配
          if (regexec(&regex_2, buffer, 2, matches2, 0) == 0) {
              size_t str_length = matches2[1].rm_eo - matches2[1].rm_so;
              strncpy(time_read, buffer + matches2[1].rm_so, str_length);
              time_read[str_length] = '\0';
              float time_syscall = atof(time_read);
              record->time_total += time_syscall;
              // printf("time:%f\n", time_syscall);
              syscall_record(record, name, time_syscall);
              // for (size_t i = matches2[1].rm_so; i < matches2[1].rm_eo; i++)
              // {
              //     putchar(buffer[i]);
              // }
              // putchar('\n');
          } else {
              fprintf(stderr,"No match for reg_2\n");
          }
          if((time(NULL)-cur)>=1){
                printf("\033[2;1H");
                printf("\033[2J");
                fflush(stdout);
                cur = time(NULL);
                find_top5(record, canvas);
          }
      }
    //   write(STDOUT_FILENO, "\n", 1);
      find_top5(record,canvas);

      close(pipefd[0]);
      regfree(&regex_1);
      regfree(&regex_2);
      _exit(EXIT_SUCCESS);

  } else {            /* child write pipe */
      close(pipefd[0]);          /* Close unused read end */
      close(1);
      dup2(pipefd[1], 2);
      execve("/bin/strace", exec_argv, exec_envp);
      perror(argv[0]);
      exit(EXIT_FAILURE);
  }
  // execve("strace",          exec_argv, exec_envp);

  // execve("/usr/bin/strace", exec_argv, exec_envp);

}

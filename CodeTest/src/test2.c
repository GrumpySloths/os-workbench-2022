#include<stdarg.h>
#include<stdint.h>
#include<stdio.h>
#include<string.h>


int my_vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    //vsnprintf实现
    char c, *s;
    int d;
    uintptr_t pt;
    char pre;  // 代表前一个字符
    size_t count = strlen(fmt);
    size_t len = count;
    size_t i = 0; 
    //对count和n进行比较
    if(count>n){
        count=n;
    }
    while (count-- > 0) {
        if((pre=*fmt++)=='%'){
            switch(*fmt++){
                case 's': /* string */
                    s = va_arg(ap, char *);
                    while(*s){
                        out[i++]=*s++;
                    }
                    break;
                case 'd':              /* int */
                    d = va_arg(ap, int32_t);
                    char buf[100];
                    int j = 0;
                    if(d<0){
                        out[i++]='-';
                        d=-d;
                    }
                    if(d==0){
                        buf[j++]='0';
                    }
                    while(d){
                        buf[j++]=d%10+'0';
                        d/=10;
                    }
                    while(j--){
                        out[i++]=buf[j];
                    }
                    break;
                case 'c':              /* char */
                    /* need a cast here since va_arg only
                        takes fully promoted types */
                    c = (char) va_arg(ap, int);
                    out[i++]=c;
                    break;
                case 'p':
                    pt = va_arg(ap, uintptr_t);
                    char buf2[100];
                    int k = 0;
                    out[i++]='0';
                    out[i++]='x';
                    if(pt==0){
                        out[i++]='0';
                    }
                    while(pt){
                        if(pt%16>=10)
                            buf2[k++]=pt%16-10+'a';
                        else
                            buf2[k++]=pt%16+'0';
                        pt/=16;
                    }
                    while(k--){
                        out[i++]=buf2[k];
                    }
                    break;
                default:
                    printf("current behavior is not defined,program exited\n");
                }
        }else{
            out[i++]=pre;
        }
    }

    out[i]='\0';
    
    return len; 
}

int my_snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = my_vsnprintf(out, n, fmt, ap);
    va_end(ap);
    return ret;

}

void test_vsn_printf() {
    char buf[100];
    my_snprintf(buf, 10, "hello world %d %s %c %p", 123, "abc", 'd', (uintptr_t)0x12345678);
    printf("%s\n", buf);
}
int main(void) { test_vsn_printf(); return 0;}
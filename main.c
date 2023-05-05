#include "head.h"


int main(int argc, char *argv[])
{
    if(argc != 2) //需要输入开发板客户端id
    {
        printf("主函数传参 ./main2 开发板id\n");
        return -1;
    }
    if(strstr(argv[1],"arm") == NULL || argv[1][0] != 'a')
    {
        printf("id格式错误，正确格式：arm001\n");
        return -1;
    }
    pro_init();
    readtxt();    
    handle_pthread(argv[1]);
    pro_free();        
    
    return 0;
}
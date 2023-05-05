#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void *recfun(void *arg);
void ctrl_arm(char *argv);
void ctrl_all_arm(char *argv);
void ctrl_single_arm(char *argv);
int sfd;

int main(int argc,char **argv)
{
    if(argc != 2)
    {
        printf("主函数传参，./main id");
        return -1;
    }
    if(strstr(argv[1],"admin") == NULL || argv[1][0] != 'a')
    {
        printf("id格式错误，正确格式：admin001\n");
        return -1;
    }

    //获取套接字
    sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
    {
        perror("socket error\n");
        return -1;
    }
    //设置网络地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(51000);    
    addr.sin_addr.s_addr = inet_addr("47.120.2.40");
    //连接服务器
    int ret = connect(sfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret < 0)
    {
        perror("connect error\n");
        return -1;
    }

    //发送id号
    send(sfd,argv[1],strlen(argv[1]),0);

    pthread_t tid = pthread_create(&tid,NULL,recfun,(void*)&sfd);
    pthread_detach(tid);

    while(1)
    {   usleep(500000);
        printf("===============主界面===============\n");
        printf("==         1.查看所有客户端       ==\n");
        printf("==         2.控制广告机           ==\n");
        printf("==         3.退出                 ==\n");
        printf("====================================\n");
        int a;
        scanf("%d",&a);getchar();
        switch (a)
        {
        case 1:
           { char buf1[128] = {0};
            sprintf(buf1,"{from:%s,to:all_arm,name:check,status:null}",argv[1]);
            send(sfd,buf1,strlen(buf1),0);   sleep(1);
            break;}
        case 2:
            ctrl_arm(argv[1]);
            break;
        case 3:
            return 0;
        default:
            printf("输入错误\n");
            break;
        }
    }

}

//控制广告机选项
void ctrl_arm(char *argv)
{
    while (1)
    {
        
        printf("==============控制界面================\n");
        printf("==         1.控制所有广告机         ==\n");
        printf("==         2.控制单个广告机         ==\n");
        printf("==         3.退出                   ==\n");
        printf("======================================\n");
        int b;
        scanf("%d",&b);getchar();
        switch (b)
        {
        case 1:
            ctrl_all_arm(argv); 
            break;
        case 2:   
            ctrl_single_arm(argv);
            break;
        case 3:
            return ;
        default:
            printf("输入错误\n");
            break;
        }
    }
           
}

//控制所有广告机
void ctrl_all_arm(char *argv)
{
    while (1)
    {
        printf("==============全部修改===============\n");
        printf("==     1.修改所有广告机推送信息     ==\n");
        printf("==     2.修改所有广告机的城市天气   ==\n");
        printf("==     3.手动切换图片               ==\n");
        printf("==     4.自动切换图片               ==\n");
        printf("==     5.切换音乐                   ==\n");
        printf("==     6.退出                       ==\n");
        printf("======================================\n");
        int c;
        scanf("%d",&c);getchar();
        switch (c)
        {
        case 1:
            {char buf2[256] = {0};
            printf("输入推送内容：\n");
            scanf("%s",buf2);getchar();
            char buf3[300] = {0};
            sprintf(buf3,"{from:%s,to:all_arm,name:rolltext,status:%s}",argv,buf2);
            send(sfd,buf3,strlen(buf3),0);  sleep(1);
            break;}
        case 2:
            {char buf4[16] = {0};
            printf("输入要修改的城市:\n");           
            scanf("%s",buf4);getchar();
            char buf5[128] = {0};
            sprintf(buf5,"{from:%s,to:all_arm,name:city,status:%s}",argv,buf4);
            send(sfd,buf5,strlen(buf5),0);  sleep(1);
            break;}
        case 3:{printf("输入bmp格式图片名称\n");
                char buf11[32] = {0}; 
                scanf("%s",buf11);getchar();
                char buf12[128] = {0};
                sprintf(buf12,"{from:%s,to:all_arm,name:picPATH,status:%s}",argv,buf11);
                send(sfd,buf12,strlen(buf12),0);  sleep(1);
                break; }
            
        case 4:
                {char buf13[128] = {0};
                sprintf(buf13,"{from:%s,to:all_arm,name:picPATH,status:ATTO}",argv);
                send(sfd,buf13,strlen(buf13),0);  sleep(1);
                break; }
        case 5:
            {char mbuf[128] = {0};
            sprintf(mbuf,"{from:%s,to:all_arm,name:music,status:next}",argv);
            send(sfd,mbuf,strlen(mbuf),0);  sleep(1);
            break;}
        case 6:    return ;
        default:
            printf("输入错误\n");
            break;
        }
    }
    
}

//控制单个广告机
void ctrl_single_arm(char *argv)
{
    char buf6[128];
    sprintf(buf6,"{from:%s,to:all_arm,name:check,status:null}",argv);
    send(sfd,buf6,strlen(buf6),0);sleep(1);
    printf("请输入要修改的广告机id：\n");
    char armid[16] = {0};
    scanf("%s",armid);getchar();
    while (1)
    {   
        printf("===============单个修改===============\n");
        printf("==         1.修改推送信息           ==\n");
        printf("==         2.修改城市               ==\n");
        printf("==         3.手动切换图片           ==\n");
        printf("==         4.自动切换图片           ==\n");
        printf("==         5.切换音乐               ==\n");
        printf("==         6.退出                   ==\n");
        printf("======================================\n");
        int d;
        scanf("%d",&d);
        switch (d)
        {
        case 1:
            {printf("输入修改的信息：\n");
            char buf7[256] = {0};
            scanf("%s",buf7);getchar();
            char buf8[300] = {0};
            sprintf(buf8,"{from:%s,to:%s,name:rolltext,status:%s}",argv,armid,buf7);
            send(sfd,buf8,strlen(buf8),0);  sleep(1);
            break;}
        case 2:
            {printf("请输入要修改的城市：\n");
            char buf9[16] = {0};
            scanf("%s",buf9);getchar();
            char buf10[128] = {0};
            sprintf(buf10,"{from:%s,to:%s,name:city,status:%s}",argv,armid,buf9);
            send(sfd,buf10,strlen(buf10),0);    sleep(1);
            break;}
        case 3:
            {printf("输入bmp格式图片名称\n");
            char buf11[32] = {0}; 
            scanf("%s",buf11);getchar();
            char buf12[128] = {0};
            sprintf(buf12,"{from:%s,to:%s,name:picPATH,status:%s}",argv,armid,buf11);
            send(sfd,buf12,strlen(buf12),0);  sleep(1);
            break; }
            
        case 4:
            {char buf13[128] = {0};
            sprintf(buf13,"{from:%s,to:%s,name:picPATH,status:ATTO}",argv,armid);
            send(sfd,buf13,strlen(buf13),0);  sleep(1);
            break; }
        case 5:
            {char mbuf[128] = {0};
            sprintf(mbuf,"{from:%s,to:%s,name:music,status:next}",argv,armid);
            send(sfd,mbuf,strlen(mbuf),0);  sleep(1);
            break;}
        case 6:    return ;
        default:
            printf("输入错误\n");
            break;
        
        }
    }

}

//接收服务器信息线程
void *recfun(void *arg)
{
    int sfd = *((int *)arg);
    char buf[1024] = {0};
    while(1)
    {
        bzero(buf,1024);
        recv(sfd,buf,1024,0);
        printf("%s\n",buf);
    }
}

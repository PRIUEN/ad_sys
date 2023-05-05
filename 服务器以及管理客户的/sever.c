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

//管理员列表结构体
struct node
{
    char id[32];
    int sockfd;
    char pic_path[64];
    char rolltext[160];
    char location[64];
    struct node *next;
} *myhead;

//广告机链表
// struct arm_node
// {
//     char id[32];
//     int sockfd;
//     char pic_path[64];
//     char rolltext[160];
//     char location[64];
//     struct arm_node *next;
// } *adverhead;



//新建节点
struct node *newnode(char *a_id,int a_soc)
{
    struct node *head = malloc(sizeof(struct node));
    strcpy(head->id,a_id);
    strcpy(head->location, "广州市-从化区-城鳌大道");
    strcpy(head->pic_path, "ATTO");
    strcpy(head->rolltext, "计划是时间的最好保障，时间是效率的坚实基础，效率是行动的优化大师。");
    head->sockfd = a_soc;
    head->next = NULL;
    return head;
}

//头插节点
int add_node(struct node *head,struct node *newn)
{
    if(head == NULL || newn == NULL)  return -1;
    newn->next = head->next;
    head->next = newn;
}

//通过id查找节点
struct node *find_node_id(struct node *head,char *id)
{
    if(head == NULL || head->next == NULL) return NULL;
    head = head->next; //移动到下一个
    while(head){
        if(strcmp(head->id, id) == 0){
            return head;
        }
        head = head->next;
    }
    return NULL;
}

//通过套接字查找
struct node *find_node_socketid(struct node *head,int fd)
{
    if(head == NULL || head->next == NULL)  return NULL;
    struct node *p = head->next;
    while (p != NULL)
    {
        if(p->sockfd == fd)  break; 
        p = p->next;
    }
    return p;
}

void *run_client(void *arg);
void *get_time(void *arg);  //获取时间


int main()
{
    //初始化头节点
    myhead = newnode(" ",-1);

    //创建tcp通信监听套接字
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
    {
        perror("socket error");
        return -1;
    }

    //绑定地址
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(51000);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(sfd,(struct sockaddr *)&saddr,sizeof(saddr));
    if(ret < 0)
    {
        perror("bind error\n");
        return -1;
    }

    ret = listen(sfd,5);
    if(ret < 0)
    {
        perror("listen error\n");
        return -1;
    }

    while(1)
    {
        //接受连接
        int cfd = accept(sfd,NULL,NULL);
        if(cfd < 0)
        {
            perror("accept error\n");
            return -1;
        }

        //创建线程
        pthread_t tid;
        pthread_create(&tid,NULL,run_client,(void *)&cfd);
        pthread_detach(tid);

    }

    return 0;
}


// 处理客户端和通信的函数
void *run_client(void *arg)
{
    int cfd = *((int *)arg);
    //读取保存id
    char id_buf[32] = {0};
    int size = recv(cfd,id_buf,32,0);
    if(size <= 0)
    {
        close(cfd);
        return NULL;
    }

    //判断id格式
    if(id_buf[0] != 'a')
    {
        char *p3 = "id格式错误，正确格式：arm001或admin001";
        write(cfd,p3,strlen(p3));
        pthread_exit(NULL);
    }
    if(strstr(id_buf,"arm") != NULL)
        printf("广告机客户端加入,ID:%s\n",id_buf);  
    else if(strstr(id_buf,"admin") != NULL)
        printf("管理客户端加入,ID:%s\n",id_buf);
    else
    {
        close(cfd);
        return NULL;
    }

    //添加id到链表
    struct node *tmp = find_node_id(myhead,id_buf);
    if(tmp == NULL)  //链表中没有就添加
    {
        tmp = newnode(id_buf,cfd);
        add_node(myhead,tmp);
    }
    else  //有就更新套接字
    {
        tmp->sockfd = cfd;
    }
    // if(strstr(id_buf,"arm") != NULL)
    // {
    //     char locat[64] = {0};
    //     char buf[254] = {0};
    //     char location_buf[300] = {0};
    //     int ret = recv(cfd,location_buf,strlen(location_buf),0);
    //     if(ret < 0)  {printf("接受初始信息错误\n");}
    //     sscanf(location_buf,"%s#%s",locat,buf);
    //     strcpy(tmp->location,locat);
    //     strcpy(tmp->rolltext,buf);
    // }

    char *p7 = "连接成功\n";
    send(cfd,p7,strlen(p7),0);
    

    while (1)
    {
        //接收数据  {from:phone00001,to:arm000001,name:led,status:1} 
        char revbuf[128] = {0};
        int s = recv(cfd,revbuf,128,0);
        if(s <= 0)
        {
            printf("客户端%s离线\n",tmp->id);
            tmp->sockfd = -1;
            break; 
        }

        char app_id[16] = {0};
        char arm_id[16] = {0};
        char name[16] = {0};
        char status[256] = {0};

        //判断信息是否符合格式
        if(revbuf[0] != '{' || revbuf[strlen(revbuf)-1] != '}')
        {
            char *p = "输入错误，请重新输入";
            write(cfd, p, strlen(p));
            continue;
        }
        
        //解析字符串
        sscanf(revbuf,"{from:%[^,],to:%[^,],name:%[^,],status:%[^}]}", app_id, arm_id, name, status);
        if(find_node_id(myhead,app_id) == NULL)
        {
            printf("%s\n",app_id);
            char *p1 = "你的id不正确，请确认后再发送";
            write(cfd,p1,strlen(p1));
            continue;
        }

        //发送给所有arm客户端
        if(strcmp(arm_id,"all_arm") == 0)  
        {
            //查看所有客户端在线情况
            if(strcmp(name,"check") == 0)  
            {
                char Atime[128] = {0};
                strcpy(Atime,(char *)get_time(NULL));
                send(cfd,Atime,strlen(Atime),0);

                char all_info[1024] = {0};
                char admin_info[512] = {"\n"};
                //遍历客户端链表
                struct node *p5 = myhead->next;
                while(p5 != NULL)
                {
                    char detailed_info[500] = {0};           
                    char type[32] = {0};
                    char status[16] = {0};
                    //格式化写入
                    if(strstr(p5->id,"arm") != NULL)                 
                    {
                        strcpy(type,"广告机客户端");
                        if(p5->sockfd < 0)  strcpy(status,"离线");
                        if(p5->sockfd > 0)  strcpy(status,"在线");

                        sprintf(detailed_info,"%sid：%-8s  %-8s  地点：%-16s  图片：%-12s\n滚动信息：%s\n",type, p5->id, status, p5->location, p5->pic_path,p5->rolltext);
                        //插入总信息数组
                        strcat(all_info,detailed_info);
                    }

                    else if(strstr(p5->id,"admin") != NULL)  
                    {
                        strcpy(type,"管理员客户端");
                        if(p5->sockfd < 0)  strcpy(status,"离线");
                        if(p5->sockfd > 0)  strcpy(status,"在线");

                        sprintf(detailed_info,"%sid：%-12s  %-8s \n",type, p5->id, status);
                        //插入总信息数组
                        strcat(admin_info,detailed_info);
                    }
                    p5 = p5->next;
                }
                strcat(all_info,admin_info);
                
                //printf("all_info:%s\n",all_info);
                send(cfd, all_info, strlen(all_info), 0);
            }
            //发送普通指令
            else
            {
                char to_all_arm[128] = {0};
                sprintf(to_all_arm,"{from:%s,name:%s,status:%s}",app_id,name,status);
                struct node *p4 = myhead->next;
                while(p4 != NULL )
                {
                    if(strstr(p4->id,"arm") != NULL)
                    {
                        send(p4->sockfd, to_all_arm, strlen(to_all_arm), 0);  
                        //修改链表中的信息
                        if(strcmp(name, "picPATH") == 0)    strcpy(p4->pic_path, status);
                        else if(strcmp(name, "rolltext") == 0)    strcpy(p4->rolltext,status);

                    }
                        
                    p4 = p4->next;
                }
            }
            
        }

        //单个发送指令
        else
        {
            //查找arm_id的套接字
            struct node *arm_node = find_node_id(myhead,arm_id);
            if(arm_node == NULL)
            {
                char *p2 = "该设备端不存在，请确认后再发送";
                write(cfd,p2,strlen(p2));
                continue;
            }
            if(arm_node->sockfd < 0)
            {
                char *p6 = "该客户端已离线";
                send(cfd, p6, strlen(p6),0);
                continue;
            }
            
            //发送数据给设备端
       
            int to_sockfd = arm_node->sockfd;
            char to_buf[128] = {0};
            sprintf(to_buf,"{from:%s,name:%s,status:%s}",app_id,name,status);
            send(to_sockfd,to_buf,strlen(to_buf),0);   
            //修改链表中的信息
            if(strcmp(name, "picPATH") == 0)    strcpy(arm_node->pic_path, status);
            else if(strcmp(name, "rolltext") == 0)    strcpy(arm_node->rolltext,status);         
        }
        printf("%s-%s-%s-%s\n",app_id,arm_id,name,status);

    }
    
    close(cfd);
}

 //获取时间和天气
void *get_time(void *arg) 
{
    time_t rawtime;
    struct tm *time_info;  //时间信息结构体
    //时间
    char today   [32] = {0};
    char now_time[32] = {0};
    char week    [32] = {0};

    time( &rawtime );

    time_info = localtime( &rawtime );
    //获取时间
    strftime(today,    32,"%F", time_info);
    strftime(now_time, 32,"%T", time_info);
    strftime(week,     32,"%a", time_info);

    //把英文缩写转换成中文
    if(strcmp(week,"Mon" ) == 0)   {  bzero(week,32);  strcpy(week, "星期一" );  } 
    if(strcmp(week,"Tue" ) == 0)   {  bzero(week,32);  strcpy(week, "星期二" );  } 
    if(strcmp(week,"Wed" ) == 0)   {  bzero(week,32);  strcpy(week, "星期三" );  } 
    if(strcmp(week,"Thu" ) == 0)   {  bzero(week,32);  strcpy(week, "星期四" );  } 
    if(strcmp(week,"Fri" ) == 0)   {  bzero(week,32);  strcpy(week, "星期五" );  } 
    if(strcmp(week,"Sat" ) == 0)   {  bzero(week,32);  strcpy(week, "星期六" );  } 
    if(strcmp(week,"Sun" ) == 0)   {  bzero(week,32);  strcpy(week, "星期天" );  }  
    
    //printf("%s  %s  %s\n", today,now_time,week );
    char now_time_all[128];
    //拼接时间字符串打印到屏幕
    sprintf(now_time_all,"%s-%s-%s",today,week,now_time);
    char *n = now_time_all;
    return (void *)n;
}
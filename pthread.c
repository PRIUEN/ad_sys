#include "head.h"



//图片路径
char paths[8][128] = {"/root/myuser/photo/messi.bmp", "/root/myuser/photo/snow.bmp",
                      "/root/myuser/photo/firegoose.bmp", "/root/myuser/photo/spy.bmp",
                      "/root/myuser/photo/qinghai.bmp", "/root/myuser/photo/food.bmp",
                      "/root/myuser/photo/maserati.bmp", "/root/myuser/photo/mi.bmp"};
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

//创建线程函数
void handle_pthread(char *arg)
{
    printf("id:%s\n",arg);
    pthread_create(&bmp_tid,NULL,call_show_bmp,NULL);
    pthread_create(&time_tid,NULL,call_show_time,NULL);
    pthread_create(&read_tid,NULL,call_readtxt,NULL);
    pthread_create(&weather_tid,NULL,call_show_weather,NULL);
    pthread_create(&rolltxt_tid,NULL,call_show_rolltxt,NULL);
    pthread_create(&music_tid,NULL,call_music_dis,NULL);
    pthread_create(&sever_tid,NULL,connect_sever,(void *)arg);
    pthread_join(bmp_tid,NULL);
    pthread_join(time_tid,NULL);
    pthread_join(read_tid,NULL);
    pthread_join(weather_tid,NULL);
    pthread_join(rolltxt_tid,NULL);
    pthread_join(music_tid,NULL);
    pthread_join(sever_tid,NULL);

}


//调用图片显示函数
void *call_show_bmp(void *arg)
{
    pic_flag = 0; 
    int flag = 0;
    while(1)
    {
        if(pic_flag == 0)
        {
            show_bmp(pic.x, pic.y, pic.w, pic.h, paths[flag]); 
            flag++;
            sleep(5);
            if(flag == 8)
            {
                flag = 0; 
            } 
        }      
        sleep(1);
    }

    //printf("图片线程id:%d\n",bmp_tid);
    pthread_exit(0);
}

//调用时间显示函数
void *call_show_time(void *arg)
{
    while(1)
    {
        show_time();  
        usleep(30000);     
    }
    pthread_exit(0);
}

//调用读取配置文件函数
void *call_readtxt(void *arg)
{
    while(1)
    {
        readtxt();
        sleep(2);
    }
    pthread_exit(0);
}

//调用显示天气函数
void *call_show_weather(void *arg)
{
    strcpy(CITY,"广州");
    while(1)
    {   //判断城市有没有被修改
        show_weather();
        char city_backup[16] = {0};
        strcpy(city_backup,CITY);
        while(1)
        {   
            if(strcmp(city_backup,CITY) != 0)  
                break;
            sleep(1);
        }
        
    }
    pthread_exit(0);
}

//调用显示滚动字幕函数
void *call_show_rolltxt(void *arg)
{
    show_rolltxt();
    pthread_exit(0);
}

//放音乐
void *call_music_dis(void *arg)
{
    int a = 0;  
    
    while (1)
    {
        strcpy(music_flag,"m1");  //初始化标志位
        char mbuf[4] = {0};
        strcpy(mbuf,music_flag);
        music_dis(a);
        while (1)
        {
            if(strcmp(mbuf,music_flag) != 0)  //判断标志位有无修改
            {
                system("killall -9 mplayer");
                if(a == 0)  a = 1;
                else a = 0;
                bzero(music_flag,4);   //重置标志位
                break;
            }
            sleep(1);
        }
        
        
    }
    
    pthread_exit(0);    
}

//连接服务器
void *connect_sever(void *arg)
{   
    //创建套接字
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    if(sfd < 0)
    {
        perror("socket error\n");
        return NULL;
    }
    //设置网络地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(51000);   //服务器端口
    addr.sin_addr.s_addr = inet_addr("47.120.2.40");  //服务器IP 47.120.2.40
    //连接服务器
    int ret = connect(sfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret < 0)
    {
        perror("connect error\n");
        return NULL;
    }
    //给服务器发送本机id
    sleep(1);
    char this_id[16] = {0};
    strcpy(this_id,(char *)arg);
    send(sfd,this_id,strlen(this_id),0);
    // usleep(600000);
    // //发送配置信息
    // char location[300] = {0};
    // sprintf(location,"广州市-从化区-城鳌大道#%s",textbuffer);
    // send(sfd,location,strlen(location),0);


    //等待服务器发送数据  {from:%s,name:%s,status:%s}
    char buf1[300] = {0};
    while(1)
    {
        bzero(buf1,300);
        recv(sfd,buf1,300,0);
        //printf("%s\n",buf1);
        char app_id[16] = {0};
        char name[16] = {0};
        char status[256] = {0};
        //解析字符串
        sscanf(buf1,"{from:%[^,],name:%[^,],status:%[^}]}",app_id,name,status);

        if(strstr(name,"rolltext") != NULL) //修改滚动字幕
        {
            bzero(textbuffer,256);
            //printf("{from:%s,name:%s,status:%s}\n",app_id,name,status);
            strcpy(textbuffer,status);
        }
        else if(strstr(name,"city") != NULL) //修改城市天气
        {
            strcpy(CITY,status);
            printf("CITY修改成%s\n",CITY);
        }
        else if(strstr(name,"music") != NULL) //切换部分音乐
        {
            strcpy(music_flag,"m2");
            printf("音乐切换\n");
        }
        else if(strstr(name,"picPATH") != NULL) //切换图片
        {
            if (strcmp(status,"ATTO") == 0) //自动轮播
            {
                pic_flag = 0;
            }
            else //手动切换
            {
                pic_flag = 1; 
                pthread_mutex_lock(&m);
                strcpy(pic_path,status);
                char pathbuf[64] = {0};
                sprintf(pathbuf,"/root/myuser/photo/%s",pic_path);           
                int ret = show_bmp(pic.x, pic.y, pic.w, pic.h, pathbuf); 
                
                pthread_mutex_unlock(&m);

            }  
        }
    }

    close(sfd);
}

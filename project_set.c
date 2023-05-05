#include "head.h"

int pro_init()
{
    //打开屏幕
    lcdfd = open("/dev/fb0",O_RDWR);
    if(lcdfd == -1)
    {
        perror("打开屏幕失败\n");
        return -1;
    }

    //屏幕内存映射
    lcdp = mmap(NULL,800*480*4,PROT_READ | PROT_WRITE ,MAP_SHARED,lcdfd,0);
    if(lcdp == MAP_FAILED)
    {
        perror("内存映射失败\n");
        return -1;
    }
    //填充间隙
    // dis_font(" ",600,0,10,400,0x4682B400,0);
    // dis_font(" ",610,160,190,10,0x4682B400,0);
    dis_font(" ",0,400,800,9,0x4682B400,0);


    return 0;
}

//显示图片
int show_bmp(int x,int y,int w,int h,char *path)
{
    int bmpfd = open(path,O_RDWR);
    if(bmpfd == -1)
    {
        perror("打开图片失败\n");
        return -1;
    }
    //读取图片信息
    int bw,bh;
    lseek(bmpfd,18,SEEK_SET);
    read(bmpfd,&bw,4);
    read(bmpfd,&bh,4);
    //printf("w=%d,h=%d\n",bw,bh);

    char bbuf[bw*bh*3];
    lseek(bmpfd,54,SEEK_SET);
    //read(bmpfd,bbuf,bw*bh*3);
    int rubbish = (4 - (bw * 3) % 4) % 4;
    //跳过垃圾数
    for(int i=0; i<bh; i++)
    {
        read(bmpfd,&bbuf[bw*3*i],bw*3);
        lseek(bmpfd,rubbish,SEEK_CUR);
    }

    if(w == -1 && h == -1)
    {
        w = bw;
        h = bh;
    }
    //放大或缩小的比值
    int reth = bh*100 / h;    //800:600 = 4:3
    int retw = bw*100 / w;    //480:400 = 6:5
    //将原图片放大或缩小后的像素存放到新的数组，过程中反转y轴
    char temp[w*h*3];
    for(int j=0; j<h; j++)
    {
        for(int k=0; k<w; k++)
        {
            temp[((h-1-j)*w+k)*3+0] = bbuf[(j*reth/100)*bw*3 + (k*retw/100)*3+0];               
            temp[((h-1-j)*w+k)*3+1] = bbuf[(j*reth/100)*bw*3 + (k*retw/100)*3+1];               
            temp[((h-1-j)*w+k)*3+2] = bbuf[(j*reth/100)*bw*3 + (k*retw/100)*3+2];               
        }
    }
    //内存映射写入到lcd文件
    int num = 0;
    int *newp = lcdp + 800*y+x;
    for(int y=0,n=0; y<h; y++)
    {
        for(int x=0; x<w; x++,n+=3)
        {           
            newp[800*y+x] = temp[n] | temp[n+1] << 8 | temp[n+2] << 16;
        }
    }

    close(bmpfd);
    return 0;
}

//显示时间
int show_time()
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
    sprintf(now_time_all,"%s\n%s\n%s",today,week,now_time);
   
    dis_font(now_time_all, tim.x, tim.y, tim.w, tim.h, tim.bgc, tim.foc);  
    

    return 0;
}

/*
    通过心知天气的api获取
    指定地点的天气信息
*/
//获取并显示天气
int show_weather()
{
    //1.创建套接字， 连接服务器， 接收数据， 发送数据，关闭套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket");
        return -1;
    }

    //2链接服务器
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET; //地址族
    saddr.sin_port = htons(80); //端口号---网络字节序
    saddr.sin_addr.s_addr = inet_addr("116.62.81.138"); //服务器地址（把字符串ip转网络字节序整型数据）
    int ret = connect(sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(ret < 0){
        perror("connect");
    }

    //发送HTTP请求头   
    char reqHeader[256];
    sprintf(reqHeader,"GET /v3/weather/now.json?key=SWDxOhBoPJ6YGKxqF&location=%s&language=zh-Hans&unit=c HTTP/1.1\r\nHost:api.seniverse.com\r\n\r\n",CITY);
    send(sockfd,reqHeader, strlen(reqHeader), 0);

    //收发数据
    char city_weather[128];
    while(1)
    {
        char text[2048]={0};
        int ret = recv(sockfd, text, 2048, 0);
        if(ret <= 0){
            perror("recv error\n");
            break;
        }//printf("%s\n",text);

        //json解析
        char* p = strstr(text,"{");//协议头结束
        if(p != NULL)
        {
            //printf("%s\n", p);
            char buffer[1024] = {0};
            strcpy(buffer, p);
            //json解析
            cJSON *root  = cJSON_Parse(buffer);
            if(root == NULL) break;
            cJSON* array = cJSON_GetObjectItem(root,"results");   
            //获取数组中的第0个元素
            cJSON* oneObj = cJSON_GetArrayItem(array,0);
            if(oneObj == NULL) break;  

            cJSON* nowObj = cJSON_GetObjectItem(oneObj,"now");
            cJSON* tempObj =  cJSON_GetObjectItem(nowObj,"temperature");
            cJSON* textObj =  cJSON_GetObjectItem(nowObj,"text");   
            //printf("广州今天温度:%s℃ %s\n", tempObj->valuestring,textObj->valuestring);
          
            
            sprintf(city_weather,"%s\n%s\n温度:%s\n", CITY, textObj->valuestring, tempObj->valuestring);       
            dis_font(city_weather, weather.x, weather.y, weather.w, weather.h, weather.bgc, weather.foc); 
            
            printf("%s\n",city_weather);
            //释放json对象       
            cJSON_Delete(root);
        }     
    }
    close(sockfd);
}


//显示滚动字幕
int show_rolltxt()
{   
    strcpy(textbuffer, "计划是时间的最好保障，时间是效率的坚实基础，效率是行动的优化大师。         ");
    //printf("%s\n",textbuf);                   
    int x = rolltext.x;
    int i = 0;
    while(1)
    {  
        dis_font(textbuffer, x, rolltext.y, rolltext.w, rolltext.h, rolltext.bgc, rolltext.foc);
        if(x <= 0) x = 799;        
        x--;
    }
    
}


//播放音乐
int music_dis(int a)
{
    if(a == 0)
        system("mplayer -slave -quiet -af volume=-10 Experience.mp3 &");
    else if(a == 1)
        system("mplayer -slave -quiet -af volume=-10 AMomentApart.mp3 &");
    return 0;
}


//项目释放
int pro_free()
{
    
    close(lcdfd);
    munmap(lcdp,800*480*4);
    return 0;
}
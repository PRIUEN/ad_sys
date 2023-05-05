#include "head.h"



int readtxt()
{   
    FILE *fp;     //文件指针
    //打开配置文件
    fp = fopen("config.txt","r+");
    if(fp == NULL)
    {
        perror("打开txt失败\n");
        return -1;
    }
    //char config[4][64];
    //初步读取配置信息到config
    for(int i=0; i<4; i++)
    {   
        char buf[256] = {0};
        char *p = NULL;
        fgets(buf,256,fp);
        p = strtok(buf,"#");
        strcpy(config[i],p);
        //printf("config[%d] %s\n",i,config[i]);
    }
    //从config分割具体信息
    sscanf(config[0],"%d,%d,%d,%d,%s ",&pic.x,&pic.y,&pic.w,&pic.h,pic.path); 

    sscanf(config[1],"%d,%d,%d,%d,%x,%x ",&tim.x,&tim.y,&tim.w,&tim.h,&tim.bgc,&tim.foc);

    sscanf(config[2],"%d,%d,%d,%d,%x,%x ",&weather.x,&weather.y,&weather.w,&weather.h,&weather.bgc,&weather.foc);

    sscanf(config[3],"%d,%d,%d,%d,%x,%x ",&rolltext.x,&rolltext.y,&rolltext.w,&rolltext.h,&rolltext.bgc,&rolltext.foc);

    // printf("%d,%d,%d,%d,%s \n",pic.x,pic.y,pic.w,pic.h,pic.path);
    // printf("tme:%d,%d,%d,%d,%#x,%#x\n",tim.x,tim.y,tim.w,tim.h,tim.bgc,tim.foc);    
    // printf("weather:%d,%d,%d,%d,%#x,%#x\n",weather.x,weather.y,weather.w,weather.h,weather.bgc,weather.foc);
    // printf("%d,%d,%d,%d,%#x,%#x\n",rolltext.x,rolltext.y,rolltext.w,rolltext.h,rolltext.bgc,rolltext.foc);
    



    fclose(fp);
    return 0;
}
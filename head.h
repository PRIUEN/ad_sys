#ifndef _HEAD_H
#define _HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "font.h"
#include "cJSON.h"


//图片信息结构体
struct picture
{
    int x,y,w,h;
    char path[64];
} pic;

//字体信息结构体
struct font
{
    int x,y,w,h;
    int bgc;     //背景颜色
    int foc;     //字体颜色
} ;

struct font tim;  //时间
struct font weather;  //天气
struct font rolltext;  //滚动字体
char textbuffer[256];   //滚动字幕内容
char CITY[32];  //城市
char music_flag[4];
int pic_flag;
char pic_path[32];


//存放总的配置文件信息
char config[4][128];


int lcdfd;    //lcd文件描述符
int touchfd;  //触摸屏文件描述符
int *lcdp;    //屏幕内存映射指针



//各线程tid  图片    时间      天气           字幕      读取配置     音乐     服务器连接
pthread_t bmp_tid, time_tid, weather_tid, rolltxt_tid, read_tid, music_tid, sever_tid;

/*========================== readtxt.c =============================*/
int readtxt();

/*======================== dis_font.c ============================*/
int dis_font(char *buf,int fx,int fy,int bord_w,int bord_h,int bcolor,int fcolor);
int dis_font2(char *buf,int fx,int fy,int bord_w,int bord_h,int bcolor,int fcolor);
void get_lcd_background(bitmap *bm, int x, int y, int *lcd);

/*======================= project_set.c =============================*/
int pro_init();
int show_bmp(int x,int y,int w,int h,char *path);
int show_time();
int show_weather();
int show_rolltxt();
int music_dis(int a);
int pro_free();

/*======================= pthread.c ===========================*/
void handle_pthread(char *arg);
void *call_show_bmp(void *arg);
void *call_show_time(void *arg);
void *call_readtxt(void *arg);
void *call_show_weather(void *arg);
void *call_show_rolltxt(void *arg);
void *call_music_dis(void *arg);
void *connect_sever(void *arg);

#endif
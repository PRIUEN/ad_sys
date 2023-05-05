#include "head.h"


void get_lcd_background(bitmap *bm, int x, int y, int *lcd)
{
        //提取背景 0,0,240,80区域内容填充到bm上
    unsigned int *p = ((unsigned int *)lcd)+x+y*800;
    for(int i=0; i<bm->height; i++)
    {
        memcpy(bm->map+i*bm->width*4, p, bm->width*4);
        p += 800;
    }
}

//文字显示
int dis_font(char *buf,int fx,int fy,int bord_w,int bord_h,int bcolor,int fcolor)
{		
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,30);  //字体大小20，可以人为修改
	
	
	//创建一个画板（点阵图）宽是200个像素，高是50个像素
	bitmap *bm = createBitmapWithInit(bord_w,bord_h,4,bcolor); //也可使用createBitmapWithInit函数，改变画板颜色
	//bitmap *bm = createBitmap(240, 80, 3);
	
	
	//将字体写到点阵图上 0,0表示汉字在画板的左上角显示          
	fontPrint(f,bm,10,10,buf,fcolor,0);
	                             // A  B  G R
	
	
	//把字体框输出到LCD屏幕上  参数0,0表示画板显示的坐标位置
	show_font_to_lcd(lcdp,fx,fy,bm);

	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
	return 0;
}


//带背景文字显示
int dis_font2(char *buf,int fx,int fy,int bord_w,int bord_h,int bcolor,int fcolor)
{		
	//打开字体	
	font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
	  
	//字体大小的设置
	fontSetSize(f,30);  //字体大小20，可以人为修改
	
	
	//创建一个画板（点阵图）宽是200个像素，高是50个像素
	bitmap *bm = createBitmapWithInit(bord_w,bord_h,4,bcolor); //也可使用createBitmapWithInit函数，改变画板颜色
	//bitmap *bm = createBitmap(240, 80, 3);

	unsigned char backbm[bord_w*bord_h*4];
	get_lcd_background(bm,0,70,lcdp);
	memcpy(backbm,bm->map, bord_w*bord_h*4);
	
	//将字体写到点阵图上 0,0表示汉字在画板的左上角显示
	fontPrint(f,bm,fx,fy,buf,fcolor,0);
	//把字体框输出到LCD屏幕上  参数0,0表示画板显示的坐标位置
	show_font_to_lcd(lcdp,0,70,bm);




	
	//关闭字体，关闭画板
	fontUnload(f);
	destroyBitmap(bm);
	return 0;
}


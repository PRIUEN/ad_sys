# ad_sys
物联网广告机管理系统
项目主要是基于gec6818开发板实现三端通信，管理员端，服务器端，广告机端。
管理员通过指定命令发送到服务器查看广告机工作状态并控制一个或多个广告机显示内容，
服务器接收并判断指令是否符合格式，再转发给对应的一个或多个广告机，广告机可显示时间，
天气，轮播图片，推送消息等，可接收服务器的指令修改上述内容，实现一对多的远程管理系统。
         项目主要用到多线程编程，套接字通信，TCP协议网络通信，文件IO，图片显示算法等。
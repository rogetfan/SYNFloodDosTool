# Inherited From WayOSGuessTool(deleted)

<del>个人项目，主要用来破解公司傻逼维盟路由器密码</del>

<del>虽说是个人项目，但是不反对人加入</del>

现在主要不破解了,直接DOS攻击破坏

这个版本第一版的代码是通过大量的进程和线程用三次握手之后的`TCP`连接去发送大量数据来攻击

但是攻击效果不明显，最高时候`100M`的路由器(其实也是交换机),`1000M`的电脑网卡只能产生接近`2M/s`的流量

于是这里采用了`SYN Flood`攻击的方式，放弃接收三次握手的第二阶段的握手包，或者接收了不发送第三个确认包

最高接近`8M/s`

<2017/11/28>

攻击时候加入了随机ip和随机端口的设置

还有自动间断攻击的方法

编辑`crontab -e`加入间隔启动代码

结束程序执行脚本在`script`里面

# Complie

Into the Dictoary and excute the `make` command
It will complie a excuted file named `tool`

直接在目录执行`make`就会产生一个`tool`的可执行文件

```
Usage : ./tool
      : ./tool -r -d
      -r   Use the random user name(default use the admin as user name)
      -d   Debug mod
      -t   Attack mod(this mod will not guess the correct password)
```
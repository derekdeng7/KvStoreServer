# linux系统相关

## Linux重要指令
### ipcs
  提供关于一些进程间通信方式的信息，包括共享内存，消息队列，信号。
  * Ipcs -a 默认输出信息，打印出所有进程间通信方式的信息；
  * Ipcs -q 输出使用消息队列进行进程间通信的信息；
  * Ipcs -s 输出使用信号进行进程间通信的信息；
  * Ipcs -m 输出使用共享内存进行进程间通信的信息；
  * Ipcs -t 输出信息的时间详细变化；
  * Ipcs -p 输出ipc方式的进程id；
  * Ipcs -c 输出ipc方式的创建者；
  * Ipcs -u 输出ipc方式的各种状态；
  
### ipcrm
  移除一个消息对象。或者共享内存段，或者一个信号集，同时会将与ipc对象相关链的数据也一起移除。只有超级管理员，或者ipc对象的创建者才有这项权利。
  * Ipcrm -M shmkey 移除使用shmkey创建的共享内存段；
  * Ipcrm -m shmid 移除使用shmid标识的共享内存段；
  * Ipcrm -Q msgkey 移除使用msgkey创建的消息队列；
  * Ipcrm -q msgid 移除使用msgid标识的消息队列；
  * Ipcrm -S semkey 移除使用semkey创建的信号；
  * Ipcrm -s semid 移除使用semid标识的信号。
  
### netstat
  用于列出系统上所有网络套接字的连接情况，包括tcp/udp/unix套接字，还能列出处于监听状态/等待请求的套接字。
  * netstat –a 上述命令列出 tcp, udp 和 unix 协议下所有套接字的所有连接；
  * netstat –at/-au 只列出tcp/udp连接（包括ipv4/ipv6）；
  * netstat -n 禁用反向域名解析，加快查询速度（只显示ip，不显示主机名），如netstat -ant；
  * netstat -l 列出监听套接字的连接;
  * netstat -p 获取进程名，进程号和用户id，-ep同时查看进程名和用户名;
  * netstat -s 打印出网络统计数据，包括某个协议下的收发包数量，若需要打印tcp/udp的数据包，则加-t/-u选项；
  * netstat -r 打印内核路由信息；
  * netstat -s 打印网络接口信息；
  * netstat -i 打印网络接口信息，-ei跟ipconfig一样；
  * netstat -c 持续输出； 
  * netstat -atnp | grep ESTA：意思是列出active状态（ESTABLISHED）的tcp连接，配合watch命令监视active状态的连接：watch-d -n0 "netstat -atnp | grep ESTA"
  * netstat -aple | grep ntp 查看服务是否在运行（ntp是网络时间协议，用于保证机器时间一致。不只是服务器，Windowd等桌面大多也有用。ntpd 是提供这个服务的守护进程）。
  
### tcpdump
  根据使用者的定义对网络上的数据包进行截获的包分析工具。 tcpdump可以将网络中传送的数据包的“头”完全截获下来提供分析。它支持针对网络层、协议、主机、网络或端口的过滤，并提供and、or、not等逻辑语句来帮助你去掉无用的信息。如果选用明文传送内容，还可以对截获内容查看。
  * tcpdump 普通情况下，直接启动tcpdump将监视第一个网络接口上所有流过的数据包
  * tcpdump -i eth1 监视指定接口的数据包
  * tcpdump host sundown 截获所有进入或离开sundown的数据包
  * tcpdump host 210.27.48.1 截获所有210.27.48.1 的主机收到的和发出的所有的数据包
  * tcpdump host helios and hotoracehotorace 打印helios 与 hot 或者与 ace 之间通信的数据包
  * tcpdump host 210.27.48.1 and \ (210.27.48.2 or 210.27.48.3 \) 截获主机210.27.48.1 和主机210.27.48.2 或210.27.48.3的通信
  * tcpdump ip host ace and not helios 打印ace与任何其他主机之间通信的IP 数据包, 但不包括与helios之间的数据包.
  * tcpdump ip host 210.27.48.1 and ! 210.27.48.2 获取主机210.27.48.1除了和主机210.27.48.2之外所有主机通信的ip包
  * tcpdump -i eth0 src host hostname 截获主机hostname发送的所有数据
  * tcpdump -i eth0 dst host hostname 监视所有送到主机hostname的数据包
  * tcpdump tcp port 23 and host 210.27.48.1 获取主机210.27.48.1接收或发出的tcp包
  * tcpdump udp port 123 对本机的udp 123 端口进行监视 
  
  
  
  
  

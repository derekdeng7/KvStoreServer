# Linux系统相关
<br>

## [ipc进程间通信/同步机制（无名管道、FIFO命名管道、消息队列、共享内存、socket）](https://www.jianshu.com/p/21fba1542026)
<br>

## 线程同步机制
### 临界区
　　保证在某一时刻只有一个线程能访问数据的简便办法。在任意时刻只允许一个线程对共享资源进行访问。
### 互斥量（mutex）
　　只有拥有互斥量的线程才具有访问资源的权限，由于mutex只有一个，因此就决定了任何情况下共享资源不会同时被多个线程所访问。
### 信号量（semaphore）
　　semaphore允许多个线程在同一时刻访问同一资源，指出了同时访问共享资源的线程最大数目，mutex可以说是semaphore在仅取值0/1时的特例。
### 条件变量（condition variable）
　　cv是用来等待而不是用来上锁的。cv以原子的方式阻塞进程，直到某特殊情况发生为止。条件变量始终与互斥锁一起使用。
### 锁（lock）
  * 读写锁（reader-writer lock）：允许多个线程同时读共享数据，而对写操作是互斥的。
  * 自旋锁（spin lock）与mutex类似，都是为了保护共享资源。mutex是当资源被占用，申请者进入睡眠状态；而自旋锁则循环检测保持者是否已经释放锁。
<br>

## Linux操作系统
### 守护进程
  * Linux Daemon（守护进程）是运行在后台的一种特殊进程。它独立于控制终端并且周期性地执行某种任务或等待处理某些发生的事件。Linux系统的大多数服务器就是通过守护进程实现的。常见的守护进程包括系统日志进程syslogd、 web服务器httpd、邮件服务器sendmail和数据库服务器mysqld等。
  * 守护进程一般在系统启动时开始运行，除非强行终止，否则直到系统关机都保持运行。守护进程经常以超级用户（root）权限运行，因为它们要使用特殊的端口（1-1024）或访问某些特殊的资源。
  * 一个守护进程的父进程是init进程，因为它真正的父进程在fork出子进程后就先于子进程exit退出了，所以它是一个由init继承的孤儿进程。
  * 守护进程是非交互式程序，它不需要用户输入就能运行，其名称通常以d结尾，比如sshd、xinetd、crond等

### 孤儿进程
　　一个父进程退出，而它的一个或多个子进程还在运行，那么那些子进程将成为孤儿进程。孤儿进程将被init进程(pid=1)所收养，并由init进程对它们完成状态收集工作。由于孤儿进程会被init进程给收养，所以**孤儿进程不会对系统造成危害**。

### 僵尸进程
  * 僵尸状态：任何一个子进程(init除外)在exit()之后，内核释放该进程所有的资源，包括打开的文件和占用的内存等，但会留下一个称为僵尸进程(Zombie)的数据结构（包括进程id、退出状态、进程使用CPU的时间等），并向父进程发送一个SIGCHLD信号，直到父进程通过wait()或是waitpid()来取时才释放。这是每个子进程在结束时都要经过的阶段。这时用ps命令就能看到子进程的状态是“Z”。如果父进程在子进程结束之前退出，则子进程将由init接管。init将会以父进程的身份对僵尸状态的子进程进行处理。
  * 危害：一个父进程如果只复制fork子进程，而不负责对子进程进行wait()或是waitpid()调用来释放其所占有资源的话，那么保留的僵尸进程的信息就不会释放，其进程号就会一直被占用，但是系统所能使用的进程号是有限的，如果大量的产生僵尸进程，将因为没有可用的进程号而导致系统不能产生新的进程。

### 怎么杀死僵尸进程
  * 用top命令来查看服务器当前是否有僵尸进程：
  ```
    Tasks: 123 total, 1 running, 122 sleeping, 0 stopped, 0 zombie  //zombie前面的数量就是僵尸进程到数量；
  ```
  * ps -efl 查找状态为Z的进程，如果最后有defunct的标记，就表明是僵尸进程：
  ```
    ps -ef | grep defunct
    4767 ?        Z      0:00      \_ [nova-novncproxy] <defunct>
  ```
  * 要消灭系统中大量的僵尸进程，**只需要将其父进程杀死，此时僵尸进程就会变成孤儿进程**，从而被init进程所收养，这样init进程就会释放所有的僵尸进程所占有的资源，从而结束僵尸进程。
<br>

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
  
  
  
  
  

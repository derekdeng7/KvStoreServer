# I/O多路复用

## select，poll，epoll比较
### 1. 功能
  * select会修改描述符，而poll不会；
  * select的描述符类型使用数组实现，FD_SETSIZE大小默认为1024，因此默认只能监听1024个描述符。如果要监听更多描述符的话，需要修改FD_SETSIZE之后重新编译；而poll的描述符类型使用链表实现，没有描述符数量的限制。epoll比select和poll更加灵活而且没有描述符数量限制；
  * poll提供了更多的事件类型，并且对描述符的重复利用上比select高。
  * 如果一个线程对某个描述符调用了select或者poll，另一个线程关闭了该描述符，会导致调用结果不确定。epoll对多线程编程更有友好，一个线程调用了 epoll_wait()，另一个线程关闭了同一个描述符也不会产生像select和poll的不确定情况。
	
### 2. 速度
  * select和poll速度都比较慢，epoll速度快很多；
  * select和poll每次调用都需要将全部描述符从应用进程缓冲区复制到内核缓冲区，epoll只需要将描述符从进程缓冲区向内核缓冲区拷贝一次。
  * select和poll的返回结果中没有声明哪些描述符已经就绪。所以如果返回值大于0时，应用进程都需要先将全部描述符从内核缓冲区复制到应用进程缓冲区，再使用轮询的方式来找到就绪的描述符；epoll直接获得只有就绪描述符的链表，应用进程不需要通过轮询来获得就绪的描述符。
	
### 3. 可移植性
  * 几乎所有的系统都支持 select，但是只有比较新的系统支持poll，epoll仅适用于Linux OS。
	
## Epoll水平触发（Level trigger）与边缘触发（Edge trigger）
### 1. 水平触发（LT）处理过程
  * accept一个连接，添加到epoll中监听EPOLLIN事件；
  * 当EPOLLIN事件到达时（即有读事件），调用read()读取fd（socket）中的数据并处理；
  * 当需要写出数据时，调用write()把数据写到fd对应的缓冲区中。**如果数据较大，无法一次性写出（缓冲区已满），那么在epoll中注册并监听EPOLLOUT事件**；
  * 当EPOLLOUT事件到达时，继续调用write()把数据写到fd对应的数据区中。**如果数据写出完毕，必须在epoll中关闭EPOLLOUT事件（每当fd的缓冲区非满时都会发送EPOLLOUT请求，不主动关闭将一直收到该请求）**。
  
### 2. 边缘触发（ET）处理过程
  * accept一个连接，添加到epoll中监听EPOLLIN|EPOLLOUT事件
  * 当EPOLLIN事件到达时，调用read()读取fd（socket）中的数据并处理，**必须一直读直到返回EAGAIN为止（返回EAGAIN表明读缓冲区已无数据可读）**；
  * 当需要写出数据时，调用write()把数据写到fd对应的数据区中，**直到数据全部写完或者write返回EAGAIN**；
  * 当EPOLLOUT事件到达时，继续调用write()把数据写到fd对应的数据区中，**直到数据全部写完或者write返回EAGAIN**。
  
### 3. 水平触发与边缘触发比较
  * LT下socket接收缓冲区不为空，读事件就会一直触发；ET下socket的接收缓冲区状态发送变化时才触发读事件，即空的接收缓冲区刚接收到数据时才触发读事件；
  * LT下socket发送缓冲区不满，写事件就会一直触发；ET下socket的发送缓冲区状态变化时触发写事件，即满的缓冲区刚空出空间时触发读事件；
  * LT的编程与poll/select接近，符合一直以来的习惯，不易出错；ET的编程可以做到更加简洁，某些场景下更加高效，但另一方面容易遗漏事件，容易产生bug（可以通过重复写入相同的EPOLLIN|EPOLLOUT事件强制刷新状态触发）；
  * LT的处理过程中，直到返回EAGAIN不是硬性要求；ET的要求是需要一直读写，直到返回EAGAIN，否则就会遗漏事件；
  * **ET理论上比LT更高效，主要出现在容易触发LT开关EPOLLOUT事件的情景。当发送数据频繁大于fd的缓冲区时，LT开关EPOLLOUT将更加频繁，性能的差异将更大**。
	
## Epoll详解
### int epoll_create(int size)
  * 创建struct eventpoll类型的对象；
  * 获取一个空闲的文件描述符fd作为返回值；
  * 创建一个名字为"[eventpoll]"的struct file, 将struct eventpoll挂载到struct file类型对象的private_data成员上，后续通过struct file访问struct eventpoll。
  
### struct eventpoll类型
  * struct rb_root rbr：存放所有通过epoll_ctl()系统调用添加到epoll中的事件对应的类型为struct epitem的对象的红黑树的根；
  * struct list_head rdllist：双向链表，存放了将要通过epoll_wait()系统调用返回给用户态应用程序的就绪事件对应的struct epitem对象；
  * struct epitem *ovflist：单向链表，对rdllist扫描期间需要保证数据的一致性，如果此时又有新的就绪事件发生，那么就需要提供临时的空间来存储，所以ovflist就扮演了这个角色。。
  
### int epoll_ctl(int epfd, int op, int fd, struct epoll_event \*event)
  * op用于指明epoll如何操作event事件，其取值包括EPOLL_CTL_ADD、EPOLL_CTL_MOD和EPOLL_CTL_DEL；
  * event则是被监控的事件对象，其类型为struct epoll_event：
  ```
    struct epoll_event {
	    __u32 events;   //events成员指明了用户态应用程序感兴趣的事件类型，比如EPOLLIN和EPOLLOUT等
	    __u64 data;     //data成员则是提供给用户态应用程序使用，一般用于存储事件的上下文，如channel
    }
  ```
  * 通过epfd获取到对应的类型为struct eventpoll的epoll对象；
  * 用用户态传递进来的事件fd及其对应的struct file对象调用ep_find()到epoll对象的rbr成员中去寻找是否有对应的类型为struct epitem的对象，有则返回，否则返回NULL；
  * op为EPOLL_CTL_ADD将调用ep_insert()。
  
### 调用ep_insert()函数创建对象
  * 创建并初始化一个strut epitem类型的对象，完成该对象和被监控事件（包括fd、struct epoll_event类型的对象）以及struct eventpoll的关联；
  * 将struct epitem类型的对象加入到epoll对象eventpoll的红黑树中管理起来；
  * 将struct epitem类型的对象加入到被监控事件对应的目标文件的等待列表中，并注册事件就绪时会调用的回调函数，在epoll中该回调函数就是ep_poll_callback()。
  
### 以socket为例的注册回调函数的工作原理
  * socket层会实现一个通用的poll回调函数，以tcp为例，这个poll回调函数就是tcp_poll()；
  * 当socket有事件就绪时，比如读事件就绪，就会调用sock->sk_data_ready回调函数，即sock_def_readable()，在这个回调函数中则会**遍历socket文件中的等待队列，然后依次调用队列节点的回调函数**；
  * epoll节点对应的回调函数是ep_poll_callback()，这个函数会将就绪事件对应的struct epitem加入到struct eventpoll的就绪链表rdllist中，这样用户态程序调用epoll_wait()的时候就能获取到该事件；
  * 如果调用ep_poll_callback()函数的时候发现epoll对象eventpoll的ovflist成员不等于EP_UNACTIVE_PTR的话，说明此时正在扫描rdllist链表，这个时候会将就绪事件对应的epitem对象加入到ovflist链表暂存起来，等rdllist链表扫描完之后在将ovflist链表中的内容移动到rdllist链表中。

### int epoll_wait(int epfd, struct epoll_event \*events, int maxevents, int timeout);
  * epoll_wait()系统调用主要是用于收集在epoll中监控的就绪事件。epoll_wait()函数返回值表示的是获取到的就绪事件个数；
  * 第二个参数则是已经分配好内存的epoll_event结构体数组，用于给内核存放就绪事件的；
  * 第三个参数表示本次最多可以返回的就绪事件个数，这个通常和events数组的大小一样；
  * 第四个参数表示在没有检测到事件发生时epoll_wait()的阻塞时长；
  * epoll_wait()首先是根据epfd获取到struct eventpoll，然后再调用ep_poll()获取就绪事件。
  * LT和ET的实现：调用epoll_wait()会把就绪事件的拷贝到用户态内存，然后清空rdllist，**最后检查若就绪事件仍有未处理的数据并且未包含EPOLLET位，则又把该句柄放回到刚刚清空的rdllist**。所以，LT模式下只要事件还有数据未处理完，epoll_wait每次都会返回。

### 调用ep_poll()函数获取就绪事件
  * 如果用户态传进来的epoll_wait()的阻塞时间大于0，则换算超时时间。如果一直没有就绪事件发生，那么epoll_wait()就会休眠，让出处理器，等超时时间到了才返回，如果有就绪事件就先放到用户态内存中，然后会返回用户态；
  * 调用了epoll_wait()系统调用等待epoll事件发生的应用进程被加入到ep->wq等待队列中，并设置了默认的回调函数用于唤醒应用程序；
  * 有就绪事件或者超时则退出循环，从ep->wq等待队列中将调用了epoll_wait()的进程对应的节点移除，设置当前进程的状态为RUNNING；
  * 判断epoll对象的rdllist链表和ovflist链表是否为空，如果不为空，说明有就绪事件发生，则调用ep_send_events()函数做进一步处理，在ep_send_events()函数中又会调用ep_scan_ready_list()函数获取epoll对象eventpoll中的rdllist链表；
  * 在我们扫描处理eventpoll中的rdllist链表的时候可能同时会有就绪事件发生，这个时候为了保证数据的一致性，在这个时间段内发生的就绪事件会临时存放在eventpoll对象的ovflist链表成员中，待rdllist处理完毕之后，再将ovflist中的内容移动到rdllist链表中，等待下次epoll_wait()的调用；
  
### 调用ep_scan_ready_list()函数获取rdllist链表
  * 调用ep_send_events_proc()进行扫描处理，即遍历rdllist链表中的epitem对象，针对每一个epitem对象调用ep_item_poll()函数去获取就绪事件的掩码；
  * 如果掩码不为0，说明该epitem对象对应的事件发生了，那么就将其对应的struct epoll_event类型的对象拷贝到用户态指定的内存中；
  * 如果掩码为0，则直接处理下一个epitem。
  

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  
  

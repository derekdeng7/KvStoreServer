# I/O多路复用

### select，poll，epoll比较
#### 功能
  * select会修改描述符，而poll不会；
  * select的描述符类型使用数组实现，FD_SETSIZE大小默认为1024，因此默认只能监听1024个描述符。如果要监听更多描述符的话，需要修改FD_SETSIZE之后重新编译；而poll在select基础上只是用链表存储描述符，去除了描述符数量的限制。epoll比select和poll更加灵活而且没有描述符数量限制；
  * poll提供了更多的事件类型，并且对描述符的重复利用上比select高。
  * 如果一个线程对某个描述符调用了select或者poll，另一个线程关闭了该描述符，会导致调用结果不确定。epoll对多线程编程更有友好，一个线程调用了 epoll_wait()，另一个线程关闭了同一个描述符也不会产生像select和poll的不确定情况。
#### 速度
  * select和poll速度都比较慢，epoll速度快很多；
  * select和poll每次调用都需要将全部描述符从应用进程缓冲区复制到内核缓冲区，epoll只需要将描述符从进程缓冲区向内核缓冲区拷贝一次。
  * select和poll的返回结果中没有声明哪些描述符已经就绪。所以如果返回值大于0时，应用进程都需要先将全部描述符从内核缓冲区复制到应用进程缓冲区，再使用轮询的方式来找到就绪的描述符；epoll直接获得只有就绪描述符的链表，应用进程不需要通过轮询来获得就绪的描述符。
#### 可移植性
  * 几乎所有的系统都支持 select，但是只有比较新的系统支持poll，epoll仅适用于Linux OS。
	
### Epoll水平触发（Level trigger）与边缘触发（Edge trigger）
#### 水平触发（LT）处理过程
  * accept一个连接，添加到epoll中监听EPOLLIN事件；
  * 当EPOLLIN事件到达时（即有读事件），调用read()读取fd（socket）中的数据并处理；
  * 当需要写出数据时，调用write()把数据写到fd对应的缓冲区中。**如果数据较大，无法一次性写出（缓冲区已满），那么在epoll中注册并监听EPOLLOUT事件**；
  * 当EPOLLOUT事件到达时，继续调用write()把数据写到fd对应的数据区中。**如果数据写出完毕，必须在epoll中关闭EPOLLOUT事件（每当fd的缓冲区非满时都会发送EPOLLOUT请求，不主动关闭将一直收到该请求）**。
#### 边缘触发（ET）处理过程
  * accept一个连接，添加到epoll中监听EPOLLIN|EPOLLOUT事件
  * 当EPOLLIN事件到达时，调用read()读取fd（socket）中的数据并处理，**必须一直读直到返回EAGAIN为止（返回EAGAIN表明读缓冲区已无数据可读）**；
  * 当需要写出数据时，调用write()把数据写到fd对应的数据区中，**直到数据全部写完或者write返回EAGAIN**；
  * 当EPOLLOUT事件到达时，继续调用write()把数据写到fd对应的数据区中，**直到数据全部写完或者write返回EAGAIN**。
  
### 水平触发与边缘触发比较
  * LT下socket接收缓冲区不为空，读事件就会一直触发；ET下socket的接收缓冲区内待读数据增加的时候时候（由空变为不空的时候、或者有新的数据进入缓冲区）；
  * LT下socket发送缓冲区不满，写事件就会一直触发；ET下socket的发送缓冲区内待发送的数据减少的时候（由满状态变为不满状态的时候、或者有部分数据被发出去的时候）；
  * LT下socket可以设置为阻塞或者非阻塞（但一般设置为非阻塞，阻塞则每次最多只能`read()`一次）；**ET下socket必须设置为非阻塞，这是工程上要求，因为需要用到`while(read())`，最后无数据可读会阻塞线程**；
  * LT的处理过程中，直到返回EAGAIN不是硬性要求；ET的要求是需要一直读写，直到返回EAGAIN，否则在缓冲区变化前，数据都无法读写；
  * LT的编程与poll/select接近，符合一直以来的习惯，不易出错；ET的编程可以做到更加简洁，某些场景下更加高效，但另一方面容易遗漏事件，容易产生bug（可以通过重复写入相同的EPOLLIN|EPOLLOUT事件强制刷新状态触发）；
  * **ET理论上比LT更高效，主要出现在容易触发LT开关EPOLLOUT事件的情景。当发送数据频繁大于fd的缓冲区时，LT开关EPOLLOUT将更加频繁，性能的差异将更大。但目前还没有实际应用场合的测试表面ET比LT性能更好**。
	
### listen fd为什么最好使用水平触发
  * 有多个连接同时到达监听端口，这时监听队列由空变为非空，EPOLLIN事件被触发，如果只进行了一次accept，则只有一个连接被接受，之后尽管监听队列非空，由于在ET模式下，也不会再次触发EPOLL事件了，需要有新的连接才会再次返回监听队列，但旧的连接很可能由于超时而失效。 
  * 在epoll中，监听套接字的触发模式最好设置为LT模式，当然也可以使用ET，但必须使用while循环调用accept，直到监听队列为空。
	
### 为什么IO多路复用要搭配非阻塞IO
  * IO多路复用只会告诉你fd对应的socket可读了，但不会告诉你有多少的数据可读。如果是非阻塞IO，循环读或者只读一次都不会有问题；如果是阻塞IO，最多只能读一次，因为你不知道第二次还有没有数据可读，一旦没有数据可读，会阻塞线程；
  * epoll的ET模式必须使用非阻塞IO，因为工程实现上用到了循环读；
  * 多线程读同一个socket可能会有惊群现象：多个进程或者线程通过select或者epoll监听一个listen socket，当有一个新连接完成三次握手之后，所有进程都会通过select或者epoll被唤醒，但是最终只有一个进程或者线程accept到这个新连接，若是采用了阻塞IO，没有accept到连接的进程或者线程就被阻塞了。（nginx采用了accept_mutex或者reuseport的方式来避免惊群）。
	
### Epoll源码详解
#### int epoll_create(int size)
  * 创建struct eventpoll类型的对象；
  * 获取一个空闲的文件描述符fd作为返回值；
  * 创建一个名字为"[eventpoll]"的struct file, 将struct eventpoll挂载到struct file类型对象的private_data成员上，后续通过struct file访问struct eventpoll。 
#### struct eventpoll类型
  * struct rb_root rbr：存放所有通过epoll_ctl()系统调用添加到epoll中的事件对应的类型为struct epitem的对象的红黑树的根；
  * struct list_head rdllist：双向链表，存放了将要通过epoll_wait()系统调用返回给用户态应用程序的就绪事件对应的struct epitem对象；
  * struct epitem \*ovflist：单向链表，对rdllist扫描期间需要保证数据的一致性，如果此时又有新的就绪事件发生，那么就需要提供临时的空间来存储，所以ovflist就扮演了这个角色。。 
#### int epoll_ctl(int epfd, int op, int fd, struct epoll_event \*event)
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
#### 调用ep_insert()函数创建对象
  * 创建并初始化一个strut epitem类型的对象，完成该对象和被监控事件（包括fd、struct epoll_event类型的对象）以及struct eventpoll的关联；
  * 将struct epitem类型的对象加入到epoll对象eventpoll的红黑树中管理起来；
  * 将struct epitem类型的对象加入到被监控事件对应的目标文件的等待列表中，并注册事件就绪时会调用的回调函数，在epoll中该回调函数就是ep_poll_callback()。
#### int epoll_wait(int epfd, struct epoll_event \*events, int maxevents, int timeout);
  * epoll_wait()系统调用主要是用于收集在epoll中监控的就绪事件。epoll_wait()函数返回值表示的是获取到的就绪事件个数；
  * 第二个参数则是已经分配好内存的epoll_event结构体数组，用于给内核存放就绪事件的；
  * 第三个参数表示本次最多可以返回的就绪事件个数，这个通常和events数组的大小一样；
  * 第四个参数表示在没有检测到事件发生时epoll_wait()的阻塞时长；
  * epoll_wait()首先是根据epfd获取到struct eventpoll，然后再调用ep_poll()获取就绪事件。
  * LT和ET的实现：调用epoll_wait()会把就绪事件的拷贝到用户态内存，然后清空rdllist，**最后检查若就绪事件仍有未处理的数据并且未包含EPOLLET位，则又把该句柄放回到刚刚清空的rdllist**。所以，LT模式下只要事件还有数据未处理完，epoll_wait每次都会返回。
#### 调用ep_poll()函数获取就绪事件
  * 如果用户态传进来的epoll_wait()的阻塞时间大于0，则换算超时时间。如果一直没有就绪事件发生，那么epoll_wait()就会休眠，让出处理器，等超时时间到了才返回，如果有就绪事件就先放到用户态内存中，然后会返回用户态；
  * 调用了epoll_wait()系统调用等待epoll事件发生的应用进程被加入到ep->wq等待队列中，并设置了默认的回调函数用于唤醒应用程序；
  * 有就绪事件或者超时则退出循环，从ep->wq等待队列中将调用了epoll_wait()的进程对应的节点移除，设置当前进程的状态为RUNNING；
  * 判断epoll对象的rdllist链表和ovflist链表是否为空，如果不为空，说明有就绪事件发生，则调用ep_send_events()函数做进一步处理，在ep_send_events()函数中又会调用ep_scan_ready_list()函数获取epoll对象eventpoll中的rdllist链表；
  * 在我们扫描处理eventpoll中的rdllist链表的时候可能同时会有就绪事件发生，这个时候为了保证数据的一致性，在这个时间段内发生的就绪事件会临时存放在eventpoll对象的ovflist链表成员中，待rdllist处理完毕之后，再将ovflist中的内容移动到rdllist链表中，等待下次epoll_wait()的调用；
#### 调用ep_scan_ready_list()函数获取rdllist链表
  * 调用ep_send_events_proc()进行扫描处理，即遍历rdllist链表中的epitem对象，针对每一个epitem对象调用ep_item_poll()函数去获取就绪事件的掩码；
  * 如果掩码不为0，说明该epitem对象对应的事件发生了，那么就将其对应的struct epoll_event类型的对象拷贝到用户态指定的内存中；
  * 如果掩码为0，则直接处理下一个epitem。
#### 源码中LT和ET的区别在ep_send_events_proc()中
```
  for (eventcnt = 0, uevent = esed->events; !list_empty(head) && eventcnt < esed->maxevents;) 
  {
		epi = list_first_entry(head, struct epitem, rdllink);
		ws = ep_wakeup_source(epi);
		if (ws) {
			if (ws->active)
				__pm_stay_awake(ep->ws);
			__pm_relax(ws);
		}
		list_del_init(&epi->rdllink);	//从链表中移除
		revents = ep_item_poll(epi, &pt);	// 获取事件

		if (revents) 	// 获取到事件，需要copy到userspace，没有事件的fd此时才从真正意义上的移除出就绪队列
		{	
			if (__put_user(revents, &uevent->events) ||
			    __put_user(epi->event.data, &uevent->data)) 
		        {
				list_add(&epi->rdllink, head);	//出错添加回去
				ep_pm_stay_awake(epi);
				return eventcnt ? eventcnt : -EFAULT;
			}
			eventcnt++;
			uevent++;
			if (epi->event.events & EPOLLONESHOT)
				epi->event.events &= EP_PRIVATE_BITS;
			else if (!(epi->event.events & EPOLLET)) 		//LT模式和ET模式的区别在此
			{	
				list_add_tail(&epi->rdllink, &ep->rdllist); //LT模式要重新加入到ready list
				ep_pm_stay_awake(epi);
			}
		}
 }
```
  * 如果是ET，epitem是不会再进入到readly list，除非fd再次发生了状态改变，使ep_poll_callback被调用。
  * 如果是LT, 此时此刻epoll_wait还没有返回，用户程序还没有开始处理事件或者数据，**epoll自然也就无法区分哪些事件或者数据没有处理完，只好把所有的事件都重新插入到ready list。也就是说就算某个fd已经处理完了，仍然会出现在下一次epoll_wait的ready list中，再次判断它没有事件或者数据才移除出ready list。假如这一次ready list的所有fd的事件都处理完，且没有新的事件到达，那么下一次epoll_wait将返回0，即空转一次**。


### 以socket为例的注册回调函数的工作原理
  * socket层会实现一个通用的poll回调函数，以tcp为例，这个poll回调函数就是tcp_poll()；
  * 当socket有事件就绪时，比如读事件就绪，就会调用sock->sk_data_ready回调函数，即sock_def_readable()，在这个回调函数中则会**遍历socket文件中的等待队列，然后依次调用队列节点的回调函数**；
  * epoll节点对应的回调函数是ep_poll_callback()，这个函数会将就绪事件对应的struct epitem加入到struct eventpoll的就绪链表rdllist中，这样用户态程序调用epoll_wait()的时候就能获取到该事件；
  * 如果调用ep_poll_callback()函数的时候发现epoll对象eventpoll的ovflist成员不等于EP_UNACTIVE_PTR的话，说明此时正在扫描rdllist链表，这个时候会将就绪事件对应的epitem对象加入到ovflist链表暂存起来，等rdllist链表扫描完之后在将ovflist链表中的内容移动到rdllist链表中。
  
### 为什么需要Reactor模型
　　Reactor模型是同步IO复用下的事件驱动模型。在Epoll已经能够完成并发处理几十万条连接请求的前提下，Reactor模型的意义在于能够极大的简化网络应用的开发。epoll返回rdllist上的就绪事件struct epoll_event后，我们需要通过其中的data字段，获得对应对象的上下文，结合上下文状态查询此时应当选择哪个业务方法处理，调用相应方法完成操作后，若请求结束，则删除对象及其上下文。整个过程可能会非常繁琐，不利于开发与维护，也不符合面向对象的思想。Reactor模型是解决上述工程问题的一种途径，将事件驱动框架与具体业务分离，将不同类型请求之间用面向对象的思想分离。
  
### Reactor模型结构
  * 文件描述符（handle）：由操作系统提供的资源，用于识别每一个事件，如Socket描述符、一般文件描述符、信号的值等。如epollfd、socketfd；
  * 同步事件多路分离器（event demultiplexer）：**事件的到来是随机的、异步的，无法预知程序何时收到一个客户连接请求或收到一个信号。所以程序要循环等待并处理事件，这就是事件循环（event loop）**。在事件循环中，等待事件一般使用I/O复用技术实现。I/O框架库一般将各种I/O复用系统调用封装成统一的接口，称为事件多路分离器。调用者会被阻塞，直到分离器分离的描述符集上有事件发生。如Epoll；
  * 事件处理器（event handler）：I/O框架库提供的事件处理器通常是由一个或多个模板函数组成的接口。事件处理器中的回调函数（必须是非阻塞的）一般声明为虚函数，以支持用户拓展。如Channel；
  * 具体的事件处理器（concrete event handler）：用户继承事件处理器的模板函数实现自己的事件处理器，即**具体事件处理器是事件处理器接口的实现**。它实现了应用程序提供的某个服务。如Acceptor、Connector；
  * Reactor管理器（reactor）：定义了一些接口，用于应用程序控制事件调度，以及应用程序注册、删除事件处理器和相关的描述符。它是事件处理器的调度核心。 **Reactor管理器使用同步事件分离器来等待事件的发生。一旦事件发生，Reactor管理器先是分离每个事件，然后调度事件处理器，最后调用相关的模板函数来处理这个事件**，如EventLoop。
  
### Reactor模型与Proactor模型比较
  * Reactor模型是同步IO，Proactor模型是异步IO；
  * Reactor和Proactor模式的主要区别就是真正的读取和写入操作是有谁来完成的。Reactor中通知应用程序自己读取或者写入数据到自己的缓冲区，而Proactor模式中，应用程序不需要进行实际的读写过程，它只需要从缓冲区读取或者写入即可，操作系统会读取缓冲区或者写入缓冲区到真正的IO设备，完成后再通知应用程序。

### read()
```
  ssize_t read(int fd, void *buf, size_t count);
```
  * 成功时返回值大于0，代表实际read的字节数。
  * **读到文件的结束了返回值是0，这是非阻塞下才会出现，阻塞模式会直接阻塞；如果有读事件发生但返回0，则是收到FIN**。
  * 异常时返回的值小于 0，如果为EINTR说明读是由中断引起的；如果为EAGAIN，阻塞模式表示超时，非阻塞模式表示无数据可读；如果为EBADF表示主动关闭再读数据；如果为ECONNREST表示连接异常关闭（对方发送RST）之后再读数据。

### write()
```
  ssize_t write(int fd, const void *buf, size_t count);
```
  * 成功时返回值大于0，表示写了部分或者是全部的数据（准确来说应该是从应用缓冲区复制到内核缓冲区的数据量）。
  * 失败时返回的值小于 0，如果为EINTR表示在写的时候出现了中断错误；如果为EBADF表示主动关闭再写数据；如果为EPIPE表示连接异常关闭（对方发送RST）之后再写数据。
  
### recv()和send()
recv()和send()函数提供了和read和write差不多的功能.不过它们提供了第四个参数来控制读写操作.
```
  int recv(int sockfd,void *buf,int len,int flags)
  int send(int sockfd,void *buf,int len,int flags)
```
  * MSG_DONTROUTE：不查找表；
  * MSG_OOB：接受或者发送带外数据；
  * MSG_PEEK：查看数据,并不从系统缓冲区移走数据；
  * MSG_WAITALL： 等待所有数据。
 
### Socket中TCP的三次握手和四次挥手
  [Linux Socket 编程（不限 Linux）](https://www.cnblogs.com/skynet/archive/2010/12/12/1903949.html)

### 内核缓冲区变化
  * 内核发送缓冲区数据的增加: 应用层调用send()成功，增加的数据量依赖于send()的返回值。
  * 内核发送缓冲区数据的减少: 收到对端内核接收缓冲区对收到的数据发送ACK回执。
  * 内核接收缓冲区数据的增加: 接收到对端内核态发送的数据(并会给这些数据发送ACK)。
  * 内核接收缓冲区数据的减少: 应用层调用recv()成功，内核删除的数据依赖于应用层读取了多少数据。

### Socket什么情况下可读
  * 当socket的接收缓冲区中的数据字节大于等于其接收缓冲区低水位`SO_RCVLOWAT`标记的大小，读操作将不阻塞并返回可读数据大小。对于TCP和UDP的socket而言，其缺省值为1；
  * 该连接的读通道关闭(也就是接收了FIN的TCP连接)，读操作将不阻塞并返回0；
  * 给监听套接字准备好新连接；
  * 当socket有异常错误条件待处理，读操作将不会阻塞并且返回小于0的值。

### Socket什么情况下可写
  * 当socket的发送缓冲区中的空闲空间大于等于该socket的发送缓冲区低水位`SO_SNDLOWAT`标记的大小，写操作将不阻塞并返回可写数据大小。对于TCP和UDP的socket而言，其缺省值为2048；
  * 该连接的写通道关闭，写操作将产生SIGPIPE信号，该信号的缺省行为是终止进程；
  * 使用非阻塞connect的套接字已建立连接, 或者connect已经以失败告终；
  * 当socket有异常错误条件待处理，写操作将不会阻塞并且返回小于0的值。
	
### select/epoll返回，是否一定可读
  * select/epoll返回可读，和`read()`去读，这是两个独立的系统调用，两个操作之间是有窗口期，可能数据被抢先读走了，也可能内核校验后把它丢弃了。也就是说select/epoll返回可读，紧接着去`read()`，不能保证一定可读。
  * 惊群现象，就是一个典型场景。多个进程或者线程通过select/epoll监听一个listen socket，当有一个新连接完成三次握手之后，所有进程都会通过select/epoll被唤醒，但是最终只有一个进程或者线程accept到这个新连接，若是采用了阻塞IO，没有accept到连接的进程或者线程就block住了。

### close()
#### 原理
  * 调用`close(sockfd)`时，内核检查此fd对应的socket上的引用计数。如果引用计数大于1则仅仅减1并返回。**如果等于1，内核才会真正销毁套接字（关闭连接只是附带结果）**。
  * 线程在未知引用计数大于1的情况下调用`close()`，以为连接已经关闭，不再作处理，会导致失去控制的socket阻塞在CLOSE_WAIT状态。服务器端如果积攒大量的COLSE_WAIT状态的socket，有可能将服务器资源耗尽，进而无法提供服务。
#### 以服务端为例调用close()
  * 如果本地缓冲区仍有未发送数据时调用close()，会向客户端发送一个RST报文，丢弃本地缓冲区的未读数据，关闭socket并释放相关资源，此种方式为强制关闭，客户端无需回复。（l_onoff=1，l_linger=0）；
  * 否则向客户端发送一个FIN报文，收到client端FIN ACK后，进入了FIN_WAIT_2阶段，可参考TCP四次挥手过程，此种方式为优雅关闭。如果在l_linger的时间内仍未完成四次挥手，则发送RST，强制关闭。（ l_onoff=1，l_linger=1）
 
### shutdown()
  * `shutdown()`**用于优雅关闭连接，它关闭socket连接而不会释放socket，需要再调用`close(sockfd)`释放套接字的资源**。
  * `SHUT_RD`：关闭连接的读通道，套接字中不再有数据可接收，进程不能再对这样的套接字调用任何读函数（返回EOF）。该套接字接收的来自对端的任何数据都被确认，然后悄然丢弃。**如果当前接收缓存中仍有未取出数据或者以后再有数据到达，则TCP会向发送端发送RST包，将连接重置**。
  * `SHUT_WR`：关闭连接的写通道，将发送缓存中的数据都发送完毕并收到所有数据的ACK后向对端发送FIN包，表明本端没有更多数据发送，这个是一个优雅关闭过程。进程不能再对这样的套接字调用任何写函数（检测到SIGPIPE）。
  * `SHUT_RDWR`：同时断开I/O流，相当于调用shutdown两次：首先是以SHUT_RD，然后以SHUT_WR；
	
### close()与shutdown()的区别
  * 默认情况下，`close()/closesocket()`会强制关闭连接，**不管输出缓冲区中是否还有数据**；而`shutdown()`会等输出缓冲区中的数据传输完毕再发送FIN包。也就意味着，调用`close()/closesocket()`将丢失输出缓冲区中的数据，而调用`shutdown()`不会。
  * shutdown()会唤醒阻塞线程，close()不会唤醒阻塞线程；
  * 在多进程中如果一个进程中`shutdown(sfd, SHUT_RDWR)`后，其它所有的进程将无法进行通信；`close(sfd)`只会影响本进程，其他进程能继续使用（如果计数不为0）。
  
### Socket连接的优雅关闭（透明传递）
  * 主动关闭连接：server发送完数据后，调用`shundown(clientSock, SHUT_WR)`，向client发送FIN，此时服务器socket变成close_wait状态。Client调用recv()返回0（如果出错则返回-1，直接close()结束），如果有数据要发送，可以继续发送，没有可以调用`shundown(serverSock, SHUT_WR)`或直接close()，会向Server发送FIN，server调用recv()返回0，此时可以调用`shundown(clientSock, SHUT_RD)`，等一个RTT再调用close()（或者直接调用close）。**如果Client没有发送FIN，将导致Server一直留在clost_wait状态，占用系统资源，此时可以通过设置超时时间强制关闭套接字**。
  * 被动关闭连接：Client发送FIN（通过close()或者shundown(serverSock, SHUT_WR)，服务端无法区分），Server调用recv()返回0，调用`shundown(clientSock, SHUT_RD)`关闭接收操作，此时Server进入close_wait状态，如果有数据继续发送，发送完毕则调用`shundown(clientSock, SHUT_WR)`关闭发送操作，等一个RTT再调用close()释放socket资源。
	
### RST
#### RST的发送 
  * 若发送FIN报文后没有收到client端的FIN ACK，server端会两次重传FIN报文，若一直收不到client端的FIN ACK，则会给client端发送RST信号，关闭socket并释放资源;
  * 当缓冲区还有未读取的数据时，调用`close(sockfd)`函数关闭socket，会触发TCP发送RST，并不会等待对方返回ACK。
#### client端如何知道已经接收到RST报文？
　　server发送RST报文后，并不等待从client端接收任何ack响应，直接关闭socket。而client端收到RST报文后，也不会产生任何响应。client端收到RST报文后，程序行为如下：
  * 阻塞模型：内核无法主动通知应用层出错，只有应用层主动调用read()或者write()这样的IO系统调用时，内核才会利用出错来通知应用层对端已经发送RST报文。
  * 非阻塞模型：select/epoll会返回sockfd可读，应用层对其进行读取时，read()会报RST错误。
  * 应用程序可以通过read()/write()函数出错返回后，获取errno来确定对端是否发送RST信号。
#### RST处理
　　client端收到RST信号后，如果调用read()读取，则会返回RST错误。在已经产生RST错误的情况下，继续调用write()，则会发生epipe错误。此时内核将向客户进程发送 SIGPIPE 信号，该信号默认会使进程终止，通常程序会异常退出（未处理SIGPIPE信号的情况下）。

### FIN处理
  * client收到FIN信号后，再调用read函数会返回0。因为FIN的接收，表明client端以后再无数据可以接收；对方发来FIN，表明对方不再发送数据了。
  * 根据tcp协议，向一个FIN_WAIT2状态的TCP写入数据是没有问题的，所以此时client可以调用write函数，写入到发送缓冲区，并由tcp连接，发送到server的接收缓冲区。由于server端已经关闭了socket，所以此时的server接收缓冲区的内容都被抛弃，同时server端返回RST给客户端。

### 如果select返回可读，结果只读到0字节，什么情况
 　　select()返回可读，表示套接字接收到数据；read()返回0，表示数据意义是“对方关闭连接”（read返回0的唯一条件是对方优雅关闭了套接字）。
   
### 非阻塞connect（）如何实现
connect设置非阻塞，放入select，select返回时检测状态是否可读可写（要排除错误的情况）。

### Linux下Server处理异常关闭连接的情况
情况 | 结果
--- | ---
Client忘记关闭Socket而退出、发送消息过程中以非正常方式结束进程 | 收到“104: Connection reset by peer”（ECONNRESET）
网线出现 | 等待“超时”，收到“101：connect time out”（ETIMEOUT）断开TCP连接
Client关闭Socket后，Server未检测到关闭的情况下发送消息 | 发送消息时产生“32: Broken pipe”（EPIPE）

### Linux下Client处理异常关闭连接的情况
情况 | 结果
--- | ---
网线出现 | 等待“超时”，收到“101：connect time out”（ETIMEOUT）断开TCP连接
Server在接收缓冲区中还有未接收数据的情况下关闭了Socket | Server发送RST，Client提前收到“104: Connection reset by peer”（RST包比正常数据包先被收到）
Server正常关闭Socket后，Client发送一个消息 | 发送成功，服务器端返回RST
Client收到RST后继续发送消息 | 发送失败并收到“32: Broken pipe”，同时收到SIGPIPE信号（该信号默认会使进程终止）

### Socket错误码errno
errno | Berkeley对应错误码 | 意义
--- | --- | ---
4 | EINTR | 阻塞IO的操作被取消阻塞的调用打断。如设置了发送接收超时
9 | EBADF | 该Socket已失效
11 | EAGAIN | 非阻塞IO返回
14 | EFAULT | 地址错误
32 | EPIPE | 对关闭的Socket读写，一般在网络程序中，首先屏蔽此消息，以免发生不及时设置socket进程被杀死的情况
35 | EWOULDBLOCK | 对非阻塞Socket进行的不能立即结束的操作返回的
61 | ECONNREFUSED | 拒绝连接。一般发生在连接建立时
104 | ECONNRESET | 对方关闭了连接
110 | ETIMEOUT | 一般设置了发送接收超时，遇到网络繁忙的情况会发生





  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

  
  

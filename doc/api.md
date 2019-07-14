# api总结

### open与fopen 、read与fread、write与fwrite区别
open系列函数 | fopen系列函数
---|---
一般用于打开设备文件（少数情况） | 一般用于打开普通文件（大多数情况）
利用文件描述符操纵文件 | 利用文件指针操作文件
open返回一个文件描述符 | fopen返回一个文件指针
POSIX系统调用 | ANSI C库函数
低层次IO | 高层次IO，对open的扩展和封装（故一次调用速度较慢）
只能在POSIX操作系统上移植 | 可移植到任何操作系统
非无缓冲区 | 有缓冲区（可减少调用次数）
只能读取二进制或普通文本 | 可以读取一个结构
可以指定要创建文件的访问权限 | 不能指定要创建文件的访问权限

### bzero和memset的区别
  * `void bzero(void *s, int n)`是POSIX函数，`void *memset(void *buffer, int c, int count)`是C库函数；
  * 在数组较小的情况下，bzero的效率比memset高；当数组超过一定大小之后，bzero的效率开始比memset低；数组越大，memset的性能优势越明显。而在数组较小的情况下，memset的性能甚至不如直接for循环对数组中的每一个字节置零的方法。

### std::thread
  * 构造函数：std::thread(Function&& f, Args&&... args);（如创建线程运行foo f的成员函数bar：std::thread t1(&foo::bar, &f);）；
  * 可被joinable的std::thread对象必须在他们销毁之前被主线程join()或者将其设置为detached。
  ```
    if(thread)
      thread->join();
  ```
  
### std::mutex
  * std::mutex是C++11中最基本的互斥量，std::mutex对象提供了独占所有权的特性（即非递归），std::recursive_mutex允许同一个线程对互斥量多次上锁（即递归上锁），来获得对互斥量对象的多层所有权；
  * 成员函数lock()：调用线程将锁住该互斥量。线程调用该函数会发生下面 3 种情况：如果该互斥量当前没有被锁住，则调用线程将该互斥量锁住，直到调用 unlock之前，该线程一直拥有该锁； 如果当前互斥量被其他线程锁住，则当前的调用线程被阻塞住；如果当前互斥量被当前调用线程锁住，则会产生死锁(deadlock)。
  * 成员函数unlock()：解锁，释放对互斥量的所有权。
  
### std::lock_guard与std::unique_lock
  * std::lock_guard采用RAII手法管理std::mutex，在对象构造时将std::mutex加锁，析构时对std::mutex解锁。std::lock_guard更为简单易用，性能也更好；
  * std::unique_lock在使用上比std::lock_guard更具有弹性，提供lock()、unlock()、try_lock()等函数，提供了更好的上锁和解锁控制；
  * std::unique_lock不一定要拥有std::mutex，所以可以透过默认构造函数建立出一个空的std::unique_lock；
  * std::unique_lock虽然一样non-copyable，但是它是可以转移的（movable）。所以，std::unique_lock不但可以被函数回传，也可以放到STL容器里。
  ```
    std::mutex mtx1;
    mtx.lock();                                   //std::lock_guard不是独占锁，需要先加锁
    std::lock_guard<std::mutex> locker(mtx1);  
    
    std::mutex mtx2;
    std::unqiue_lock<std::mutex> locker(mtx2);    //std::unqiue_lock是独占锁，若mtx被占用则阻塞
  ```
  
### std::condition_variable
  * 当std::condition_variable对象的某个wait被调用的时候，通常使用std::unique_lock(通过std::mutex)来锁住当前线程。当前线程会一直被阻塞，直到另外一个线程在相同的std::condition_variable对象上调用了notification函数来唤醒当前线程;
  ```
    std::unique_lock<std::mutex> locker(mutex_);
    notEmpty_.wait(locker, [this]{return needStop_ || NotEmpty(); });     //当lamda表达式为false时阻塞
    
    .....
    
    notEmpty_.notify_one();
  ```
   * wait()；当前线程已经获得了mutex锁，若被阻塞则该函数会自动调用locker.unlock()释放锁，使得其他被阻塞在锁竞争上的线程得以继续执行。另外，一旦当前线程获得通知(通常是另外某个线程调用`notify_*`唤醒了当前线程)，wait()会自动调用locker.lock()，使得unlock的状态和wait()被调用时相同；
   * wait_for()；与wait相比增加了超时时间，在当前线程收到通知或者指定的时间超时之前，该线程都会处于阻塞状态。而一旦超时或者收到了其他线程的通知返回；
   * notify_one()：唤醒某个等待(wait)线程。如果当前没有等待线程，则该函数什么也不做，如果同时存在多个等待线程，则唤醒某个线程是不确定的；
   * notify_all()：唤醒所有的等待(wait)线程。如果当前没有等待线程，则该函数什么也不做。

### cv虚假唤醒的问题
   * 虚假唤醒的根源在于互斥锁与条件变量的分离：
   ```
     if(!status) 
     {
       //假设是在等待消费队列，一个线程A被nodify，但是还没有获得锁时，
       //另一个线程B获得了锁，并消费掉了队列中的数据，更新了status。
       //B退出或wait后，A获得了锁，而这时status条件已不满足，不应继续执行下去。
       con.wait(lock);
     }
   ```
   * 传统方法通过while()避免虚假唤醒
   ```
     while(!status)
     {
       //如果出现虚假唤醒，不会结束while()
       con.wait(lock);
     }
   ```
   * C++11 wait()标准调用不存在虚假调用：
   ```
     con.wait(lock, [](){return status});
   ```

### std::atomic_flag
  ```
    //std::atomic_flag实现自旋锁
    std::atomic_flag lock = ATOMIC_FLAG_INIT;               //初始化，默认为false
    while (lock.test_and_set(std::memory_order_acquire)) ;  //访问了一次就置true
    lock.clear(std::memory_order_release);                  //clear()置false
    
  ```
  
### std::atomic 
  ```
    std::atomic<void*> rep_;                                 //原子指针
    
    //C++11在标准库中引入了memory model，意义在于我们可以在high 
    //level language层面实现对在多处理器中多线程共享内存交互的控制
    rep_.store(v, std::memory_order_release);                //修改被封装的值； 
    void* pointer = rep_.load(std::memory_order_acquire);    //读取被封装的值；
  ```
#### 常用的memory_order
  * Sequential consistency：顺序一致性，默认的选项，其不允许reorder，那么也会带来一些性能损失
  * Relaxed ordering：在单个线程内所有原子操作基本上就是代码顺序顺序进行的，这就是唯一的限制了。两个来自不同线程的原子操作是任意顺序；
  * Release-acquire：来自不同线程的两个原子操作顺序可能会不一致，需要两个线程进行一下同步（synchronize-with）限制一下它们的顺序。x86就是Release-acquire语义。
  ```
    aquire语义：load之后的读写操作无法被重排至load之前。即load-load，load-store不能被重排；
    release语义：store之前的读写操作无法被重排至store之后。即load-store，store-store不能被重排。

  ```
  * Release-consume：弱化Release-acquire的同步范围，提高性能。
  
  
  
  
  
  
  
  
  
  
  
  

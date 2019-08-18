# KvStoreServer
a kvstore server with epoll 

## net bench
```
CPU:        12 * Intel(R) Core(TM) i7-8700 CPU @ 3.20GHz
```

Sessions | Tasks | MessageSize (Bytes) | QPS | Throughput (MB/s)
--- | --- | --- | --- | ---
1000 | 1024 | 8192 | 117015 | 914
1000 | 4096 | 2048 | 164696 | 322
1000 | 16384 | 512 | 227555 | 112
1000 | 65536 | 128 | 205402 | 25

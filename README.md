# KvStoreServer
a kvstore server with epoll 

## net bench

Sessions | Tasks | MessageSize (Bytes) | QPS | Throughput (MB/s)
--- | --- | --- | --- | ---
1000 | 1024 | 8192 | 117015 | 914
1000 | 4096 | 2048 | 164696 | 322
1000 | 16384 | 512 | 278720 | 136
1000 | 65536 | 128 | 205402 | 25

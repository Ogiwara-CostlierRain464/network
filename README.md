# How to play

```shell
git clone git@github.com:Ogiwara-CostlierRain464/network.git
cd network
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

Server
```shell
./silo_epoll
```

Client
```shell
./client -t 20
Loop sec: 20, Read/Write: read only
Throughput: 22842.772589 Latency: 42.286819 micro sec
```

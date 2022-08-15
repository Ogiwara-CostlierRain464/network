# How to play

```shell
git clone git@github.com:Ogiwara-CostlierRain464/network.git
cd network
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
./epoll_sample 
```

In another terminal:
```shell
$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
w 2 3
WRITE Key: 2 Value: 3
Connection closed by foreign host.

$ telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
r 2 
READ Key: 2 Value: 3
Connection closed by foreign host.
```
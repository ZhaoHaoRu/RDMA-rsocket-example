# README
## overview
This program allows a server to connect multiple clients simultaneously. The client sends the terminal input to the server, and the server replies to the client with the same content

## prepare
Ensure that you have the hardware conditions for RDMA
```shell
mkdir build
cd build
cmake ..
make
```

## run
1. open the first terminal
```shell
cd build
./server [port]
```

2. open other terminal, start the first client

    the ip is server ip, the port is the server port 
```shell
cd build
./client [ip] [port]
```
3. you can open the third terminal, execute the same command as step2 to run another client

#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <vector>
#include <cassert>
#include <memory>
#include <ctime>
#include <string>
#include <unordered_map>
#include <iostream>

const int MAX_NUM = 1024;

class Epoll {
private:
    int _epfd;
    int _epoll_events_max_nums;
    std::unique_ptr<struct epoll_event> _events;
    ///! key: fd, val: the fd status
    std::unordered_map<int, int> _status;
    ///! key: fd, val: the fd's socket handler create time
    std::unordered_map<int, std::time_t> _start_times;

public:
    std::unordered_map<int, std::string> _username_maps;
    std::unordered_map<std::string, int> _fd_maps;

    Epoll();

    ~Epoll();

    int setnonblocking(int fd);

    int rsetnonblocking(int fd);

    bool epollCreate();

    bool epollAdd(int fd, int state = EPOLLIN | EPOLLOUT);

    bool epollDel(int fd, int state = EPOLLIN | EPOLLOUT);

    bool epollMod(int fd, int state = EPOLLIN | EPOLLOUT);

    int epollWait();

    ///@brief status: -1: close 0:listen 1:listen for unseq data
    void setStatus(int pos, int val);

    int getStatus(int pos);

    ///@brief record the bi-direction relationship between fd and username
    void setUsername(int fd, std::string &username);

    ///@brief get fd's according username
    std::string getUsername(int fd);

    ///@brief get username's according fd
    int getFd(std::string &username);

    void setStartTime(std::time_t new_time, int pos);

    std::time_t getStartTime(int pos);

    struct epoll_event* getEvent();
};
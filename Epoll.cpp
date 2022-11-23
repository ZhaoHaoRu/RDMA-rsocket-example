#include "Epoll.h"
#include <rdma/rsocket.h>

Epoll::Epoll():_epfd(-1),_epoll_events_max_nums(MAX_NUM){
    _events.reset(nullptr);
}

Epoll::~Epoll() {
    if(_epfd > 0) {
        close(_epfd);
    } 
}

int Epoll::setnonblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

int Epoll::rsetnonblocking(int fd)
{
	int old_option = rfcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	rfcntl(fd, F_SETFL, new_option);
	return old_option;
}

bool Epoll::epollCreate() {
    _epfd = epoll_create(1);
    if(_epfd == -1) {
        std::cerr << "epoll create error" << std::endl;
        return false;
    }
    if(_events.get() != nullptr) {
        struct epoll_event *del_event = _events.release();
        delete []del_event;
    }
    _events.reset(new epoll_event[_epoll_events_max_nums]);
    return true;
}

bool Epoll::epollAdd(int fd, int state) {
    struct epoll_event new_event;
    new_event.events = state;
    new_event.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &new_event);
    if(ret < 0) {
        return false;
    } else {
        return true;
    }
}

bool Epoll::epollDel(int fd, int state) {
    struct epoll_event new_event;
    new_event.events = EPOLLIN | EPOLLOUT;
    new_event.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &new_event);
    if(ret < 0) {
        return false;
    } else {
        // delete the fd and username info in two maps
        std::string username;
        if(_username_maps.count(fd)) {
            // std::cout << "earse fd" << fd << "from the username and fd map" << std::endl; 
            username = _username_maps[fd];
            _username_maps.erase(fd);
        }
        if(_fd_maps.count(username)) {
            _fd_maps.erase(username);
        }
        return true;
    }
}

bool Epoll::epollMod(int fd, int state) {
    struct epoll_event new_event;
    new_event.events = state;
    new_event.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &new_event);
    if(ret < 0) {
        return false;
    } else {
        return true;
    }
}

int Epoll::epollWait() {
    int fds = epoll_wait(_epfd, _events.get(), MAX_NUM, -1);
    return fds;
}

void Epoll::setStatus(int pos, int val) {
    // assert(pos >= 0 && pos < MAX_NUM);
    _status[pos] = val;
}

int Epoll::getStatus(int pos) {
    assert(_status.count(pos));
    return _status[pos];
}

void Epoll::setUsername(int fd, std::string &username) {
    _username_maps[fd] = username;
    _fd_maps[username] = fd;
}

std::string Epoll::getUsername(int fd) {
    if(!_username_maps.count(fd)) {
        return "";
    } else {
        return _username_maps[fd];
    }
}

int Epoll::getFd(std::string &username) {
    if(!_fd_maps.count(username)) {
        return -1;
    } else {
        return _fd_maps[username];
    }
}

void Epoll::setStartTime(std::time_t new_time, int pos) {
    // assert(pos >= 0 && pos < MAX_NUM);
    _start_times[pos] = new_time;
}

std::time_t Epoll::getStartTime(int pos) {
    assert(_start_times.count(pos));
    return _start_times[pos];
}


struct epoll_event* Epoll::getEvent() {
    return _events.get();
}
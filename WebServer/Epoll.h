// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"

// 每一个EventLoop都有一个
// 其能够调用channel
// 同时跟tcpconnection
class Epoll {
 public:
  Epoll();
  ~Epoll();
 
  std::vector<std::shared_ptr<Channel>> poll();
  std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num);
  
  void epoll_add(SP_Channel request, int timeout);
  void epoll_mod(SP_Channel request, int timeout);
  void epoll_del(SP_Channel request);
 
  void add_timer(std::shared_ptr<Channel> request_data, int timeout);
  void handleExpired();
  int getEpollFd() { return epollFd_; }
 private:
  static const int MAXFDS = 100000;
  int epollFd_;
  std::vector<epoll_event> events_;
 
  std::shared_ptr<Channel> fd2chan_[MAXFDS];
  std::shared_ptr<HttpData> fd2http_[MAXFDS];
  
  TimerManager timerManager_;
};

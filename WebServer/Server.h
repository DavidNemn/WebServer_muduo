#pragma once
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server {
 public:
  Server(EventLoop *loop, int threadNum, int port);
  ~Server() {}
 
  EventLoop *getLoop() const { return loop_; }
 
  // ****
  void start();
  void handNewConn();
  void handThisConn() { loop_->updatePoller(acceptChannel_); }

 private:
  bool started_;
  int threadNum_;
  EventLoop *loop_;
 
  std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
  int port_;
  int listenFd_;
  std::shared_ptr<Channel> acceptChannel_;

  static const int MAXFDS = 100000;
};

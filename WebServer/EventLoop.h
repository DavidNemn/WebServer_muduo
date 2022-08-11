#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"


#include <iostream>
using namespace std;

class EventLoop {
 public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();
 
  void loop();
  void quit();

  // ***重点
  bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
  void assertInLoopThread() { assert(isInLoopThread()); }
 
  void runInLoop(Functor&& cb); // 
  void queueInLoop(Functor&& cb);// 
 
  void removeFromPoller(shared_ptr<Channel> channel) {
    poller_->epoll_del(channel);
  }
  void updatePoller(shared_ptr<Channel> channel, int timeout = 0) {
    poller_->epoll_mod(channel, timeout);
  }
  void addToPoller(shared_ptr<Channel> channel, int timeout = 0) {
    poller_->epoll_add(channel, timeout);
  }
 
  void shutdown(shared_ptr<Channel> channel) { shutDownWR(channel->getFd()); }

 private:
  // 声明顺序 wakeupFd_ > pwakeupChannel_
  bool looping_;
  bool quit_;
  bool eventHandling_;
 
  shared_ptr<Epoll> poller_;
  int wakeupFd_;
  shared_ptr<Channel> pwakeupChannel_;

  mutable MutexLock mutex_;
  std::vector<Functor> pendingFunctors_;
  bool callingPendingFunctors_;
 
  const pid_t threadId_;


  void wakeup();
  void handleRead();
  void doPendingFunctors();
  void handleConn();
};

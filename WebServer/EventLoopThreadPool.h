#pragma once
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "base/Logging.h"
#include "base/noncopyable.h"

// 所有的reactor
//  EventLoopThread封装thread + EventLoop 
//  thread封装真实的thread
class EventLoopThreadPool : noncopyable {
 public:
  EventLoopThreadPool(EventLoop* baseLoop, int numThreads);
  ~EventLoopThreadPool() { LOG << "~EventLoopThreadPool()"; }
  
  void start();
  EventLoop* getNextLoop();

 private:
  bool started_;
  int numThreads_;
  int next_;
 
  EventLoop* baseLoop_;
  std::vector<std::shared_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop*> loops_;
};

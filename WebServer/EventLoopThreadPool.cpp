#include "EventLoopThreadPool.h"

// round bin 就是循环数组
EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numThreads) : baseLoop_(baseLoop), started_(false), numThreads_(numThreads), next_(0) {
  if (numThreads_ <= 0) {
    LOG << "numThreads_ <= 0";
    abort();
  }
}

// 关注运行这些函数的线程
void EventLoopThreadPool::start() {
  baseLoop_->assertInLoopThread();
  started_ = true;
  for (int i = 0; i < numThreads_; ++i) {
    std::shared_ptr<EventLoopThread> t(new EventLoopThread());
    threads_.push_back(t);
    loops_.push_back(t->startLoop());
  }
}

// 
EventLoop *EventLoopThreadPool::getNextLoop() {
  baseLoop_->assertInLoopThread();
  assert(started_);
  EventLoop *loop = baseLoop_;
  if (!loops_.empty()) {
    loop = loops_[next_];
    next_ = (next_ + 1) % numThreads_;
  }
  return loop;
}

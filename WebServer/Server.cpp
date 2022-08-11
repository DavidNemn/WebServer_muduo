// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Util.h"
#include "base/Logging.h"

Server::Server(EventLoop *loop, int threadNum, int port)
    : loop_(loop),
      threadNum_(threadNum),
      eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
      started_(false),
      acceptChannel_(new Channel(loop_)),
      port_(port),
      listenFd_(socket_bind_listen(port_)) {
    
    // channel -> loop -> addToPoller() -> epoller
    acceptChannel_->setFd(listenFd_);
    handle_for_sigpipe();
    if (setSocketNonBlocking(listenFd_) < 0) {
        perror("set socket non block failed");
        abort();
    }
}

// setConnHandler到底是干嘛的？

void Server::start() {
  // 各个线程开始绑定loop，执行起来，只是为空
  eventLoopThreadPool_->start();

  acceptChannel_->setEvents(EPOLLIN | EPOLLET);
  // acceptChannel_ 绑定的是 Server的函数，用来处理新的连接
  acceptChannel_->setReadHandler(bind(&Server::handNewConn, this));
  acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));
  loop_->addToPoller(acceptChannel_, 0);
    
  started_ = true;
}

void Server::handNewConn() {
    
  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_in));
  socklen_t client_addr_len = sizeof(client_addr);
  int accept_fd = 0;
  // 循环接受
  while ((accept_fd = accept(listenFd_, (struct sockaddr *)&client_addr,&client_addr_len)) > 0) {
    EventLoop *loop = eventLoopThreadPool_->getNextLoop();
      
    LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"<< ntohs(client_addr.sin_port);
    // 限制服务器的最大并发连接数
    if (accept_fd >= MAXFDS) {
      close(accept_fd);
      continue;
    }
    // 设为非阻塞模式
    if (setSocketNonBlocking(accept_fd) < 0) {
      LOG << "Set non block failed!";
      // perror("Set non block failed!");
      return;
    }

    setSocketNodelay(accept_fd);
    // setSocketNoLinger(accept_fd);
    // （loop + accept_fd） -> 建立HttpData
    shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
    req_info->getChannel()->setHolder(req_info);
    // 放到里面执行 也就是子线程的defendingfuntors
    loop->queueInLoop(std::bind(&HttpData::newEvent, req_info));
  }
  acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}

void handThisConn()
{
    loop_->updatePoller(acceptChannel_); 
}
 

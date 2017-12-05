#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <mutex>
#include <queue>
#include <iostream>

#include "redis/redismodule_wrapper.h"

struct event{
    RedisModuleCtx *ctx;
    RedisModuleString **argv;
    int argc;
    RedisModuleBlockedClient * client;
    event(RedisModuleCtx *, RedisModuleString **, int);
    ~event();
    virtual void execute() = 0;
};


//singleton wrapper for fifo std::queue
class event_queue{

    public:
        std::queue<std::shared_ptr<event>> events;
        std::mutex queue_mutex;

        static event_queue& getInstance();

        void enqueue(std::shared_ptr<event> const & e);
        std::shared_ptr<event> dequeue();

        template <typename E> static int handler(RedisModuleCtx *, RedisModuleString **, int);

        event_queue(event_queue const &) = delete;//prevents these functions from being called
        void operator = (event_queue const &) = delete; 
        event_queue(event_queue &&) = delete;
        void operator = (event_queue &&) = delete;

    private:
        event_queue(){ }
};

template<typename E>
int event_queue::handler(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    event_queue& eq = event_queue::getInstance();
    std::shared_ptr<E> e(new E(ctx, argv, argc));
    eq.enqueue(e);
    return REDISMODULE_OK;
    //this return is why we need to block the client, the return will basically tell the server that the module has completed the computation,
    //therefore we block and tell the server to save the client information and forbid client to send anymore requests, once the enqueued event is completed
    //we can return the results to the blocked client and then unblock the client. 
}  

#endif

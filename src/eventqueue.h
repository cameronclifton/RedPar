#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <mutex>
#include <queue>

#include "redis/redismodule_wrapper.h"

struct event{
    RedisModuleCtx *ctx;
    RedisModuleString **argv;
    int argc;
    RedisModuleBlockedClient * client;
    event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc, RedisModuleBlockedClient* client);
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

        event_queue(event_queue const &) = delete;//prevents these functions from being called
        void operator = (event_queue const &) = delete; 
        event_queue(event_queue &&) = delete;
        void operator = (event_queue &&) = delete;

    private:
        event_queue(){ }
};

int reply_func(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

int timeout_func(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

template<typename E>
int handler(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModuleBlockedClient * client = RedisModule_BlockClient(ctx, reply_func, timeout_func, NULL, 0);
    event_queue& eq = event_queue::getInstance();
    std::shared_ptr<E> e(new E{ctx, argv, argc, client});
    eq.enqueue(e);
    return REDISMODULE_OK;
}  

#endif

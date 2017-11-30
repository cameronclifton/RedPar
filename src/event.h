#ifndef EVENT_H
#define EVENT_H

#include "redis/redismodule_wrapper.h"

struct event{
    RedisModuleCtx *ctx;
    RedisModuleString **argv;
    int argc;
    event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc);
    //event(event const &) = delete;
    //void operator = (event const &) = delete; 
    //event(event &&) = delete;
    //void operator = (event &&) = delete;


    virtual void execute() = 0;
};

#endif

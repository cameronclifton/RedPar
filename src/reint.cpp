#include "reint.h"
#include "eventqueue.h"
#include "llca.h"
#include "redis/redismodule_wrapper.h"
#include <thread>
#include <iostream>
#include <vector>

#define CA_THREAD_COUNT 4

static std::vector<std::thread> thread_pool;

void work(){
    event_queue& eq = event_queue::getInstance();
    while(true){
        std::shared_ptr<event> e = eq.dequeue();
        if(e != nullptr){
            e->execute();
	    RedisModule_FreeThreadSafeContext(e->ctx);
	}
    }
}


extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (Export_RedisModule_Init(ctx, "CAMAP", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if(LLCA_OnLoad(ctx)!=REDISMODULE_OK){
      return REDISMODULE_ERR;
    }

    for( int i =0; i < CA_THREAD_COUNT; ++i){
        thread_pool.emplace_back(work);
    }

    return REDISMODULE_OK;
}


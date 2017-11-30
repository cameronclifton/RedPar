#include "reint.h"
#include "event.h"
#include "eventqueue.h"
#include "llca.h"
#include "redis/redismodule_wrapper.h"
#include <thread>




void work(){
    event_queue& eq = event_queue::getInstance();
    while(true){
        std::shared_ptr<event> e = eq.dequeue();
        if(e != nullptr){
            e->execute();
        }
    }
}



extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    // Register the module itself – it’s called example and has an API version of 1
    if (Export_RedisModule_Init(ctx, "CAMAP", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if(LLCA_OnLoad(ctx)!=REDISMODULE_OK){
      return REDISMODULE_ERR;
    }

    for( int i =0; i < THREAD_COUNT; ++i){
        std::thread(work);
    }
    return REDISMODULE_OK;
}


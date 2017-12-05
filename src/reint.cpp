#include "reint.h"
#include "eventqueue.h"
#include "llca.h"
#include "llca_conc.h"
#include "redis/redismodule_wrapper.h"

#include <thread>
#include <iostream>
#include <vector>

#include <cds/init.h>
#include <cds/gc/hp.h>

#define CA_THREAD_COUNT 4

void work(){
    cds::threading::Manager::attachThread();
    event_queue& eq = event_queue::getInstance();
    while(true){
        std::shared_ptr<event> e = eq.dequeue();
        if(e != nullptr){
            e->execute();
	    }
    }
}


extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (Export_RedisModule_Init(ctx, "CA_multithread", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if(LLCA_OnLoad(ctx)!=REDISMODULE_OK){
      return REDISMODULE_ERR;
    }

    if(LLCA_Conc_OnLoad(ctx)!=REDISMODULE_OK){
        return REDISMODULE_ERR;
    }

    //Garbage collection initialization for cds skip list
    cds::Initialize();
    static cds::gc::HP hpGC(67);

    static std::vector<std::thread> thread_pool;
    for( int i =0; i < CA_THREAD_COUNT; ++i){
        thread_pool.emplace_back(work);
    }

    return REDISMODULE_OK;
}


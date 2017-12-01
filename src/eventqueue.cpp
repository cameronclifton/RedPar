
#include "eventqueue.h"

event::event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc, RedisModuleBlockedClient* client): ctx(ctx), argv(argv), argc(argc), client(client) {}

event_queue& event_queue::getInstance(){ //thread safe?
    static event_queue instance;//initialized once and guaranteed to be destroyed
    return instance;
}

void event_queue::enqueue(std::shared_ptr<event> const & e){
    std::lock_guard<std::mutex> lg(queue_mutex);
    events.push(e);
}

std::shared_ptr<event> event_queue::dequeue(){
    std::lock_guard<std::mutex> lg(queue_mutex);
    if(!events.empty()){
        std::shared_ptr<event> e = events.front();
        events.pop();
        return e;
    }else{
        return nullptr;
    }
}

int reply_func(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    RedisModuleString* result = (RedisModuleString*) RedisModule_GetBlockedClientPrivateData(ctx);
    return RedisModule_ReplyWithString(ctx,result);
}

int timeout_func(RedisModuleCtx *ctx, RedisModuleString **argv, int argc){
    return RedisModule_ReplyWithNull(ctx);
}

void free_privdata(void *privdata){
    RedisModule_Free(privdata);
}

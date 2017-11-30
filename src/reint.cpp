#include "reint.h"
#include "event.h"
#include "redis/redismodule_wrapper.h"

#include <queue>
#include <thread>
#include <mutex>
#include <system_error>

struct llca_insert_event: public event {
    llca_insert_event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc): event(ctx,argv,argc){};
    virtual void execute(){
        if(argc != 2){
            //RedisModule_WrongArity(ctx);
        }
 
        RedisModule_ReplyWithLongLong(ctx,1);
    };    
};

//singleton wrapper for fifo std::queue
class event_queue{

    public:
        std::queue<std::shared_ptr<event>> events;
        std::mutex queue_mutex;

        static event_queue& getInstance(){ //thread safe?
            static event_queue instance;//initialized once and guaranteed to be destroyed
            return instance;
        }

        void enqueue(std::shared_ptr<event> const & e){
            std::lock_guard<std::mutex> lg(queue_mutex);
            events.push(e);
        }

        std::shared_ptr<event> dequeue(){
            std::lock_guard<std::mutex> lg(queue_mutex);
            if(!events.empty()){
                std::shared_ptr<event> e = events.front();
                events.pop();
                return e;
            }else{
                return nullptr;
            }
        }

        event_queue(event_queue const &) = delete;//prevents these functions from being called
        void operator = (event_queue const &) = delete; 
        event_queue(event_queue &&) = delete;
        void operator = (event_queue &&) = delete;

    private:
        event_queue(){ }
};


void work(){
    event_queue& eq = event_queue::getInstance();
    while(true){
        std::shared_ptr<event> e = eq.dequeue();
        if(e != nullptr){
            e->execute();
        }
    }
}


/* reint.set KEY VALUE */
int reint_set(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    event_queue& eq = event_queue::getInstance();
    std::shared_ptr<llca_insert_event> lie(new llca_insert_event(ctx, argv, argc));
    eq.enqueue(lie);
    return 1;
}

/*
int reint_get(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 2){
        RedisModule_WrongArity(ctx);
    }

    RedisModule_ReplyWithLongLong(ctx,1);
    return REDISMODULE_OK;
}

int reint_incr(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 3){
        RedisModule_WrongArity(ctx);
    }

    RedisModule_ReplyWithLongLong(ctx,1);
    return REDISMODULE_OK;
}

int reint_decr(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 3){
        RedisModule_WrongArity(ctx);
    }

    RedisModule_ReplyWithLongLong(ctx,1);
    return REDISMODULE_OK;
}
*/
extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    // Register the module itself – it’s called example and has an API version of 1
    if (Export_RedisModule_Init(ctx, "CAMAP", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    /*if (RedisModule_CreateCommand(ctx, "CAMAP.create", reint_set, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "CAMAP.insert", reint_get, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "CAMAP.remove", reint_incr, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "CAMAP.contains", reint_decr, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    for( int i =0; i < THREAD_COUNT; ++i){
        std::thread(work);
    }
    return REDISMODULE_OK;
    */
    return REDISMODULE_OK;
}


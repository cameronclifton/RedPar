#include "reint.h"

struct event{
  RedisModuleCtx *ctx;
  RedisModuleString **argv;
  int argc;
  event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc):ctx(ctx),argv(argv),argc(argc) {};
};

//singleton wrapper for fifo std::queue
class event_queue{
  
public:
  std::queue<event> events;

  std::mutex queue_mutex;
 
  static event_queue& getInstance(){
    
    static event_queue instance;//initialized once and guaranteed to be destroyed
    
    return instance;
  }
  
  event_queue(event_queue const &) = delete;//prevents these functions from being called
  void operator = (event_queue const &) = delete; 
  
private:
  event_queue(){ }

};


void work(){

  while(true){
    
    event_queue& eq = event_queue::getInstance();
    if(eq.queue_mutex.try_lock() == true){
      if(eq.events.empty() == true){
	eq.queue_mutex.unlock();
      }
      else{
	event e = eq.events.front();
	//do something based on e attributes
	eq.events.pop();
	eq.queue_mutex.unlock();
      }
    }
     

      
    }
     //switch for all operations
  
}


/* reint.set KEY VALUE */
int reint_set(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 3){
        RedisModule_WrongArity(ctx);
    }
    
    RedisModule_ReplyWithLongLong(ctx,1);

    return REDISMODULE_OK;
}

/* redis.get KEY */
int reint_get(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 2){
        RedisModule_WrongArity(ctx);
    }

    RedisModule_ReplyWithLongLong(ctx,1);
    return REDISMODULE_OK;
}

/* redis.incr KEY VALUE */
int reint_incr(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 3){
        RedisModule_WrongArity(ctx);
    }

    RedisModule_ReplyWithLongLong(ctx,1);
    return REDISMODULE_OK;
}

/* redis.decr KEY VALUE */
int reint_decr(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    if(argc != 3){
        RedisModule_WrongArity(ctx);
    }

    RedisModule_ReplyWithLongLong(ctx,1);
    return REDISMODULE_OK;
}

extern "C" int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    // Register the module itself – it’s called example and has an API version of 1
    if (RedisModule_Init(ctx, "example", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "reint.set", reint_set, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    if (RedisModule_CreateCommand(ctx, "reint.get", reint_get, "readonly", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

 if (RedisModule_CreateCommand(ctx, "reint.incr", reint_incr, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

 if (RedisModule_CreateCommand(ctx, "reint.decr", reint_decr, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
 
 for( int i =0; i < THREAD_COUNT; ++i){
   std::thread(work);
 }
 
 return REDISMODULE_OK;
}


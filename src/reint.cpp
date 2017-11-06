#include "reint.h"

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

    return REDISMODULE_OK;
}


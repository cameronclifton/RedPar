#include "event.h"

event::event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc): ctx(ctx), argv(argv), argc(argc) 
{
    RedisModule_ReplyWithLongLong(ctx,1);
}

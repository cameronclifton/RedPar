#include "event.h"

event::event(RedisModuleCtx *ctx, RedisModuleString **argv, int argc): ctx(ctx), argv(argv), argc(argc) {}

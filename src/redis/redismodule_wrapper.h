#define REDISMODULE_OK 0
#define REDISMODULE_ERR 1

#define REDISMODULE_APIVER_1 1

typedef struct RedisModuleCtx RedisModuleCtx;
typedef struct RedisModuleKey RedisModuleKey;
typedef struct RedisModuleString RedisModuleString;
typedef struct RedisModuleCallReply RedisModuleCallReply;
typedef struct RedisModuleIO RedisModuleIO;
typedef struct RedisModuleType RedisModuleType;
typedef struct RedisModuleDigest RedisModuleDigest;
typedef struct RedisModuleBlockedClient RedisModuleBlockedClient;

extern "C" int RedisModule_ReplyWithLongLong(RedisModuleCtx*,long long); 

extern "C" int Export_RedisModule_Init(RedisModuleCtx *ctx, const char *name, int ver, int apiver); 

#ifndef REDISMODULE_WRAPPER_H
#define REDISMODULE_WRAPPER_H

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>

/* ---------------- Defines common between core and modules --------------- */

/* Error status return values. */
#define REDISMODULE_OK 0
#define REDISMODULE_ERR 1

/* API versions. */
#define REDISMODULE_APIVER_1 1

/* API flags and constants */
#define REDISMODULE_READ (1<<0)
#define REDISMODULE_WRITE (1<<1)

#define REDISMODULE_LIST_HEAD 0
#define REDISMODULE_LIST_TAIL 1

/* Key types. */
#define REDISMODULE_KEYTYPE_EMPTY 0
#define REDISMODULE_KEYTYPE_STRING 1
#define REDISMODULE_KEYTYPE_LIST 2
#define REDISMODULE_KEYTYPE_HASH 3
#define REDISMODULE_KEYTYPE_SET 4
#define REDISMODULE_KEYTYPE_ZSET 5
#define REDISMODULE_KEYTYPE_MODULE 6

/* Reply types. */
#define REDISMODULE_REPLY_UNKNOWN -1
#define REDISMODULE_REPLY_STRING 0
#define REDISMODULE_REPLY_ERROR 1
#define REDISMODULE_REPLY_INTEGER 2
#define REDISMODULE_REPLY_ARRAY 3
#define REDISMODULE_REPLY_NULL 4

/* Postponed array length. */
#define REDISMODULE_POSTPONED_ARRAY_LEN -1

/* Expire */
#define REDISMODULE_NO_EXPIRE -1

/* Sorted set API flags. */
#define REDISMODULE_ZADD_XX      (1<<0)
#define REDISMODULE_ZADD_NX      (1<<1)
#define REDISMODULE_ZADD_ADDED   (1<<2)
#define REDISMODULE_ZADD_UPDATED (1<<3)
#define REDISMODULE_ZADD_NOP     (1<<4)

/* Hash API flags. */
#define REDISMODULE_HASH_NONE       0
#define REDISMODULE_HASH_NX         (1<<0)
#define REDISMODULE_HASH_XX         (1<<1)
#define REDISMODULE_HASH_CFIELDS    (1<<2)
#define REDISMODULE_HASH_EXISTS     (1<<3)

/* Context Flags: Info about the current context returned by RM_GetContextFlags */

/* The command is running in the context of a Lua script */
#define REDISMODULE_CTX_FLAGS_LUA 0x0001
/* The command is running inside a Redis transaction */
#define REDISMODULE_CTX_FLAGS_MULTI 0x0002
/* The instance is a master */
#define REDISMODULE_CTX_FLAGS_MASTER 0x0004
/* The instance is a slave */
#define REDISMODULE_CTX_FLAGS_SLAVE 0x0008
/* The instance is read-only (usually meaning it's a slave as well) */
#define REDISMODULE_CTX_FLAGS_READONLY 0x0010
/* The instance is running in cluster mode */
#define REDISMODULE_CTX_FLAGS_CLUSTER 0x0020
/* The instance has AOF enabled */
#define REDISMODULE_CTX_FLAGS_AOF 0x0040 //
/* The instance has RDB enabled */
#define REDISMODULE_CTX_FLAGS_RDB 0x0080 //
/* The instance has Maxmemory set */
#define REDISMODULE_CTX_FLAGS_MAXMEMORY 0x0100
/* Maxmemory is set and has an eviction policy that may delete keys */
#define REDISMODULE_CTX_FLAGS_EVICT 0x0200 


/* A special pointer that we can use between the core and the module to signal
 * field deletion, and that is impossible to be a valid pointer. */
#define REDISMODULE_HASH_DELETE ((RedisModuleString*)(long)1)

/* Error messages. */
#define REDISMODULE_ERRORMSG_WRONGTYPE "WRONGTYPE Operation against a key holding the wrong kind of value"

#define REDISMODULE_POSITIVE_INFINITE (1.0/0.0)
#define REDISMODULE_NEGATIVE_INFINITE (-1.0/0.0)

#define REDISMODULE_NOT_USED(V) ((void) V)

/* ------------------------- End of common defines ------------------------ */

#ifndef REDISMODULE_CORE

typedef long long mstime_t;

/* Incomplete structures for compiler checks but opaque access. */
typedef struct RedisModuleCtx RedisModuleCtx;
typedef struct RedisModuleKey RedisModuleKey;
typedef struct RedisModuleString RedisModuleString;
typedef struct RedisModuleCallReply RedisModuleCallReply;
typedef struct RedisModuleIO RedisModuleIO;
typedef struct RedisModuleType RedisModuleType;
typedef struct RedisModuleDigest RedisModuleDigest;
typedef struct RedisModuleBlockedClient RedisModuleBlockedClient;

typedef int (*RedisModuleCmdFunc) (RedisModuleCtx *ctx, RedisModuleString **argv, int argc);

typedef void *(*RedisModuleTypeLoadFunc)(RedisModuleIO *rdb, int encver);
typedef void (*RedisModuleTypeSaveFunc)(RedisModuleIO *rdb, void *value);
typedef void (*RedisModuleTypeRewriteFunc)(RedisModuleIO *aof, RedisModuleString *key, void *value);
typedef size_t (*RedisModuleTypeMemUsageFunc)(const void *value);
typedef void (*RedisModuleTypeDigestFunc)(RedisModuleDigest *digest, void *value);
typedef void (*RedisModuleTypeFreeFunc)(void *value);

#define REDISMODULE_TYPE_METHOD_VERSION 1
typedef struct RedisModuleTypeMethods {
    uint64_t version;
    RedisModuleTypeLoadFunc rdb_load;
    RedisModuleTypeSaveFunc rdb_save;
    RedisModuleTypeRewriteFunc aof_rewrite;
    RedisModuleTypeMemUsageFunc mem_usage;
    RedisModuleTypeDigestFunc digest;
    RedisModuleTypeFreeFunc free;
} RedisModuleTypeMethods;

#define REDISMODULE_GET_API(name) \
    RedisModule_GetApi("RedisModule_" #name, ((void **)&RedisModule_ ## name))

#define REDISMODULE_API_FUNC(x) (*x)


extern "C" void *(RedisModule_Alloc)(size_t bytes);
extern "C" void *(RedisModule_Realloc)(void *ptr, size_t bytes);
extern "C" void (RedisModule_Free)(void *ptr);
extern "C" void *(RedisModule_Calloc)(size_t nmemb, size_t size);
extern "C" char *(RedisModule_Strdup)(const char *str);
extern "C" int (RedisModule_GetApi)(const char *, void *);
extern "C" int (RedisModule_CreateCommand)(RedisModuleCtx *ctx, const char *name, RedisModuleCmdFunc cmdfunc, const char *strflags, int firstkey, int lastkey, int keystep);
extern "C" int (RedisModule_SetModuleAttribs)(RedisModuleCtx *ctx, const char *name, int ver, int apiver);
extern "C" int (RedisModule_WrongArity)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_ReplyWithLongLong)(RedisModuleCtx *ctx, long long ll);
extern "C" int (RedisModule_GetSelectedDb)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_SelectDb)(RedisModuleCtx *ctx, int newid);
extern "C" void *(RedisModule_OpenKey)(RedisModuleCtx *ctx, RedisModuleString *keyname, int mode);
extern "C" void (RedisModule_CloseKey)(RedisModuleKey *kp);
extern "C" int (RedisModule_KeyType)(RedisModuleKey *kp);
extern "C" size_t (RedisModule_ValueLength)(RedisModuleKey *kp);
extern "C" int (RedisModule_ListPush)(RedisModuleKey *kp, int where, RedisModuleString *ele);
extern "C" RedisModuleString *(RedisModule_ListPop)(RedisModuleKey *key, int where);
extern "C" RedisModuleCallReply *(RedisModule_Call)(RedisModuleCtx *ctx, const char *cmdname, const char *fmt, ...);
extern "C" const char *(RedisModule_CallReplyProto)(RedisModuleCallReply *reply, size_t *len);
extern "C" void (RedisModule_FreeCallReply)(RedisModuleCallReply *reply);
extern "C" int (RedisModule_CallReplyType)(RedisModuleCallReply *reply);
extern "C" long long (RedisModule_CallReplyInteger)(RedisModuleCallReply *reply);
extern "C" size_t (RedisModule_CallReplyLength)(RedisModuleCallReply *reply);
extern "C" RedisModuleCallReply *(RedisModule_CallReplyArrayElement)(RedisModuleCallReply *reply, size_t idx);
extern "C" RedisModuleString *(RedisModule_CreateString)(RedisModuleCtx *ctx, const char *ptr, size_t len);
extern "C" RedisModuleString *(RedisModule_CreateStringFromLongLong)(RedisModuleCtx *ctx, long long ll);
extern "C" RedisModuleString *(RedisModule_CreateStringFromString)(RedisModuleCtx *ctx, const RedisModuleString *str);
extern "C" RedisModuleString *(RedisModule_CreateStringPrintf)(RedisModuleCtx *ctx, const char *fmt, ...);
extern "C" void (RedisModule_FreeString)(RedisModuleCtx *ctx, RedisModuleString *str);
extern "C" const char *(RedisModule_StringPtrLen)(const RedisModuleString *str, size_t *len);
extern "C" int (RedisModule_ReplyWithError)(RedisModuleCtx *ctx, const char *err);
extern "C" int (RedisModule_ReplyWithSimpleString)(RedisModuleCtx *ctx, const char *msg);
extern "C" int (RedisModule_ReplyWithArray)(RedisModuleCtx *ctx, long len);
extern "C" void (RedisModule_ReplySetArrayLength)(RedisModuleCtx *ctx, long len);
extern "C" int (RedisModule_ReplyWithStringBuffer)(RedisModuleCtx *ctx, const char *buf, size_t len);
extern "C" int (RedisModule_ReplyWithString)(RedisModuleCtx *ctx, RedisModuleString *str);
extern "C" int (RedisModule_ReplyWithNull)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_ReplyWithDouble)(RedisModuleCtx *ctx, double d);
extern "C" int (RedisModule_ReplyWithCallReply)(RedisModuleCtx *ctx, RedisModuleCallReply *reply);
extern "C" int (RedisModule_StringToLongLong)(const RedisModuleString *str, long long *ll);
extern "C" int (RedisModule_StringToDouble)(const RedisModuleString *str, double *d);
extern "C" void (RedisModule_AutoMemory)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_Replicate)(RedisModuleCtx *ctx, const char *cmdname, const char *fmt, ...);
extern "C" int (RedisModule_ReplicateVerbatim)(RedisModuleCtx *ctx);
extern "C" const char *(RedisModule_CallReplyStringPtr)(RedisModuleCallReply *reply, size_t *len);
extern "C" RedisModuleString *(RedisModule_CreateStringFromCallReply)(RedisModuleCallReply *reply);
extern "C" int (RedisModule_DeleteKey)(RedisModuleKey *key);
extern "C" int (RedisModule_StringSet)(RedisModuleKey *key, RedisModuleString *str);
extern "C" char *(RedisModule_StringDMA)(RedisModuleKey *key, size_t *len, int mode);
extern "C" int (RedisModule_StringTruncate)(RedisModuleKey *key, size_t newlen);
extern "C" mstime_t (RedisModule_GetExpire)(RedisModuleKey *key);
extern "C" int (RedisModule_SetExpire)(RedisModuleKey *key, mstime_t expire);
extern "C" int (RedisModule_ZsetAdd)(RedisModuleKey *key, double score, RedisModuleString *ele, int *flagsptr);
extern "C" int (RedisModule_ZsetIncrby)(RedisModuleKey *key, double score, RedisModuleString *ele, int *flagsptr, double *newscore);
extern "C" int (RedisModule_ZsetScore)(RedisModuleKey *key, RedisModuleString *ele, double *score);
extern "C" int (RedisModule_ZsetRem)(RedisModuleKey *key, RedisModuleString *ele, int *deleted);
extern "C" void (RedisModule_ZsetRangeStop)(RedisModuleKey *key);
extern "C" int (RedisModule_ZsetFirstInScoreRange)(RedisModuleKey *key, double min, double max, int minex, int maxex);
extern "C" int (RedisModule_ZsetLastInScoreRange)(RedisModuleKey *key, double min, double max, int minex, int maxex);
extern "C" int (RedisModule_ZsetFirstInLexRange)(RedisModuleKey *key, RedisModuleString *min, RedisModuleString *max);
extern "C" int (RedisModule_ZsetLastInLexRange)(RedisModuleKey *key, RedisModuleString *min, RedisModuleString *max);
extern "C" RedisModuleString *(RedisModule_ZsetRangeCurrentElement)(RedisModuleKey *key, double *score);
extern "C" int (RedisModule_ZsetRangeNext)(RedisModuleKey *key);
extern "C" int (RedisModule_ZsetRangePrev)(RedisModuleKey *key);
extern "C" int (RedisModule_ZsetRangeEndReached)(RedisModuleKey *key);
extern "C" int (RedisModule_HashSet)(RedisModuleKey *key, int flags, ...);
extern "C" int (RedisModule_HashGet)(RedisModuleKey *key, int flags, ...);
extern "C" int (RedisModule_IsKeysPositionRequest)(RedisModuleCtx *ctx);
extern "C" void (RedisModule_KeyAtPos)(RedisModuleCtx *ctx, int pos);
extern "C" unsigned long long (RedisModule_GetClientId)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_GetContextFlags)(RedisModuleCtx *ctx);
extern "C" void *(RedisModule_PoolAlloc)(RedisModuleCtx *ctx, size_t bytes);
extern "C" RedisModuleType *(RedisModule_CreateDataType)(RedisModuleCtx *ctx, const char *name, int encver, RedisModuleTypeMethods *typemethods);
extern "C" int (RedisModule_ModuleTypeSetValue)(RedisModuleKey *key, RedisModuleType *mt, void *value);
extern "C" RedisModuleType *(RedisModule_ModuleTypeGetType)(RedisModuleKey *key);
extern "C" void *(RedisModule_ModuleTypeGetValue)(RedisModuleKey *key);
extern "C" void (RedisModule_SaveUnsigned)(RedisModuleIO *io, uint64_t value);
extern "C" uint64_t (RedisModule_LoadUnsigned)(RedisModuleIO *io);
extern "C" void (RedisModule_SaveSigned)(RedisModuleIO *io, int64_t value);
extern "C" int64_t (RedisModule_LoadSigned)(RedisModuleIO *io);
extern "C" void (RedisModule_EmitAOF)(RedisModuleIO *io, const char *cmdname, const char *fmt, ...);
extern "C" void (RedisModule_SaveString)(RedisModuleIO *io, RedisModuleString *s);
extern "C" void (RedisModule_SaveStringBuffer)(RedisModuleIO *io, const char *str, size_t len);
extern "C" RedisModuleString *(RedisModule_LoadString)(RedisModuleIO *io);
extern "C" char *(RedisModule_LoadStringBuffer)(RedisModuleIO *io, size_t *lenptr);
extern "C" void (RedisModule_SaveDouble)(RedisModuleIO *io, double value);
extern "C" double (RedisModule_LoadDouble)(RedisModuleIO *io);
extern "C" void (RedisModule_SaveFloat)(RedisModuleIO *io, float value);
extern "C" float (RedisModule_LoadFloat)(RedisModuleIO *io);
extern "C" void (RedisModule_Log)(RedisModuleCtx *ctx, const char *level, const char *fmt, ...);
extern "C" void (RedisModule_LogIOError)(RedisModuleIO *io, const char *levelstr, const char *fmt, ...);
extern "C" int (RedisModule_StringAppendBuffer)(RedisModuleCtx *ctx, RedisModuleString *str, const char *buf, size_t len);
extern "C" void (RedisModule_RetainString)(RedisModuleCtx *ctx, RedisModuleString *str);
extern "C" int (RedisModule_StringCompare)(RedisModuleString *a, RedisModuleString *b);
extern "C" RedisModuleCtx *(RedisModule_GetContextFromIO)(RedisModuleIO *io);
extern "C" long long (RedisModule_Milliseconds)(void);
extern "C" void (RedisModule_DigestAddStringBuffer)(RedisModuleDigest *md, unsigned char *ele, size_t len);
extern "C" void (RedisModule_DigestAddLongLong)(RedisModuleDigest *md, long long ele);
extern "C" void (RedisModule_DigestEndSequence)(RedisModuleDigest *md);

/* Experimental APIs */
#ifdef REDISMODULE_EXPERIMENTAL_API
extern "C" RedisModuleBlockedClient *(RedisModule_BlockClient)(RedisModuleCtx *ctx, RedisModuleCmdFunc reply_callback, RedisModuleCmdFunc timeout_callback, void (*free_privdata)(void*), long long timeout_ms);
extern "C" int (RedisModule_UnblockClient)(RedisModuleBlockedClient *bc, void *privdata);
extern "C" int (RedisModule_IsBlockedReplyRequest)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_IsBlockedTimeoutRequest)(RedisModuleCtx *ctx);
extern "C" void *(RedisModule_GetBlockedClientPrivateData)(RedisModuleCtx *ctx);
extern "C" int (RedisModule_AbortBlock)(RedisModuleBlockedClient *bc);
extern "C" RedisModuleCtx *(RedisModule_GetThreadSafeContext)(RedisModuleBlockedClient *bc);
extern "C" void (RedisModule_FreeThreadSafeContext)(RedisModuleCtx *ctx);
extern "C" void (RedisModule_ThreadSafeContextLock)(RedisModuleCtx *ctx);
extern "C" void (RedisModule_ThreadSafeContextUnlock)(RedisModuleCtx *ctx);
#endif

/* This is included inline inside each Redis module. */
extern "C" int Export_RedisModule_Init(RedisModuleCtx *ctx, const char *name, int ver, int apiver) __attribute__((unused));

#else

/* Things only defined for the modules core, not exported to modules
 * including this file. */
#define RedisModuleString robj

#endif /* REDISMODULE_CORE */
#endif /* REDISMOUDLE_H */

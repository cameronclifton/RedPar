#include "llca_conc.h"
#include "eventqueue.h"
#include <cds/container/skip_list_set_hp.h>

#include <iostream>

using cds::container::SkipListSet;
typedef SkipListSet<cds::gc::HP, int64_t> SkipListSet_impl;

static RedisModuleType * llca_conc;

struct llca_conc_obj{
    SkipListSet_impl sl; 
    bool insert(int64_t);
    bool remove(int64_t);
    bool contains(int64_t);
};

bool llca_conc_obj::insert(int64_t val){
    return sl.insert(val); 
}

bool llca_conc_obj::remove(int64_t val){
    return sl.erase(val);
}

bool llca_conc_obj::contains(int64_t val){
    return sl.contains(val);
}

bool check_arity(RedisModuleCtx* ctx, int argc, int expected){
    if (argc != expected){
        RedisModule_ReplyWithError(ctx, "ERR: wrong arity");
        return false;
    }
    return true;
}

bool string_to_longlong(RedisModuleCtx* ctx, RedisModuleString* str, long long &value){
    if ((RedisModule_StringToLongLong(str,&value) != REDISMODULE_OK)) {
        RedisModule_ReplyWithError(ctx,"ERR invalid value: must be a signed 64 bit integer");
        return false;
    }
    return true;
}

llca_conc_obj* lookup_key(RedisModuleCtx* ctx, RedisModuleString* key_str, bool thread_safe){
    if(!thread_safe){ RedisModule_ThreadSafeContextLock(ctx); }
    RedisModuleKey *key = (RedisModuleKey*) RedisModule_OpenKey(ctx,key_str,REDISMODULE_READ|REDISMODULE_WRITE);
    if(!thread_safe){ RedisModule_ThreadSafeContextUnlock(ctx); }
    int type = RedisModule_KeyType(key); 
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != llca_conc){
        RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
        return nullptr;
    }
    if(type == REDISMODULE_KEYTYPE_EMPTY){
        RedisModule_ReplyWithError(ctx,"ERR: no list exists for that key");
        return nullptr;
    }
    return (llca_conc_obj*) RedisModule_ModuleTypeGetValue(key);
}

/* "llca" type methods */
void* llca_conc_rdb_load(RedisModuleIO* rdb, int encver){
    //TODO
    return NULL;
}

void llca_conc_rdb_save(RedisModuleIO *rdb, void* value) {
    //TODO
}

void llca_conc_aof_rewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
    //TODO
}

size_t llca_conc_mem_usage(const void *value) {
    //TODO
    return 0;
}

void llca_conc_free(void *value) {
    //TODO
}

void llca_conc_digest(RedisModuleDigest *md, void *value) {
    //TODO
}

struct llca_conc_create_event: public event{
    using event::event;
    void execute(){
        if(!check_arity(ctx, argc, 2)){
            return;
        }
        if(!thread_safe){ RedisModule_ThreadSafeContextLock(ctx); }
        RedisModuleKey* key =  (RedisModuleKey*)RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ|REDISMODULE_WRITE);
        int type = RedisModule_KeyType(key); 
        if(type != REDISMODULE_KEYTYPE_EMPTY){
            RedisModule_ReplyWithSimpleString(ctx,"ERR: key already exists");
            if(!thread_safe){ RedisModule_ThreadSafeContextUnlock(ctx); }
            return;
        }
        void * ll_address = RedisModule_Alloc(sizeof(llca_conc_obj));
        llca_conc_obj * ll = new (ll_address) llca_conc_obj();
        RedisModule_ModuleTypeSetValue(key,llca_conc,ll);
        if(!thread_safe){ RedisModule_ThreadSafeContextUnlock(ctx); }
        RedisModule_ReplyWithSimpleString(ctx,"OK");
    }
};

struct llca_conc_delete_event: public event{
    using event::event;
    void execute(){
        if(!check_arity(ctx, argc, 2)){
            return;
        }
        if(!thread_safe) { RedisModule_ThreadSafeContextLock(ctx); }
        RedisModuleKey* key =  (RedisModuleKey*)RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ|REDISMODULE_WRITE);
        int type = RedisModule_KeyType(key); 
        if(type == REDISMODULE_KEYTYPE_EMPTY){
            RedisModule_ReplyWithSimpleString(ctx,"ERR: key does not exist");
            if(!thread_safe){ RedisModule_ThreadSafeContextUnlock(ctx); }
            return; 
        }
        if (RedisModule_ModuleTypeGetType(key) != llca_conc){
            RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
            if(!thread_safe){ RedisModule_ThreadSafeContextUnlock(ctx); }
            return; 
        }
        llca_conc_obj * ll = (llca_conc_obj*) RedisModule_ModuleTypeGetValue(key); 
        ll->sl.~SkipListSet_impl();
        RedisModule_Free(ll);
        RedisModule_DeleteKey(key);
        if(!thread_safe){ RedisModule_ThreadSafeContextUnlock(ctx); }
        RedisModule_ReplyWithSimpleString(ctx,"OK");
    }
};

struct llca_conc_remove_event: public event{
    using event::event;
    void execute(){
        if(!check_arity(ctx, argc, 3)){
            return;
        }
        long long value;
        if(!string_to_longlong(ctx,argv[2],value)){
            return;
        }
        llca_conc_obj* obj = lookup_key(ctx, argv[1], thread_safe);
        if(obj == nullptr){
            return;
        }
        if(obj->remove(value)){
            RedisModule_ReplyWithSimpleString(ctx,"OK");
        }else{
            RedisModule_ReplyWithSimpleString(ctx,"ERR: value does not exist");
        }
    }
};

struct llca_conc_contains_event: public event{
    using event::event;
    void execute(){
        if(!check_arity(ctx, argc, 3)){
            return;
        }
        long long value;
        if(!string_to_longlong(ctx,argv[2],value)){
            return;
        }
        llca_conc_obj* obj = lookup_key(ctx, argv[1], thread_safe);
        if(obj == nullptr){
            return;
        }
        if(obj->contains(value)){
            RedisModule_ReplyWithSimpleString(ctx,"OK");
        }else{
            RedisModule_ReplyWithSimpleString(ctx,"ERR: value does not exist");
        }
    }
};

struct llca_conc_insert_event: public event {
    using event::event;
    void execute(){
        if(!check_arity(ctx, argc, 3)){
            return;
        }
        long long value;
        if(!string_to_longlong(ctx,argv[2],value)){
            return;
        }
        llca_conc_obj* obj = lookup_key(ctx, argv[1], thread_safe);
        if(obj == nullptr){
            return;
        }
        if(obj->insert(value)){
            RedisModule_ReplyWithSimpleString(ctx,"OK");
        }else{
            RedisModule_ReplyWithSimpleString(ctx,"ERR: value exists");
        }
    }      
};

int LLCA_Conc_OnLoad(RedisModuleCtx *ctx) {

    RedisModuleTypeMethods tm = {
        1,
        llca_conc_rdb_load,
        llca_conc_rdb_save,
        llca_conc_aof_rewrite,
        llca_conc_mem_usage,
        llca_conc_digest,
        llca_conc_free
    };

    llca_conc = RedisModule_CreateDataType(ctx,"llca_conc",0,&tm);
    if(llca_conc == nullptr) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "llca_conc.insert", event_queue::conc_handler<llca_conc_insert_event>, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if(RedisModule_CreateCommand(ctx, "llca_conc.contains", event_queue::conc_handler<llca_conc_contains_event>, "write", 1,1,1) == REDISMODULE_ERR){
        return REDISMODULE_ERR;
    }
    if(RedisModule_CreateCommand(ctx, "llca_conc.remove", event_queue::conc_handler<llca_conc_remove_event>, "write", 1,1,1) == REDISMODULE_ERR){
        return REDISMODULE_ERR;
    }
    if(RedisModule_CreateCommand(ctx, "llca_conc.create", event_queue::conc_handler<llca_conc_create_event>, "write", 1,1,1) == REDISMODULE_ERR){
        return REDISMODULE_ERR;
    }
    if(RedisModule_CreateCommand(ctx, "llca_conc.delete", event_queue::conc_handler<llca_conc_delete_event>, "write", 1,1,1) == REDISMODULE_ERR){
        return REDISMODULE_ERR;
    }

    return REDISMODULE_OK;
}


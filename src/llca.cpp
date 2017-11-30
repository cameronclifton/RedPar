#include "llca.h"

static RedisModuleType * llca;

struct llca_node{
  int64_t value;
  struct llca_node* next;
};

struct llca_obj{
  struct llca_node* head;
  size_t len;
};


struct llca_obj *createLLCAObject(){

  llca_obj* o; 
  o =(llca_obj*) RedisModule_Alloc(sizeof(*o));
  o->head= NULL;
  o->len = 0;
  return o;
}

void llca_insert(struct llca_obj* o, int64_t val){

   llca_node* next = o->head;
   llca_node* newnode;
   llca_node* prev = nullptr;

  while(next->value < val){
    prev = next;
    next = next->next;
  }
  newnode =(llca_node*)RedisModule_Alloc(sizeof(*newnode));
  newnode->value = val;
  newnode->next = next;
  if(prev){
    prev->next = newnode;
  }
  else{
    o->head = newnode;
  }
  o->len++;
}



void llca_release(struct llca_obj* o){
  
  struct llca_node *cur, *next;
  cur = o->head;
  while(cur) {
    next = cur->next;
    RedisModule_Free(cur);
    cur = next;
  }
    RedisModule_Free(o);
}



/* ====== "llca" commands ==============*/
 
/*llca.insert key val*/
int llca_insert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
   RedisModule_AutoMemory(ctx); /* Use automatic memory management. */

    if (argc != 3) return RedisModule_WrongArity(ctx);
    RedisModuleKey *key =(RedisModuleKey*) RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != llca)
    {
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }

    long long value;
    if ((RedisModule_StringToLongLong(argv[2],&value) != REDISMODULE_OK)) {
        return RedisModule_ReplyWithError(ctx,"ERR invalid value: must be a signed 64 bit integer");
    }

    struct llca_obj *ll;
    if(type == REDISMODULE_KEYTYPE_EMPTY){
      ll = createLLCAObject();
      RedisModule_ModuleTypeSetValue(key,llca,ll);
    }
    else{
      ll =(llca_obj*) RedisModule_ModuleTypeGetValue(key);
    }
    llca_insert(ll,value);
    RedisModule_ReplyWithLongLong(ctx,ll->len);
    RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* "llca" type methods */

void* llca_rdb_load(RedisModuleIO* rdb, int encver){
   if (encver != 0) {
        /* RedisModule_Log("warning","Can't load data with version %d", encver);*/
        return NULL;
    }
    uint64_t elements = RedisModule_LoadUnsigned(rdb);
    struct llca_obj* ll = createLLCAObject();
    while(elements--) {
        int64_t ele = RedisModule_LoadSigned(rdb);
        llca_insert(ll,ele);
    }
    return ll;
}

void llca_rdb_save(RedisModuleIO *rdb, void* value) {
  struct llca_obj *ll = (llca_obj*)value;
    struct llca_node *node = ll->head;
    RedisModule_SaveUnsigned(rdb,ll->len);
    while(node) {
        RedisModule_SaveSigned(rdb,node->value);
        node = node->next;
    }
}

void llca_free(void *value) {
  llca_release((llca_obj*)value);
}




int LLCA_OnLoad(RedisModuleCtx *ctx) {
    // Register the module itself – it’s called example and has an API version of 1
    if (Export_RedisModule_Init(ctx, "llca_type", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    //no periods before, because g++, this doesn't work todo fix this all options need to be initiliazed
    RedisModuleTypeMethods tm = {
     rdb_load  llca_rdb_load,
     rdb_save llca_rdb_save,
     free llca_free
    };
    
    llca = RedisModule_CreateDataType(ctx,"llca_type",0,&tm);
    if(llca == nullptr) return REDISMODULE_ERR;
    
    if (RedisModule_CreateCommand(ctx, "llca_type.insert",llca_insert_RedisCommand , "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }

    return REDISMODULE_OK;
}


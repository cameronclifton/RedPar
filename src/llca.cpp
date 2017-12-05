#include "llca.h"
#include "eventqueue.h"

#include <iostream>

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

  while(next && next->value < val){
    prev = next;
    next = next->next;
  }
  std::cerr<<"new node allocation\n";
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

bool llca_remove(struct llca_obj* o, int64_t val){
  if(o->head == NULL){//list is initialized but empty
    return false;
  }
  llca_node* cur = o->head;
  llca_node* prev = cur;
  if(cur->value == val){//needs to delete first value in list
    o->head = cur->next;
    RedisModule_Free(cur);
    return true;
  }
  
  while(cur->next != NULL && cur->value != val){
    prev = cur;
    cur= cur->next;
  } 
  if(cur->next == NULL){//traversed to end of list
    if(cur->value == val){//is value at end of list?
      prev->next = NULL;
      RedisModule_Free(cur);
      return true;
    }
    else{//value not in list
    return false;
    }
  }
  //must have exited while loop b/c cur->value = val
  prev->next = cur->next;// delete what lies at n
  RedisModule_Free(cur);
  return true;

}

bool llca_contains(struct llca_obj* o, int64_t val){
  llca_node* cur = o->head;
  while(cur) {
    if(cur->value == val){
      return true;
    }
    cur = cur->next;
  }
  return false;
}

//untested
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
RedisModuleString* llca_remove_RedisCommand(RedisModuleCtx* ctx, RedisModuleString **argv, int argc){
  RedisModule_AutoMemory(ctx);
  RedisModuleString * result;
  if (argc != 3){
    result =  RedisModule_CreateString(ctx, "wrong arity",11);
    return result;   
  }
  else{
    RedisModule_ThreadSafeContextLock(ctx);
    RedisModuleKey *key =  (RedisModuleKey*) RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ|REDISMODULE_WRITE);
    RedisModule_ThreadSafeContextUnlock(ctx);
    int type = RedisModule_KeyType(key); 
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != llca){
      result = RedisModule_CreateString(ctx, "wrong type",10);
      return result;
    }
    //set value
    long long value;
    if ((RedisModule_StringToLongLong(argv[2],&value) != REDISMODULE_OK)) {
      result = RedisModule_CreateString(ctx, "wrong int type",14);
      return result;
    }
    if(type == REDISMODULE_KEYTYPE_EMPTY){
      result = RedisModule_CreateString(ctx, "List does not exist",19);
      return result;
    }
    else{
      llca_obj* ll = (llca_obj*) RedisModule_ModuleTypeGetValue(key);
      bool remover = llca_remove(ll,value);
      if(remover){
	result = RedisModule_CreateString(ctx, "success",7);
      }
      else{
	result = RedisModule_CreateString(ctx, "failure",7);
      }
      return result;
 
    }
  }
}
 
/* llca_type.contains key val*/
RedisModuleString*  llca_contains_RedisCommand(RedisModuleCtx* ctx, RedisModuleString **argv, int argc){
  RedisModule_AutoMemory(ctx);
  RedisModuleString * result;
  if (argc != 3){
    result = RedisModule_CreateString(ctx, "wrong arity",11);
    return result;
 }
  RedisModule_ThreadSafeContextLock(ctx);
  RedisModuleKey *key =  (RedisModuleKey*) RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ|REDISMODULE_WRITE);
  RedisModule_ThreadSafeContextUnlock(ctx);
  int type = RedisModule_KeyType(key); 
  if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != llca)
    {
      result = RedisModule_CreateString(ctx, "wrong type",10);
      return result;
    }
  //set value
  long long value;
  if ((RedisModule_StringToLongLong(argv[2],&value) != REDISMODULE_OK)) {
    result = RedisModule_CreateString(ctx,"wrong int type",14);
    return result;
    }
  if(type == REDISMODULE_KEYTYPE_EMPTY){
    result = RedisModule_CreateString(ctx, "List does not exist",19);
    return result;
  } else{
    llca_obj* ll = (llca_obj*) RedisModule_ModuleTypeGetValue(key);
    if(llca_contains(ll,value)){
      result = RedisModule_CreateString(ctx, "value present",13);
    }
    else{
      result = RedisModule_CreateString(ctx, "value not present",17);
    }
    return result;
  }

}


/*llca.insert key val*/
RedisModuleString* llca_insert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  RedisModule_AutoMemory(ctx); /* Use automatic memory management. */
  RedisModuleString* result; 
  if (argc != 3){
   result =  RedisModule_CreateString(ctx, "wrong arity",11);
   return result;
 }
  else{
    RedisModule_ThreadSafeContextLock(ctx);
    RedisModuleKey* key =  (RedisModuleKey*)RedisModule_OpenKey(ctx,argv[1],REDISMODULE_READ|REDISMODULE_WRITE);
    RedisModule_ThreadSafeContextUnlock(ctx);
    int type = RedisModule_KeyType(key); 
    if (type != REDISMODULE_KEYTYPE_EMPTY && RedisModule_ModuleTypeGetType(key) != llca)
      {
	result = RedisModule_CreateString(ctx, "wrong type",10);
	return result;
      }
    long long value;
    if ((RedisModule_StringToLongLong(argv[2],&value) != REDISMODULE_OK)) {
      result = RedisModule_CreateString(ctx, "wrong int type",14);
      return result;
    }
    struct llca_obj *ll;
    if(type == REDISMODULE_KEYTYPE_EMPTY){//create new key if one doens't already exist
      ll = createLLCAObject();
      RedisModule_ModuleTypeSetValue(key,llca,ll);
    }
    else{
      ll =(llca_obj*) RedisModule_ModuleTypeGetValue(key);
    }
    llca_insert(ll,value);
    return RedisModule_CreateString(ctx, "success",7);
  }
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


void llca_aof_rewrite(RedisModuleIO *aof, RedisModuleString *key, void *value) {
  llca_obj *ll =(llca_obj*) value;
    llca_node *node = ll->head;
    while(node) {
        RedisModule_EmitAOF(aof,"LLCA_TYPE.INSERT","sl",key,node->value);
        node = node->next;
    }
}

/* The goal of this function is to return the amount of memory used by
 * the HelloType value. */
size_t llca_mem_usage(const void *value) {
  const llca_obj *ll = (llca_obj*)value;
    llca_node *node = ll->head;
    return sizeof(*ll) + sizeof(*node)*ll->len;
}



void llca_free(void *value) {
  llca_release((llca_obj*)value);
}

void llca_digest(RedisModuleDigest *md, void *value) {
  llca_obj *ll = (llca_obj*)value;
    llca_node *node = ll->head;
    while(node) {
        RedisModule_DigestAddLongLong(md,node->value);
        node = node->next;
    }
    RedisModule_DigestEndSequence(md);
}


struct llca_remove_event: public event{
  using event::event;
  void execute(){
    RedisModuleString * result = llca_remove_RedisCommand(ctx,argv,argc);
    RedisModule_ReplyWithString(ctx,result);
    RedisModule_UnblockClient(client,NULL);
  }

};

struct llca_contains_event: public event{
  using event::event;
  void execute(){
    RedisModuleString * result;
    result = llca_contains_RedisCommand(ctx,argv,argc);
    RedisModule_ReplyWithString(ctx,result);
    RedisModule_UnblockClient(client,NULL);
  }

};

struct llca_insert_event: public event {
    using event::event;
    void execute(){
      RedisModuleString * result = llca_insert_RedisCommand(ctx,argv,argc);
      RedisModule_ReplyWithString(ctx,result);
      RedisModule_UnblockClient(client,NULL);
      }
};
/*
struct llca_create_event: public event{
  using event::event;		       
  void execute(){
    if(llca_create_RedisCommand(ctx,argv,argc) == REDISMODULE_OK){
      RedisModuleString * result = RedisModule_CreateString(ctx, "success",7);
      RedisModule_UnblockClient(client,result);
    }
    else{
	//todo : implement error
	}
  }
  };*/


int LLCA_OnLoad(RedisModuleCtx *ctx) {

    RedisModuleTypeMethods tm = {
        1,
        llca_rdb_load,
        llca_rdb_save,
        llca_aof_rewrite,
        llca_mem_usage,
        llca_digest,
        llca_free
    };

    llca = RedisModule_CreateDataType(ctx,"llca_type",0,&tm);
    if(llca == nullptr) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "llca_type.insert", handler<llca_insert_event>, "write", 1, 1, 1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    
    if(RedisModule_CreateCommand(ctx, "llca_type.contains", handler<llca_contains_event>, "write", 1,1,1) == REDISMODULE_ERR){
      return REDISMODULE_ERR;
    }
    if(RedisModule_CreateCommand(ctx, "llca_type.remove", handler<llca_remove_event>, "write", 1,1,1) == REDISMODULE_ERR){
      return REDISMODULE_ERR;
    }

    return REDISMODULE_OK;
}


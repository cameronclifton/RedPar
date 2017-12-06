# Parallel Redis Module supporting multiple Data Types

# Getting Started: 
 * Before getting started it is important to note that this code has not achieved speedup from running sequentially. This is due to the Redis Server requiring our module to lock all thread safe contexts before searching the global keyspace for keys.
 
  - Download and Compile the repository with "make Command.
  - Add path to compiled ".so" file to redis.conf file in redis server 
  - Start redis-server excutable, you should be able to see that it loaded the module. 
  
# About This Repository 
  - redpar.cpp is the hook into our module, it exports ModuleOnLoad function and spawns all the threads when ModuleOnLoad is called. 
  - eventqueue.cpp is the queue of all events our module will handle, each thread tries to aquire a lock on it to dequeue and execute an event. It also holds a mutex. 
  - llca.cpp implements a sequential linked list but shows how to implement a data type that will have multiple threads do work on it at the same time. 
  - skiplist.cpp implements a concurrent lock free skiplist following the conventions of our module. 
  - benchmark.go uses the redbench library from https://github.com/tidwall/redbench and benchmarks our module. 
  - the "redis" directory in the src folder is key to integrating our C++ module with the redis server written in C. It holds a wrapper (redis_module_wrapper.h) around the redismodule api (found in redismodule.h) that allows us to include the wrapper in all files for compilation. It still compiles and links the redismodule.h and redismodule.c into a redismodule.o file, as the wrapper defines the function pointers and is included in all files using the API. 

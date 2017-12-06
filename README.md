# Parallel Redis Module supporting multiple Data Types
Getting Started
  - Download and Compile the repository with "make Command.
  - Add path to compiled ".so" file to redis.conf file in redis server 
  - Start redis-server excutable, you should be able to see that it loaded the module. 
  
About This Repository 
  - redpar.cpp is the hook into our module, it exports ModuleOnLoad function and spawns all the threads when ModuleOnLoad is called. 
  - eventqueue.cpp is the queue of all events our module will handle, each thread tries to aquire a lock on it to dequeue and execute an event. It also holds a mutex. 
  - llca.cpp implements a sequential linked list but shows how to implement a data type that will have multiple threads do work on it at the same time. 
  - skiplist.cpp implements a concurrent lock free skiplist following the conventions of our module. 
  - benchmark.go uses the redbench library from https://github.com/tidwall/redbench and benchmarks our module. 
  - 

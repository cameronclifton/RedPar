#include "t_int.h"

t_int::t_int(int v): value(v){}

void t_int::set(int v, int & status){
    status = REDISMODULE_ERR;
    value = v;
}

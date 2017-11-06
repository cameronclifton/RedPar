namespace t_int__rm {
    #include "redismodule.h"
}

class t_int {
    int value;
  public:
    t_int(int v);
    void set(int v, int & status);
};

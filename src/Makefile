ifndef RM_INCLUDE_DIR
	RM_INCLUDE_DIR=$(shell pwd)
endif

DEPS_DIR=$(RM_INCLUDE_DIR)/../deps

#debug is compiled with -O0
ifndef DEBUG
	DEBUG = 0
endif
DEBUGFLAGS = -g -ggdb -O2
ifeq ($(DEBUG), 1)
	DEBUGFLAGS = -g -ggdb -O0
endif

TARGET = redpar.so

SOURCEDIR=$(shell pwd -P)
CXX_SOURCES = $(wildcard $(SOURCEDIR)/*.cpp) 
OFILES = $(patsubst $(SOURCEDIR)/%.cpp, $(SOURCEDIR)/%.o, $(CXX_SOURCES))

INCLUDE_DIRS = -I"$(DEPS_DIR)/cds" 
CXXFLAGS = $(INCLUDE_DIRS) -Wall $(DEBUGFLAGS) -std=c++11
LIBS = $(DEPS_DIR)/cds/out/bin/libcds-s_d.a
SHOBJ_LDFLAGS ?= -shared
all: $(TARGET)

cds:
	cmake -B$(DEPS_DIR)/cds/out -H$(DEPS_DIR)/cds
	$(MAKE) -C $(DEPS_DIR)/cds/out
.PHONY: cds

%.o: %.cpp
	@echo [CXX] $< "-->" $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

redismodule.o: $(SOURCEDIR)/redis/redismodule.c 
	@echo [CXX] $< "-->" $@
	@gcc -c -o $(SOURCEDIR)/$@ $<

$(TARGET): $(OFILES) redismodule.o cds 
	@echo [LD] $^ "-->" $@
	@$(CXX) -o $@ redismodule.o $(OFILES) $(LIBS) $(SHOBJ_LDFLAGS) 


.PHONY: all clean

clean:
	rm -rvf *.xo *.so *.o *.a


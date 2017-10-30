# Set environment variable RM_INCLUDE_DIR to the location of redismodule.h
ifndef RM_INCLUDE_DIR
	RM_INCLUDE_DIR=$(shell pwd)
endif

#debug is compiled with -O0
ifndef DEBUG
	DEBUG = 0
endif
DEBUGFLAGS = -g -ggdb -O2
ifeq ($(DEBUG), 1)
	DEBUGFLAGS = -g -ggdb -O0
endif

# Find the OS
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
INCLUDE_DIRS = -I"$(RM_INCLUDE_DIR)" 
CFLAGS = $(INCLUDE_DIRS) -Wall $(DEBUGFLAGS) -fPIC -std=c++11  -D_GNU_SOURCE
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

ifeq ($(uname_S),Linux)
	SHOBJ_CFLAGS ?=  -fno-common -g -ggdb
	SHOBJ_LDFLAGS ?= -shared -Bsymbolic -Bsymbolic-functions
else
	CFLAGS += -mmacosx-version-min=10.6
	SHOBJ_CFLAGS ?= -dynamic -fno-common -g -ggdb
	SHOBJ_LDFLAGS ?= -dylib -exported_symbol _RedisModule_OnLoad -macosx_version_min 10.6
endif
export CFLAGS
export DEBUGFLAGS

SOURCEDIR=$(shell pwd -P)
CC_SOURCES = $(wildcard ./*.cpp)
CC_OBJECTS = $(patsubst ./%.cpp, ./%.o, $(CC_SOURCES))

all: reint.so libreint.a

.PHONY: rmutil

reint.so: $(CC_OBJECTS)
	$(LD) -o $@ $(CC_OBJECTS) $(SHOBJ_LDFLAGS) -lc -lm

libreint.a: $(CC_OBJECTS)
	ar rcs $@ $(LIBS) $(CC_OBJECTS)

clean:
	rm -rvf *.xo *.so *.o *.a


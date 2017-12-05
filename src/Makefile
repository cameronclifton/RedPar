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

TARGET = reint.so

JUNCTION_DIR = $(DEPS_DIR)/junction
JUNCTION_INSTALL_DIR = $(JUNCTION_DIR)/junction-install

SOURCEDIR=$(shell pwd -P)
CXX_SOURCES = $(wildcard $(SOURCEDIR)/*.cpp) 
OFILES = $(patsubst $(SOURCEDIR)/%.cpp, $(SOURCEDIR)/%.o, $(CXX_SOURCES))

INCLUDE_DIRS = -I"$(JUNCTION_INSTALL_DIR)/include" -I"$(DEPS_DIR)/cds" 
#CXXFLAGS = $(INCLUDE_DIRS) -Wall $(DEBUGFLAGS) -std=c++11 -stdlib=libc++
CXXFLAGS = $(INCLUDE_DIRS) -Wall $(DEBUGFLAGS) -std=c++11
LIBS = $(JUNCTION_INSTALL_DIR)/lib/libjunction.a $(DEPS_DIR)/cds/out/bin/libcds-s_d.a
#SHOBJ_LDFLAGS ?= -shared -lc++
#SHOBJ_LDFLAGS ?= -dylib -exported_symbol _RedisModule_OnLoad -macosx_version_min 10.6 -lstdc++
SHOBJ_LDFLAGS ?= -shared
all: $(TARGET)

junction:
	cmake -DCMAKE_INSTALL_PREFIX=$(JUNCTION_INSTALL_DIR) -DJUNCTION_WITH_SAMPLES=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -B$(JUNCTION_DIR)/build -H$(JUNCTION_DIR)
	$(MAKE) install -C $(DEPS_DIR)/junction/build 
.PHONY: junction

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

$(TARGET): $(OFILES) redismodule.o cds #junction
	@echo [LD] $^ "-->" $@
	@$(CXX) -o $@ redismodule.o $(OFILES) $(LIBS) $(SHOBJ_LDFLAGS) 


.PHONY: all clean

clean:
	rm -rvf *.xo *.so *.o *.a


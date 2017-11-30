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

CXX = g++
CXXFILES = t_int reint llca

TARGET = reint.so

JUNCTION_DIR = $(DEPS_DIR)/junction
JUNCTION_INSTALL_DIR = $(JUNCTION_DIR)/junction-install

OFILES = $(patsubst %, %.o, $(CXXFILES))
INCLUDE_DIRS = -I"$(JUNCTION_INSTALL_DIR)/include" 
CXXFLAGS = $(INCLUDE_DIRS) -Wall $(DEBUGFLAGS) -std=c++11
LIBS = $(JUNCTION_INSTALL_DIR)/lib/libjunction.a
SHOBJ_LDFLAGS ?= -dylib -exported_symbol _RedisModule_OnLoad -macosx_version_min 10.6

all: $(TARGET)

junction:
	cmake -DCMAKE_INSTALL_PREFIX=$(JUNCTION_INSTALL_DIR) -DJUNCTION_WITH_SAMPLES=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -B$(JUNCTION_DIR)/build -H$(JUNCTION_DIR)
	$(MAKE) install -C $(DEPS_DIR)/junction/build 
.PHONY: junction

%.o: %.cpp
	@echo [CXX] $< "-->" $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OFILES) junction
	@echo [LD] $^ "-->" $@
	$(LD) -o $@ $(OFILES) $(LIBS) $(SHOBJ_LDFLAGS) -lc -lm -lc++ -lstdc++ 


.PHONY: all clean

clean:
	rm -rvf *.xo *.so *.o *.a


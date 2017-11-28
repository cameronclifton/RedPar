ifndef RM_INCLUDE_DIR
	RM_INCLUDE_DIR=$(shell pwd)
endif

DEPS_DIR="$(RM_INCLUDE_DIR)/../deps"

#debug is compiled with -O0
ifndef DEBUG
	DEBUG = 0
endif
DEBUGFLAGS = -g -ggdb -O2
ifeq ($(DEBUG), 1)
	DEBUGFLAGS = -g -ggdb -O0
endif

CXX = g++
CXXFILES = t_int reint

TARGET = reint.so

OFILES = $(patsubst %, %.o, $(CXXFILES))
INCLUDE_DIRS = -I"$(DEPS_DIR)/junction/junction-install/include" 
CXXFLAGS = $(INCLUDE_DIRS) -Wall $(DEBUGFLAGS) -std=c++11
#LIBS = $(DEPS_DIR)/junction/libjunction.a
SHOBJ_LDFLAGS ?= -dylib -exported_symbol _RedisModule_OnLoad -macosx_version_min 10.6

all: $(TARGET)

junction:
	$(MAKE) -C $(DEPS_DIR)/junction 
.PHONY: junction

%.o: %.cpp
	@echo [CXX] $< "-->" $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OFILES) #junction
	@echo [LD] $^ "-->" $@
	$(LD) -o $@ $(OFILES) $(LIBS) $(SHOBJ_LDFLAGS) -lc -lm -lstdc++


.PHONY: all clean

clean:
	rm -rvf *.xo *.so *.o *.a


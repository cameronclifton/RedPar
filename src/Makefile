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
CXXFLAGS = -Wall $(DEBUGFLAGS) -std=c++11
SHOBJ_LDFLAGS ?=  -dylib -exported_symbol _RedisModule_OnLoad -macosx_version_min 10.6

all: $(TARGET)

%.o: %.cpp
	@echo [CXX] $< "-->" $@
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OFILES)
	@echo [LD] $^ "-->" $@
	$(LD) -o $@ $^ $(SHOBJ_LDFLAGS) -lc -lm -lstdc++

.PHONY: all clean

clean:
	rm -rvf *.xo *.so *.o *.a


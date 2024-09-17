
LIBDIR          := $(HOME)/lib

CPP             := g++

GCC_VERSION     := $(shell $(CC) -dumpversion 2>&1)
GCC_MAJOR       := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f1)
GCC_MINOR       := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f2)
GCC_MMMINOR     := $(shell $(CC) -dumpversion 2>&1 | cut -d'.' -f3)
KERN64BIT       := $(findstring x86_64,$(shell uname -r))

ifdef KERN64BIT
        XARCH := -m64
else
        XARCH := -m32
endif

CFLAGS          := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer -I.
CPPFLAGS        := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
        CPPFLAGS += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
        CPPFLAGS += -std=c++11
endif
ROOTLIBS        := `root-config --libs`
LIBS            := -L. -lDAQcommon -lpthread -lrt

libsources := daqcommon.cpp

libobjects := $(patsubst %.cpp,%.o,$(libsources))

all: libDAQcommon.so testdaqcommon
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

libDAQcommon.so: $(libobjects)
	$(CPP) $(CPPFLAGS) -shared -o $@ $^ -lc

testdaqcommon: testdaqcommon.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $^ -o $@

clean:
	rm -f libDAQcommon.so *.o testdaqcommon

headers:
	@echo "nothing to be done here"

install:
	@mkdir -p $(LIBDIR)
	/bin/cp -p libDAQcommon.so $(LIBDIR)/


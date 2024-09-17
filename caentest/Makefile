#
# DreamDaq Makefile
#
# Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

F77             := g77
CC              := gcc
CPP             := g++
LINKER          := g++

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

CFLAGS          := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer -I$(VMELIBDIR) -I$(FIFOLIBDIR)
CPPFLAGS        := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
	CPPFLAGS += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
	CPPFLAGS += -std=c++11
else ifeq ($(GCC_VERSION),4.9.1)
	CPPFLAGS += -std=c++11
endif
LIBS            := -lCAENVME

APPS            := caenreadtest caentestV513

all: $(APPS)
	@echo "************************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`        "
	@echo "        kernel is `uname -r`                                "
	@echo "************************************************************"

%: %.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)


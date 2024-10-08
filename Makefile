#
# DreamDaq Makefile
#
# Version 2014 25.11.2014 roberto.ferrari@pv.infn.it

BINDIR          := $(HOME)/bin
WRKDIR          := $(HOME)/working
VMELIBDIR       := ./VMElib
FIFOLIBDIR      := ./FIFOlib
DAQCOMMONLIBDIR      := ./DAQcommonlib
ROOTINC         := `root-config --cflags`

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

CFLAGS          := -W -Wall -ansi $(XARCH) -pipe -g -O2 -mtune=core2 -fPIC -ftree-vectorize -fomit-frame-pointer -I$(VMELIBDIR) -I$(FIFOLIBDIR) -I${DAQCOMMONLIBDIR}
CPPFLAGS        := $(CFLAGS)
ifeq ($(GCC_VERSION),4.4.7)
	CPPFLAGS += -std=gnu++0x
else ifeq ($(GCC_VERSION),4.8.2)
	CPPFLAGS += -std=c++11
else ifeq ($(GCC_VERSION),4.9.1)
	CPPFLAGS += -std=c++11
endif
ROOTLIBS        := `root-config --libs`
LIBS            := -L${DAQCOMMONLIBDIR} -lDAQcommon -L$(VMELIBDIR) -lDreamVme -L$(FIFOLIBDIR) -lDreamFIFO -lpthread -lrt -lCAENDigitizer -lCAENComm -lCAENVME

APPS            := guiStart pulserVme pulseOut testV792 testV862 testV262 testV513 readV513 initV513 resetV513 setV812 setV814 setV258 setV258thr reader writer sampler \
                  myDataWriter doOfflineHistoDrs4 myReadOut myReadOutDrs4 myFileChecker testInput enableOnOff testDAQ myReadOutCosmics myReadOutDesy myReadOutTDC myReadOutSPS2021 myReadOutNoStop fixLostTriggers myReadOutSPS2023 myReadOutNoStop2023 myReadOutSPS2024 myReadOutNoStop2024.09.02 myReadOutNoStop2024 testReadout2024 myTestReadout myReadOutNoStop2024.08.30 testV259N myReadOutNoStopV2718 testV2718 incDaqSem isInSpill stopGracely daqLogic daqReadout

CONFFILE        := drs4command.list board_corrections.dat

all: $(APPS)
	cd decode && make
	cd DaqControlInterface && qmake && make
	@echo "****************************************************"
	@echo "        all done with g++ version `g++ -dumpversion`"
	@echo "****************************************************"

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $^

%: %.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

daqReadout: daqReadout.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

daqReadout.o: daqReadout.cpp
	$(CPP) $(CPPFLAGS) -c $^

daqLogic: daqLogic.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

daqLogic.o: daqLogic.cpp
	$(CPP) $(CPPFLAGS) -c $^

stopGracely: stopGracely.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

stopGracely.o: stopGracely.cpp
	$(CPP) $(CPPFLAGS) -c $^

isInSpill: isInSpill.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

isInSpill.o: isInSpill.cpp
	$(CPP) $(CPPFLAGS) -c $^

testV2718: testV2718.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV2718.o: testV2718.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutNoStopV2718: myReadOutNoStopV2718.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutNoStopV2718.o: myReadOutNoStopV2718.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutNoStop2024.08.30: myReadOutNoStop2024.08.30.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutNoStop2024.08.30.o: myReadOutNoStop2024.08.30.cpp
	$(CPP) $(CPPFLAGS) -c $^

testV259N: testV259N.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV259N.o: testV259N.cpp
	$(CPP) $(CPPFLAGS) -c $^

myTestReadout: myTestReadout.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myTestReadout.o: myTestReadout.cpp
	$(CPP) $(CPPFLAGS) -c $^

testReadout2024: testReadout2024.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testReadout2024.o: testReadout2024.cpp
	$(CPP) $(CPPFLAGS) -c $^

testDAQ: testDAQ.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testDAQ.o: testDAQ.cpp
	$(CPP) $(CPPFLAGS) -c $^

guiStart: guiStart.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

pulserVme: pulserVme.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

pulserVme.o: pulserVme.cpp
	$(CPP) $(CPPFLAGS) -c $^

pulseOut: pulseOut.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

pulseOut.o: pulseOut.cpp
	$(CPP) $(CPPFLAGS) -c $^

testInput: testInput.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

enableOnOff: enableOnOff.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV792: testV792.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV792.o: testV792.cpp
	$(CPP) $(CPPFLAGS) -c $^

testV862: testV862.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV862.o: testV862.cpp
	$(CPP) $(CPPFLAGS) -c $^

testV262: testV262.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV262.o: testV262.cpp
	$(CPP) $(CPPFLAGS) -c $^

testV513: testV513.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

testV513.o: testV513.cpp
	$(CPP) $(CPPFLAGS) -c $^

readV513: readV513.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

initV513.o: initV513.cpp
	$(CPP) $(CPPFLAGS) -c $^

initV513: initV513.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

resetV513.o: resetV513.cpp
	$(CPP) $(CPPFLAGS) -c $^

resetV513: resetV513.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

readV513.o: readV513.cpp
	$(CPP) $(CPPFLAGS) -c $^

setV812: setV812.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

setV812.o: setV812.cpp
	$(CPP) $(CPPFLAGS) -c $^

setV814: setV814.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

setV814.o: setV814.cpp
	$(CPP) $(CPPFLAGS) -c $^

setV258: setV258.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

setV258.o: setV258.cpp
	$(CPP) $(CPPFLAGS) -c $^

setV258thr: setV258thr.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

setV258thr.o: setV258thr.cpp
	$(CPP) $(CPPFLAGS) -c $^

incDaqSem: incDaqSem.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

incDaqSem.o: incDaqSem.cpp
	$(CPP) $(CPPFLAGS) -c $^

readout: testReadout2024 myReadOutNoStop2024 myReadOutNoStop2024.09.02 myReadOutSPS2024 myReadOutNoStop2023 myReadOutSPS2023 myReadOutNoStop myReadOutSPS2021 myReadOutTDC myReadOutDesy myReadOutCosmics myReadOut myReadOutDrs4

myReadOutNoStop2024.o: myReadOutNoStop2024.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutNoStop2024: myReadOutNoStop2024.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutNoStop2024.09.02.o: myReadOutNoStop2024.09.02.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutNoStop2024.09.02: myReadOutNoStop2024.09.02.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutSPS2024.o: myReadOutSPS2024.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutSPS2024: myReadOutSPS2024.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutNoStop2023.o: myReadOutNoStop2023.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutNoStop2023: myReadOutNoStop2023.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutSPS2023.o: myReadOutSPS2023.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutSPS2023: myReadOutSPS2023.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutNoStop.o: myReadOutNoStop.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutNoStop: myReadOutNoStop.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutSPS2021.o: myReadOutSPS2021.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutSPS2021: myReadOutSPS2021.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutTDC.o: myReadOutTDC.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutTDC: myReadOutTDC.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutDesy.o: myReadOutDesy.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutDesy: myReadOutDesy.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutCosmics.o: myReadOutCosmics.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutCosmics: myReadOutCosmics.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOut.o: myReadOut.cpp
	$(CPP) $(CPPFLAGS) -c $^

myReadOutDrs4.o: myReadOut.cpp
	$(CPP) $(CPPFLAGS) -DUSE_DRS_V1742 -o $@ -c $^

myReadOut: myReadOut.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutDrs4: myReadOutDrs4.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myReadOutAll: myReadOutAll.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myFileChecker: myFileChecker.o myDecode.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

reader: reader.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

myDataWriter: myDataWriter.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

sampler: sampler.o dreammon.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTLIBS)

doOfflineHistoDrs4: doOfflineHisto.o dreammon.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS) $(ROOTLIBS)

doOfflineHisto.o: doOfflineHisto.c
	$(CPP) $(CPPFLAGS) -c $^

writer: writer.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

reader.o: reader.cpp
	$(CPP) $(CPPFLAGS) -c $^

myDataWriter.o: myDataWriter.cpp
	$(CPP) $(CPPFLAGS) -c $^

myDecode.o: myDecode.cpp
	$(CPP) $(CPPFLAGS) -c $^

myFileChecker.o: myFileChecker.cpp
	$(CPP) $(CPPFLAGS) -c $^

sampler.o: sampler.cpp
	$(CPP) $(CPPFLAGS) -c $^

writer.o: writer.cpp
	$(CPP) $(CPPFLAGS) -c $^

dreammon.o:  dreammon.c mapping.h dreammon_adc.h myhbook.h dreammon.h
	$(CPP) $(CPPFLAGS) $(ROOTINC) -c dreammon.c

fixLostTriggers: fixLostTriggers.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

install:
	@mkdir -p $(BINDIR)
	@mkdir -p $(WRKDIR)
	/bin/cp -p $(APPS) $(BINDIR)/
	/bin/cp -p $(CONFFILE) $(WRKDIR)/
	cd decode && make install
	cd DaqControlInterface && /bin/cp -p DaqControlInterface $(BINDIR)/

clean:
	@rm -f *.o $(APPS)
	@cd decode && make clean
	@cd DaqControlInterface && qmake && make clean && rm -f DaqControlInterface

#
# End of Makefile
#

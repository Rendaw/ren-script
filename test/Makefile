ifeq ($(PLATFORM),windows)
	PLATFORMFLAGS = -DWINDOWS
	EXTENSION = .exe
else
	PLATFORMFLAGS = -DLINUX
	EXTENSION = 
endif

COMPILEDEBUG = g++ \
	-pedantic -Wno-long-long -Wall -std=c++0x -ggdb -D DEBUG \
	$(CFLAGS) $(PLATFORMFLAGS) -I../../ $(LDFLAGS) \
	-o

test: testdatabuilder
	objects/testdatabuilder$(EXTENSION)
	
testdatabuilder: testdatabuilder.cxx
	$(COMPILEDEBUG) objects/testdatabuilder$(EXTENSION) $^ ../objects/*.debug.o -lren-general -llua5.2

clean:
	rm objects/*

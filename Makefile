NAME = script
OBJECTS = objects/script.o objects/databuilder.o
COMPILE_EXTRA = 
ifeq ($(PLATFORM),windows)
	LINK_EXTRA = -lren-general -llua5.1
else
	LINK_EXTRA = -lren-general -llua
endif

include ../library-tools/Makefile.include

objects/script.o objects/script.debug.o: script.cxx script.h
	$(COMPILE) $@ script.cxx

objects/databuilder.o objects/databuilder.debug.o: databuilder.cxx databuilder.h
	$(COMPILE) $@ databuilder.cxx

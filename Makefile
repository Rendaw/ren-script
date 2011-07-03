NAME = script
OBJECTS = objects/script.o objects/databuilder.o
COMPILE_EXTRA = 
LINK_EXTRA = -llua

include ../library-tools/Makefile.include

objects/script.o objects/script.debug.o: script.cxx script.h
	$(COMPILE) $@ script.cxx

objects/databuilder.o objects/databuilder.debug.o: databuilder.cxx databuilder.h
	$(COMPILE) $@ databuilder.cxx

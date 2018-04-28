#
# Created by gmakemake (Ubuntu Jul 25 2014) on Sat Apr 28 14:56:57 2018
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.cc) $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Flags from header.mak

BASE = /home/course/csci453
ARCHVER = arch2-5a
CXX = g++
CCFLAGS = -g -I$(BASE)/include/$(ARCHVER)
CXXFLAGS = $(CCFLAGS) -std=c++14
LIBFLAGS = -g -L$(BASE)/lib/$(SYS_TYPE) -l$(ARCHVER)
CCLIBFLAGS = $(LIBFLAGS)

########## End of flags from header.mak


CPP_FILES =	components.cpp connections.cpp instruction_decode.cpp run_program.cpp z88.cpp
C_FILES =	
PS_FILES =	
S_FILES =	
H_FILES =	components.h connections.h instruction_decode.h run_program.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	components.o connections.o instruction_decode.o run_program.o 

#
# Main targets
#

all:	z88 

z88:	z88.o $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o z88 z88.o $(OBJFILES) $(CCLIBFLAGS)

#
# Dependencies
#

components.o:	components.h
connections.o:	components.h connections.h
instruction_decode.o:	instruction_decode.h
run_program.o:	components.h instruction_decode.h run_program.h
z88.o:	components.h connections.h run_program.h

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -f $(OBJFILES) z88.o core

realclean:        clean
	-/bin/rm -f z88 

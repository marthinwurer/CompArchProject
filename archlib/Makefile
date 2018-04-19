#
# Definitions
#

# CCC = /opt/SUNWspro/bin/CC
CCC = g++
BASE=/home/course/vcsg720
PACKAGE=arch2-5a
LIBNAME=lib$(PACKAGE).a
LOCALLIBNAME=$(SYS_TYPE).a
TARNAME=$(PACKAGE).tar.gz
ZIPNAME=$(PACKAGE).zip
# CCFLAGS = +w -g -xsb -I.
CCFLAGS = -g -I.
CXXFLAGS = $(CCFLAGS)

CHMOD=chmod
MKDIR=mkdir
CP=cp
RM=rm
TAR=tar
ZIP=zip

CPP_FILES =	ArchLibError.C Bus.C BusALU.C COSet.C CPUObject.C \
	Clearable.C Clock.C \
	ClockedObject.C Connector.C Constant.C Counter.C Flow.C FlowSet.C \
	InFlow.C Memory.C OutFlow.C PseudoInput.C PseudoOutput.C \
	ShiftRegister.C StorageObject.C

C_FILES =	

H_FILES =	ArchLibError.h Bus.h BusALU.h COSet.h CPUObject.h \
	Clearable.h Clock.h \
	ClockedObject.h Connector.h Constant.h Counter.h Flow.h FlowSet.h \
	InFlow.h Memory.h OutFlow.h PseudoInput.h PseudoOutput.h \
	ShiftRegister.h StorageObject.h Version.h

SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES)

.precious:	$(SOURCEFILES)

OBJFILES =	ArchLibError.o Bus.o BusALU.o COSet.o CPUObject.o \
	Clearable.o Clock.o \
	ClockedObject.o Connector.o Constant.o Counter.o Flow.o \
	FlowSet.o InFlow.o Memory.o OutFlow.o PseudoInput.o \
	PseudoOutput.o ShiftRegister.o StorageObject.o

#
# Main targets
#

all:	 $(LOCALLIBNAME)

$(LOCALLIBNAME):	$(LOCALLIBNAME)($(OBJFILES))
#	$(CCC) -c $(CXXFLAGS) $(?:.o=.C)
#	ar -rv $(LOCALLIBNAME) $?
#	rm $?

install:	$(LOCALLIBNAME)
	$(RM) -rf $(BASE)/lib/$(SYS_TYPE)/$(LIBNAME)
	$(CP) $(LOCALLIBNAME) $(BASE)/lib/$(SYS_TYPE)/$(LIBNAME)
	$(CHMOD) 444 $(BASE)/lib/$(SYS_TYPE)/$(LIBNAME)

install-all:	install
	$(RM) -rf $(BASE)/include/$(PACKAGE)
	$(MKDIR) $(BASE)/include/$(PACKAGE)
	$(CHMOD) 755 $(BASE)/include/$(PACKAGE)
	$(CP) $(H_FILES) $(BASE)/include/$(PACKAGE)
	$(CHMOD) 444 $(BASE)/include/$(PACKAGE)/*

$(TARNAME):	$(SOURCEFILES) Makefile
	$(RM) -f $(TARNAME)
	$(TAR) cf - $(SOURCEFILES) Makefile | gzip > $(TARNAME)

$(ZIPNAME):	$(SOURCEFILES) Makefile
	$(RM) -f $(ZIPNAME)
	$(ZIP) $(ZIPNAME) $(SOURCEFILES) Makefile

archive:	$(TARNAME) $(ZIPNAME)
	$(RM) -f $(BASE)/pub/misc/$(TARNAME) $(BASE)/pub/misc/$(ZIPNAME)
	$(CP) $(TARNAME) $(ZIPNAME) $(BASE)/pub/misc
	$(CHMOD) 444 $(BASE)/pub/misc/$(TARNAME) $(BASE)/pub/misc/$(ZIPNAME)

#
# Dependencies
#

$(LOCALLIBNAME)(Bus.o):			Bus.h		 Bus.C
$(LOCALLIBNAME)(BusALU.o):		BusALU.h	 BusALU.C
$(LOCALLIBNAME)(COSet.o):		COSet.h		 COSet.C
$(LOCALLIBNAME)(CPUObject.o):		CPUObject.h	 CPUObject.C
$(LOCALLIBNAME)(Clearable.o):		Clearable.h	 Clearable.C
$(LOCALLIBNAME)(Clock.o):		Clock.h		 Clock.C
$(LOCALLIBNAME)(ClockedObject.o):	ClockedObject.h	 ClockedObject.C
$(LOCALLIBNAME)(Connector.o):		Connector.h	 Connector.C
$(LOCALLIBNAME)(Constant.o):		Constant.h	 Constant.C
$(LOCALLIBNAME)(Counter.o):		Counter.h	 Counter.C
$(LOCALLIBNAME)(Flow.o):		Flow.h		 Flow.C
$(LOCALLIBNAME)(FlowSet.o):		FlowSet.h	 FlowSet.C
$(LOCALLIBNAME)(InFlow.o):		InFlow.h	 InFlow.C
$(LOCALLIBNAME)(Memory.o):		Memory.h	 Memory.C
$(LOCALLIBNAME)(OutFlow.o):		OutFlow.h	 OutFlow.C
$(LOCALLIBNAME)(PseudoInput.o):		PseudoInput.h	 PseudoInput.C
$(LOCALLIBNAME)(PseudoOutput.o):	PseudoOutput.h	 PseudoOutput.C
$(LOCALLIBNAME)(ShiftRegister.o):	ShiftRegister.h	 ShiftRegister.C
$(LOCALLIBNAME)(StorageObject.o):	StorageObject.h	 StorageObject.C

#
# Housekeeping
#

clean:
	-/bin/rm -r $(OBJFILES) ptrepository SunWS_cache .sb ii_files core 2> /dev/null

realclean:	clean
	/bin/rm -rf  

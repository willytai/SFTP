# any flags defined in other packages need to be defined here as well
PKGFLAG = -DDEV -DMEMDEBUG
EXTHDRS =

include ../Makefile.in

BINDIR = ../../bin
TARGET = $(BINDIR)/$(EXEC)

target: $(TARGET)

$(TARGET): $(COBJS) $(LIBCHECKPOINT)
	@$(ECHO) "> building: $(EXEC) ..."
	@$(CXX) $(CFLAGS) $(WFLAGS) $(FFLAGS) $(DPNIFLAG) $(DPNLFLAG) $(COBJS) -o $@ $(INCLIB)

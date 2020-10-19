PKGFLAG =
EXTHDRS =

include ../Makefile.in

BINDIR = ../../bin
TARGET = $(BINDIR)/$(EXEC)

target: $(TARGET)

$(TARGET): $(COBJS)
	@$(ECHO) "> building: $(EXEC) ..."
	@$(CXX) $(CFLAGS) $(WFLAGS) $(DPNFILAG) $(DPNLFLAG) $(COBJS) -o $@ $(INCLIB)

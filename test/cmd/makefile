PKGFLAG = -DDEV

include ../Makefile.in

BINDIR   = bin
TESTEXEC = $(addprefix $(BINDIR)/, $(addsuffix .bin, $(basename $(CSRCS))))

target: $(BINDIR) $(TESTEXEC)

$(BINDIR)/%.bin: %.o $(LIBCHECKPOINT)
	@$(ECHO) "> building: $@ ..."
	@$(CXX) $(CFLAGS) $(WFLAGS) $(DPNFILAG) $(DPNLFLAG) $< -o $@ $(INCLIB)

$(BINDIR):
	@mkdir $(BINDIR)

.PHONY: clean
clean:
	@rm -f $(BINDIR)/*.bin

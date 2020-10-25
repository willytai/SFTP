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

run:
	@for exec in $(TESTEXEC); \
	do \
		tput setaf 6; \
		$(ECHO) "----------- [Test] test/$(PKGNAME)/$$exec -----------"; \
		tput setaf 2; \
		./$$exec -d yes; \
	done

.PHONY: clean
clean:
	@rm -f $(BINDIR)/*.bin
	@rm -f *.o

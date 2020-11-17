ECHO      = /bin/echo
EXEC      = mySFTP
SRCLIBDIR = lib#                                       -- lib dir for source code
DIRS      = bin include $(SRCLIBDIR)

PKGS    = cmd util sftp#                               -- packages from source code
SRCLIBS = $(addsuffix .a, $(addprefix lib, $(PKGS)))#  -- the actual libs from source code
EXTPKG  = ssh#                                         -- external libs required for source code
ALLPKGS = $(PKGS) $(EXTPKG)#                           -- all libs for source code
INCLIB  = $(addprefix -l, $(ALLPKGS))#                 -- -l flags for complier

ASAN   ?= false

all: dir main

.PHONY: dir libs main

dir:
	@for dir in $(DIRS); \
	do \
		if [ ! -d $$dir ]; then \
			mkdir $$dir; \
			$(ECHO) "Creating directory \"$$dir\" ..."; \
		fi; \
	done


main: libs
	@$(ECHO) "Checking main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory INCLIB="$(INCLIB)" EXEC=$(EXEC) ASAN=$(ASAN)
	@ln -fs bin/$(EXEC) .


libs:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Checking $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg ASAN=$(ASAN); \
	done

.PHONY: cppcheck
cppcheck:
	@cppcheck . --enable=all --std=c++14 2> check.txt
	@$(ECHO) "\033[33mcppcheck finished, see results in ./check.txt\033[30m"

.PHONY: test
test:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Compiling test binary for $$pkg ..."; \
		$(MAKE) -C test/$$pkg -f makefile --no-print-directory INCLIB="$(INCLIB)"; \
	done

testclean:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Cleaning test: $$pkg ..."; \
		$(MAKE) -C test/$$pkg -f makefile --no-print-directory clean; \
	done

.PHONY: runtest
runtest:
	@for pkg in $(PKGS); \
	do \
		$(MAKE) -C test/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg run; \
	done

cleanall:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Cleaning $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg cleanall; \
	done
	@$(ECHO) "Cleaning main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory cleanall;
	@for slib in $(SRCLIBS); \
	do \
		$(ECHO) "Removing $$slib ..."; \
		rm -rf $(SRCLIBDIR)/$$slib; \
	done
	@$(ECHO) "Removing $(EXEC) ..."
	@rm -f $(EXEC) bin/$(EXEC)

clean:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Cleaning $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg clean; \
	done
	@$(ECHO) "Cleaning main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory clean;
	@for slib in $(SRCLIBS); \
	do \
		$(ECHO) "Removing $$slib ..."; \
		rm -rf $(SRCLIBDIR)/$$slib; \
	done
	@$(ECHO) "Removing $(EXEC) ..."
	@rm -f $(EXEC) bin/$(EXEC)

.PHONY: tags
tags: ctags
ctags:
	@rm -f src/tags
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Tagging $$pkg ... "; \
		cd src; \
		ctags -a $$pkg/*.cpp $$pkg/*.h; \
		cd ..; \
	done
	@$(ECHO) "Tagging main ... "
	@cd src; ctags -a main/*.cpp main/*.h;

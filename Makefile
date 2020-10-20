STATIC ?= false

ECHO      = /bin/echo
EXEC      = mySFTP
SRCLIBDIR = lib#                                       -- lib dir for source code
DIRS      = bin include $(SRCLIBDIR)

PKGS    = cmd util sftp#                               -- packages from source code
SRCLIBS = $(addsuffix .a, $(addprefix lib, $(PKGS)))#  -- the actual libs from source code
EXTPKG  = ssh#                                         -- external libs required for source code
ALLPKGS = $(PKGS) $(EXTPKG)#                           -- all libs for source code
INCLIB  = $(addprefix -l, $(ALLPKGS))#                 -- -l flags for complier

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
	@$(MAKE) -C src/main -f makefile --no-print-directory INCLIB="$(INCLIB)" EXEC=$(EXEC) STATIC=$(STATIC)
	@ln -fs bin/$(EXEC) .


libs:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Checking $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg; \
	done


clean:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Cleaning $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg clean; \
	done
	@$(ECHO) "Cleaning main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory EXEC=$(EXEC) STATIC=$(STATIC) clean;
	@for slib in $(SRCLIBS); \
	do \
		$(ECHO) "Removing $$slib ..."; \
		rm -rf $(SRCLIBDIR)/$$slib; \
	done
	@$(ECHO) "Removing $(EXEC) ..."
	@rm -f $(EXEC) bin/$(EXEC)

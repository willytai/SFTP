STATIC ?= false

ECHO   = /bin/echo
EXEC   = mySFTP
LIBDIR = lib
DIRS   = bin include $(LIBDIR)

PKGS    = cmd util sftp
INCLIB  = $(addprefix -l, $(PKGS))
SRCLIBS = $(addsuffix .a, $(addprefix lib, $(PKGS)))

all: dir libs main

.PHONY: dir libs main

dir:
	@for dir in $(DIRS); \
	do \
		if [ ! -d $$dir ]; then \
			mkdir $$dir; \
			$(ECHO) "Creating directory \"$$dir\" ..."; \
		fi; \
	done

libs:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Checking $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg; \
	done


main:
	@$(ECHO) "Checking main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory INCLIB="$(INCLIB)" EXEC=$(EXEC) STATIC=$(STATIC)
	@ln -fs bin/$(EXEC) .

clean:
	@for pkg in $(PKGS); \
	do \
		$(ECHO) "Cleaning $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg clean; \
	done
	@$(ECHO) "Cleaning main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory EXEC=$(EXEC) STATIC=$(STATIC) clean;
	@$(ECHO) "Removing $(SRCLIBS) ..."
	@rm -rf $(LIBDIR)/$(SRCLIBS)
	@$(ECHO) "Removing $(EXEC) ..."
	@rm -f $(EXEC) bin/$(EXEC)

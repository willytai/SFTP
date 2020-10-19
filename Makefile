STATIC ?= false

ECHO   = /bin/echo
EXEC   = mySFTP
DIRS   = bin include lib

LIBS    = cmd util sftp
SRCLIBS = $(addsuffix .a, $(addprefix lib, $(LIBS)))

all: dir libs main

dir:
	@for dir in $(DIRS); \
	do \
		if [ ! -d $$dir ]; then \
			mkdir $$dir; \
			echo "Creating directory \"$$dir\" ..."; \
		fi; \
	done

libs:
	@for pkg in $(LIBS); \
	do \
		echo "Checking $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg; \
	done


main:
	@echo "Checking main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory EXEC=$(EXEC) STATIC=$(STATIC)
	@ln -fs bin/$(EXEC) .

clean:
	@for pkg in $(LIBS); \
	do \
		echo "Cleaning $$pkg ..."; \
		$(MAKE) -C src/$$pkg -f makefile --no-print-directory PKGNAME=$$pkg clean; \
	done
	@echo "Cleaning main ..."
	@$(MAKE) -C src/main -f makefile --no-print-directory EXEC=$(EXEC) STATIC=$(STATIC) clean;
	@echo "Removing $(SRCLIBS) ..."
	@cd lib; rm -rf $(SRCLIBS)
	@echo "Removing $(EXEC) ..."
	@rm -f $(EXEC) bin/$(EXEC)

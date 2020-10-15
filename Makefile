STATIC ?= false

ECHO   = /bin/echo
EXEC   = mySFTP
DIRS   = bin

all: directory

directory:
	@for dir in $(DIRS); \
	do \
		if [ ! -d $$dir ]; then \
			mkdir $$dir; \
			echo "Creating directory \"$$dir\" ..."; \
		fi; \
	done
	@$(MAKE) -C src -f Makefile.src --no-print-directory EXEC=$(EXEC) STATIC=$(STATIC);
	@ln -fs bin/$(EXEC)

test:
	@$(MAKE) -C src -f Makefile.src test --no-print-directory EXEC=test STATIC=false

clean:
	@$(MAKE) -C src -f Makefile.src --no-print-directory clean;
	@rm -rf $(EXEC)

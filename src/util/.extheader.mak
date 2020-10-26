utilLNK: ../../include/def.h ../../include/dirPrinter.h ../../include/util.h 
../../include/def.h: def.h
	@cd ../../include; ln -fs ../src/util/def.h ./
../../include/dirPrinter.h: dirPrinter.h
	@cd ../../include; ln -fs ../src/util/dirPrinter.h ./
../../include/util.h: util.h
	@cd ../../include; ln -fs ../src/util/util.h ./

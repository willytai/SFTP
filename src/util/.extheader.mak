utilLNK: ../../include/util.h ../../include/dirPrinter.h ../../include/def.h ../../include/dirIO.h 
../../include/util.h: util.h
	@cd ../../include; ln -fs ../src/util/util.h ./
../../include/dirPrinter.h: dirPrinter.h
	@cd ../../include; ln -fs ../src/util/dirPrinter.h ./
../../include/def.h: def.h
	@cd ../../include; ln -fs ../src/util/def.h ./
../../include/dirIO.h: dirIO.h
	@cd ../../include; ln -fs ../src/util/dirIO.h ./

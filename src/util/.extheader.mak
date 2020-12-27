utilLNK: ../../include/Usage.h ../../include/def.h ../../include/dirIO.h ../../include/dirPrinter.h ../../include/memUtil.h ../../include/util.h 
../../include/Usage.h: Usage.h
	@cd ../../include; ln -fs ../src/util/Usage.h ./
../../include/def.h: def.h
	@cd ../../include; ln -fs ../src/util/def.h ./
../../include/dirIO.h: dirIO.h
	@cd ../../include; ln -fs ../src/util/dirIO.h ./
../../include/dirPrinter.h: dirPrinter.h
	@cd ../../include; ln -fs ../src/util/dirPrinter.h ./
../../include/memUtil.h: memUtil.h
	@cd ../../include; ln -fs ../src/util/memUtil.h ./
../../include/util.h: util.h
	@cd ../../include; ln -fs ../src/util/util.h ./

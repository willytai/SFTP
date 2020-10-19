utilLNK: ../../include/def.h ../../include/util.h 
../../include/def.h: def.h
	@cd ../../include; ln -fs ../src/util/def.h ./
../../include/util.h: util.h
	@cd ../../include; ln -fs ../src/util/util.h ./

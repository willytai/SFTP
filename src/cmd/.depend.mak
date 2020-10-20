cmdCharDef.o: cmdCharDef.cpp cmdCharDef.h ../../include/def.h
cmdClass.o: cmdClass.cpp cmdClass.h cmdParser.h ../../include/def.h
cmdControl.o: cmdControl.cpp cmdClass.h cmdParser.h ../../include/def.h
cmdHelper.o: cmdHelper.cpp cmdHelper.h ../../include/util.h \
 ../../include/def.h ../../include/def.h cmdParser.h
cmdLocal.o: cmdLocal.cpp cmdClass.h cmdParser.h ../../include/def.h \
 cmdHelper.h ../../include/util.h ../../include/def.h
cmdParser.o: cmdParser.cpp cmdParser.h ../../include/def.h cmdCharDef.h \
 ../../include/util.h ../../include/def.h
cmdServer.o: cmdServer.cpp cmdClass.h cmdParser.h ../../include/def.h
cmdSftp.o: cmdSftp.cpp cmdClass.h cmdParser.h ../../include/def.h
errorMgr.o: errorMgr.cpp cmdParser.h ../../include/def.h

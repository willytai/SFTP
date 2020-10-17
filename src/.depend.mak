cmdCharDef.o: cmdCharDef.cpp cmdCharDef.h def.h
cmdClass.o: cmdClass.cpp cmdClass.h cmdParser.h cmdCharDef.h def.h
cmdControl.o: cmdControl.cpp cmdClass.h cmdParser.h cmdCharDef.h def.h
cmdLocal.o: cmdLocal.cpp cmdClass.h cmdParser.h cmdCharDef.h def.h util.h
cmdParser.o: cmdParser.cpp cmdParser.h cmdCharDef.h def.h util.h
cmdServer.o: cmdServer.cpp cmdClass.h cmdParser.h cmdCharDef.h def.h
cmdSftp.o: cmdSftp.cpp cmdClass.h cmdParser.h cmdCharDef.h def.h
errorMgr.o: errorMgr.cpp cmdParser.h cmdCharDef.h def.h
main.o: main.cpp sftpSession.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh_version.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/legacy.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/sftp.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh.h def.h cmdParser.h \
 cmdCharDef.h
sftpSession.o: sftpSession.cpp sftpSession.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh_version.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/legacy.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/sftp.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh.h def.h
util.o: util.cpp util.h def.h

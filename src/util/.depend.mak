dirIO.o: dirIO.cpp dirIO.h def.h
dirPrinter.o: dirPrinter.cpp dirPrinter.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/sftp.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/libssh_version.h \
 /usr/local/Cellar/libssh/0.9.5/include/libssh/legacy.h util.h def.h \
 dirIO.h
util.o: util.cpp util.h def.h

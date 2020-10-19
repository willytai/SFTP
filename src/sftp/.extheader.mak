sftpLNK: ../../include/sftpSession.h 
../../include/sftpSession.h: sftpSession.h
	@cd ../../include; ln -fs ../src/sftp/sftpSession.h ./

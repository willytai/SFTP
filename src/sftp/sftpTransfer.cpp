#include "sftpSession.h"

namespace sftp
{

// read file to memory in chunks and transmit with sftp_read/sftp_write

sftpStat sftpSession::get(const std::vector<std::string>& targets) const {
    return SFTP_OK;
}

sftpStat sftpSession::get_recursive(const std::vector<std::string>& targets) const {
    return SFTP_OK;
}

}

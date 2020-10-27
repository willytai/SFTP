#ifndef __CMD_CLASS_H__
#define __CMD_CLASS_H__

#include "cmdParser.h"
#include "util.h"

typedef std::vector<std::string> vecstr;

/*************/
/* for local */
/*************/
cmdClass(llsCmd);

// the cd class is slightly different from the others
class lcdCmd : public cmdExec
{
public:
    lcdCmd() { UTIL::getHomeDir(&_home); }
    ~lcdCmd() {}

    cmdStat execute(const std::string&) const;
    void    usage()   const;
    void    help()    const;

private:
    const char* _home;
};


/**************/
/* for server */
/**************/
cmdClass(cdCmd);
cmdClass(lsCmd);
cmdClass(mkdirCmd);
cmdClass(rmCmd);
cmdClass(chmodCmd);


/***********************/
/* for program control */
/***********************/
cmdClass(quitCmd);


/************/
/* for sftp */
/************/
cmdClass(putCmd);
cmdClass(getCmd);


#endif /* __CMD_CLASS_H__ */

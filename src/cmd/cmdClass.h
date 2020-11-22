#ifndef __CMD_CLASS_H__
#define __CMD_CLASS_H__

#include "cmdParser.h"
#include "util.h"


// TODO SUPPORT string options instead of char options only
//      i.e. make --recursive works

typedef std::vector<std::string> vecstr;

/*************/
/* for local */
/*************/
cmdClassLocal(llsCmd);

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
cmdClassServer(cdCmd);
cmdClassServer(lsCmd);
cmdClassServer(mkdirCmd);
cmdClassServer(rmCmd);
cmdClassServer(chmodCmd);
cmdClassServer(putCmd);
cmdClassServer(getCmd);


/***********************/
/* for program control */
/***********************/
cmdClassLocal(quitCmd);
cmdClassLocal(exitCmd);
cmdClassLocal(usageCmd);
cmdClassLocal(clearCmd);
cmdClassLocal(helpCmd);
cmdClassLocal(remoteCmd);
cmdClassLocal(localCmd);


#endif /* __CMD_CLASS_H__ */

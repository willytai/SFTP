#ifndef __CMD_CLASS_H__
#define __CMD_CLASS_H__

#include "cmdParser.h"
#include "cmdHelper.h"

/*************/
/* for local */
/*************/
cmdClass(llsCmd);
cmdClass(lcdCmd);


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

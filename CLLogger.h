#ifndef __CLLOGGER_H__
#define __CLLOGGER_H__
#include "CLStatus.h"

#define LOG_FILE_NAME   "CLLogger.txt"
#define MAX_SIZE        256

class CLLogger {
       public:
               static CLLogger* GetInstance();
               CLStatus WriteLogMsg(const char* pstrMsg , long lErrorCode);
               CLStatus WriteLog(const char *pstrMsg , long lErrorCode);
               CLLogger(CLLogger&) = delete;
               CLLogger& operator= (CLLogger&) = delete;
       private:
               CLLogger();
       private:
               static CLLogger *m_pLog;
               int m_fd;
};
#endif

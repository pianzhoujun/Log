#ifndef __CLLOGGER_H__
#define __CLLOGGER_H__
#include "CLStatus.h"

#define LOG_FILE_NAME   "CLLogger.txt"
#define MAX_SIZE        256
#define BUFFER_SIZE_LOG_FILE    4096

class CLLogger {
       public:
               static CLLogger* GetInstance();
               CLStatus WriteLogMsg(const char* pstrMsg , long lErrorCode);
               CLStatus WriteLog(const char *pstrMsg , long lErrorCode);
               CLStatus Flush();
               CLLogger(CLLogger&) = delete;
               CLLogger& operator= (CLLogger&) = delete;

       private:
               CLLogger();
               ~CLLogger();
               static void OnProcessExit();

       private:
               static CLLogger *m_pLog;
               int m_fd;
               char *m_pLogBuffer;
               unsigned int m_nUsedBytesForBuffer;
               bool m_bFlagForProcessExit;               
};
#endif

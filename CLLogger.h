#ifndef __CLLOGGER_H__
#define __CLLOGGER_H__
#include "CLStatus.h"
#include <pthread.h>

#define LOG_FILE_NAME   "CLLogger.txt"
#define MAX_SIZE        256
#define BUFFER_SIZE_LOG_FILE    4096

class CLLogger {
       public:
               static CLLogger* GetInstance();
               static CLStatus WriteLogMsg(const char* pstrMsg , long lErrorCode);
               CLStatus WriteLog(const char *pstrMsg , long lErrorCode);
               CLStatus Flush();
               CLLogger(CLLogger&) = delete;
               CLLogger& operator= (CLLogger&) = delete;

       private:
               CLLogger();
               ~CLLogger();
               static void OnProcessExit();

       private:
               int m_fd;
               char *m_pLogBuffer;
               unsigned int m_nUsedBytesForBuffer;
               bool m_bFlagForProcessExit;               
	       CLStatus WriteMsgAndErrcodeToFile (const char* pstrMsg , const char* pstrErrcode);
//@m_log : used as singlton model
	       static pthread_mutex_t* InitializeMutex();
               static CLLogger *m_pLog;
	       static pthread_mutex_t *m_pMutexForWritingLog;
	       static pthread_mutex_t *m_pMutexForCreatingLogger;
};
#endif

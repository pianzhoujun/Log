#include "CLLogger.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>

CLLogger* CLLogger::m_pLog = NULL;

CLLogger::CLLogger()
{
        m_fd = open (LOG_FILE_NAME , O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
        m_nUsedBytesForBuffer = 0;
        m_pLogBuffer = NULL;
        m_bFlagForProcessExit = false;
}


CLLogger* CLLogger::GetInstance()
{
        if (!m_pLog) {
                m_pLog = new CLLogger(); 
                if (atexit (CLLogger::OnProcessExit) != 0) {
                        if (m_pLog != NULL) {
                                m_pLog->WriteLog("In CLLogger::GetInstance() , atexit error" , errno);
                                m_pLog->Flush();
                        }
                }
        }
        return m_pLog;
}

CLStatus CLLogger::WriteLogMsg(const char* pstrMsg , long lErrorCode)
{
        CLLogger* pLog = CLLogger::GetInstance();
        if (pLog == NULL)
                return CLStatus(-1 , 0);
        CLStatus s = pLog->WriteLog(pstrMsg , lErrorCode); 
        if (s.IsSuccess())
                return CLStatus(0 , 0);
        else
                return CLStatus(-1 , 0);
}

CLStatus CLLogger::WriteLog (const char* pstrMsg , long lErrorCode)
{
        if (pstrMsg == NULL || strlen(pstrMsg) == 0 || m_fd == -1)
                return CLStatus(-1 , 0);
        unsigned int nleftroom = BUFFER_SIZE_LOG_FILE - m_nUsedBytesForBuffer;
        unsigned int len_strmsg = strlen(pstrMsg);
        char buf[MAX_SIZE];
        snprintf(buf , MAX_SIZE , "Error Code:%ld\n",lErrorCode);
        unsigned int len_code = strlen(buf);
        int total_len = len_strmsg + len_code;
        if (total_len > BUFFER_SIZE_LOG_FILE || m_bFlagForProcessExit) {
                Flush();
                if (m_fd == -1)
                        return CLStatus (-1 , 0);
                size_t r = write (m_fd , pstrMsg , len_strmsg);
                if (r == -1)
                        return CLStatus (-1 , 0);
                r = write (m_fd , buf , len_code);
                if (r == -1)
                        return CLStatus (-1 , 0);
                return CLStatus (0 , 0);
        }

        if (total_len > nleftroom) {
                CLStatus s = Flush();
                if (!s.IsSuccess()) {
                        return CLStatus(-1 , 0);
                }
        }

        memcpy (m_pLogBuffer + m_nUsedBytesForBuffer , pstrMsg , len_strmsg);
        m_nUsedBytesForBuffer += len_strmsg;
        memcpy (m_pLogBuffer + m_nUsedBytesForBuffer , buf , len_code);
        m_nUsedBytesForBuffer += len_code;
        return CLStatus (0 , 0);
#if 0
        size_t r = write (m_fd , pstrMsg , sizeof(pstrMsg));
        if (r == -1)
                return CLStatus(-1 ,errno);
        char buf[MAX_SIZE];
        snprintf(buf , MAX_SIZE , "Error Code:%ld\n" , lErrorCode);
        r = write(m_fd , buf , strlen(buf));
        if (r == -1) 
                return CLStatus(-1 , errno);
        else
                return CLStatus(0 , 0);
#endif
}

CLStatus CLLogger::Flush()
{
        if (m_fd == -1 || m_pLogBuffer == NULL)
                return CLStatus (-1 , 0);

        if (m_nUsedBytesForBuffer == 0)
                return CLStatus (0 , 0);

        size_t r = write (m_fd , m_pLogBuffer , m_nUsedBytesForBuffer);
        if (r == -1)
                return CLStatus (-1 , 0);
        else
                return CLStatus (0 , 0);

}

void CLLogger::OnProcessExit()
{
        CLLogger *pLog = CLLogger::GetInstance();
        if (!pLog) {
                pLog->Flush();
                pLog->m_bFlagForProcessExit = true;
        }
}



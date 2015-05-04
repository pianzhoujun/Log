#include "CLLogger.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

CLLogger* CLLogger::m_pLog = NULL;

CLLogger::CLLogger()
{
        m_fd = open (LOG_FILE_NAME , O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
}


CLLogger* CLLogger::GetInstance()
{
        if (!m_pLog) {
                m_pLog = new CLLogger(); 
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
}

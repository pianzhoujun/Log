#include "CLLogger.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>

CLLogger* CLLogger::m_pLog = NULL;
pthread_mutex_t* CLLogger::m_pMutexForCreatingLogger = CLLogger::InitializeMutex();
pthread_mutex_t* CLLogger::m_pMutexForWritingLog = NULL;

CLLogger::CLLogger()
{
        m_fd = open (LOG_FILE_NAME , O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
        m_nUsedBytesForBuffer = 0;
        m_pLogBuffer = new char[BUFFER_SIZE_LOG_FILE];
        m_bFlagForProcessExit = false;
	
	m_pMutexForWritingLog = new pthread_mutex_t;
	if (pthread_mutex_init (m_pMutexForWritingLog , NULL) != 0) {
		delete m_pMutexForWritingLog;
		delete[] m_pLogBuffer;
		close(m_fd);
		
		throw "In CLLogger::CLLogger() ,  error";
	}
}

CLLogger* CLLogger::GetInstance()
{
	if (m_pLog)
		return m_pLog;

	if (pthread_mutex_lock(m_pMutexForCreatingLogger) != 0)
		return 0;

	if (!m_pLog) {
		try {
			m_pLog = new CLLogger(); 
		}
		catch (const char*){
			pthread_mutex_unlock(m_pMutexForCreatingLogger);
			return 0;
		}
		if (atexit (CLLogger::OnProcessExit) != 0) {
			m_pLog->m_bFlagForProcessExit = true;
			if (pthread_mutex_unlock(m_pMutexForCreatingLogger) != 0)
				return 0;

			m_pLog->WriteLog("In CLLogger::GetInstance() , atexit error" , errno);
		}
		return m_pLog;
	}
	pthread_mutex_unlock(m_pMutexForCreatingLogger);
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

	unsigned int len_strmsg = strlen(pstrMsg);
	char buf[MAX_SIZE];
	snprintf(buf , MAX_SIZE , "Error Code:%ld\n",lErrorCode);
	unsigned int len_code = strlen(buf);
	int total_len = len_strmsg + len_code;

	try {
		if (total_len > BUFFER_SIZE_LOG_FILE || m_bFlagForProcessExit) {
			throw WriteMsgAndErrcodeToFile(pstrMsg , buf);	
		}
		unsigned int nleftroom = BUFFER_SIZE_LOG_FILE - m_nUsedBytesForBuffer;
		if (total_len > nleftroom) {
			if (write(m_fd , m_pLogBuffer , m_nUsedBytesForBuffer) == -1)
				throw CLStatus(-1 ,errno);
			m_nUsedBytesForBuffer = 0;
			throw CLStatus(0 , 0);
		}
		memcpy (m_pLogBuffer + m_nUsedBytesForBuffer , pstrMsg , len_strmsg);
		m_nUsedBytesForBuffer += len_strmsg;
		memcpy (m_pLogBuffer + m_nUsedBytesForBuffer , buf , len_code);
		m_nUsedBytesForBuffer += len_code;
		throw  CLStatus (0 , 0);
	}
	catch (CLStatus &s) {
		if (pthread_mutex_unlock (m_pMutexForWritingLog) != 0) 
			return CLStatus(-1 , 0);
		return s;
	}

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
	if (pthread_mutex_lock(m_pMutexForWritingLog) != 0)
		return CLStatus(-1 , 0);
	try {
		if ( m_nUsedBytesForBuffer == 0)
			throw  CLStatus (0 , 0);

		if ( write (m_fd , m_pLogBuffer , m_nUsedBytesForBuffer) == -1)
			throw CLStatus (-1 ,errno);
		m_nUsedBytesForBuffer = 0;
		throw CLStatus(0,0);
	}
	catch (CLStatus &s){
		if (pthread_mutex_unlock(m_pMutexForWritingLog) != 0) 
			return CLStatus (-1 , 0);
		return s;
	}

}

void CLLogger::OnProcessExit()
{
        CLLogger *pLog = CLLogger::GetInstance();
        if (!pLog) {
                pLog->Flush();
                pLog->m_bFlagForProcessExit = true;
        }
}

CLStatus CLLogger::WriteMsgAndErrcodeToFile(const char* pstrMsg , const char* pstrErrcode)
{
	if (write (m_fd , pstrMsg , strlen(pstrMsg)) == -1)
		return CLStatus (-1 , errno);
	if (write (m_fd , pstrErrcode , strlen(pstrErrcode))== -1)
		return CLStatus (-1 , errno);
	return CLStatus (0 , 0);
			
}

pthread_mutex_t* CLLogger::InitializeMutex()
{
	pthread_mutex_t *p = new pthread_mutex_t;
	if (pthread_mutex_init (p , 0) != 0){
		delete p;
		return 0;
	}
	return p;	
}

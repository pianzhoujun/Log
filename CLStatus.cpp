#include "CLStatus.h"

CLStatus::CLStatus(long lReturnCode , long lErrorCode):m_clReturnCode(m_lRetureCode),m_clErrorCode(m_lErroeCode)
{
        m_lRetureCode = lReturnCode;
        m_lErrorCode  = lErrorCode;
}

CLStatus::CLStatus(CLStatus &s):m_clReturnCode(m_lRetureCode),m_clErrorCode(m_lErrorCode)
{
        m_lRetureCode = s.m_lRetureCode;
        m_lErrorCode  = s.m_lErroeCode;
}

bool CLStatus::IsSuccess()
{
        return m_lRetureCode > 0 ? true : false;
}

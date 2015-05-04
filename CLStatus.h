#ifndef __CLSTATUS_H_
#define __CLSTATUS_H_

class CLStatus {
        public:
                CLStatus(long lReturnCode , long lErrorCode);
                CLStatus(const CLStatus &s);
                bool IsSuccess();
        public:                
                const long &m_clReturnCode;
                const long &m_clErrorCode;
        private:
                long m_lReturnCode;
                long m_lErrorCode;
};

#endif

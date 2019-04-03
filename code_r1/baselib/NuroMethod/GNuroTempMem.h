#ifndef __NURO_TEMP_MEMORY_20100607
#define __NURO_TEMP_MEMORY_20100607

class CGNuroTempMem
{
public:
    CGNuroTempMem()
    {
        m_pBuff = NULL;
    }
    virtual ~CGNuroTempMem()
    {
        Free();
    }

    nuCHAR* Malloc(nuSIZE nSize)
    {
        if( NULL == m_pBuff )
        {
            m_pBuff = new nuCHAR[nSize];
			nuMemset(m_pBuff, 0, sizeof(nuCHAR) * nSize);
        }
        return m_pBuff;
    }
    nuVOID Free()
    {
        if( NULL != m_pBuff )
        {
            delete[] m_pBuff;
            m_pBuff = NULL;
        }
    }

public:
    nuCHAR*     m_pBuff;
};

#endif
#if !defined(__NURO_FILE_20100524)
#define __NURO_FILE_20100524


class CGNuroFile
{
public:
    CGNuroFile()
    {
        m_pFile = NULL;
    }
    virtual ~CGNuroFile()
    {
        Close();
    }

    nuBOOL Open(const nuTCHAR* ptsName, nuINT nMode)
    {
        Close();
        m_pFile = nuTfopen(ptsName, nMode);
        if( m_pFile )
        {
            return nuTRUE;
        }
        else
        {
            return nuFALSE;
        }
    }
    nuVOID Close()
    {
        if( m_pFile )
        {
            nuFclose(m_pFile);
            m_pFile = NULL;
        }
    }
    nuINT Seek(nuLONG nDis, nuINT nType)
    {
        return nuFseek(m_pFile, nDis, nType);
    }
    nuSIZE Read(nuVOID* pBuff, nuSIZE size, nuSIZE count)
    {
        return nuFread(pBuff, size, count, m_pFile);
    }
    nuSIZE Write(const nuVOID* pBuff, nuSIZE size, nuSIZE count)
    {
        return nuFwrite(pBuff, size, count, m_pFile);
    }
    nuSIZE Fgetlen()
    {
        return nuFgetlen(m_pFile);
    }
public:
    nuFILE* m_pFile;
    
};

#endif
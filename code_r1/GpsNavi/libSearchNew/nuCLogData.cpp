// nuCLogData.cpp: implementation of the nuCLogData class.
//
//////////////////////////////////////////////////////////////////////
#include "nuCLogData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

nuCLogData::nuCLogData()
{
	nuMemset(&m_header, 0, sizeof(m_header));
}

nuCLogData::~nuCLogData()
{
	Close();
}

nuBOOL nuCLogData::Create(nuTCHAR *pFileName, nuUINT nMax, nuUINT nEachRecordLen, int mode)
{
	if (!pFileName 
		|| !nMax
		|| !nEachRecordLen
		|| nMax>_LOGDATAFILE_DATA_MAXNUM 
		|| nEachRecordLen>65535)
	{
		return nuFALSE;
	}
	nuCFile file;
	if (!file.Open(pFileName, NURO_FS_OB2))
	{
		return nuFALSE;
	}
	LOGDATAFILEHEADER header = {0};
	nuWORD table[_LOGDATAFILE_DATA_MAXNUM] = {0};
	header.now	= 0;
	header.mode	= mode;
	header.max	= nMax;
	header.size = (nEachRecordLen>>1)<<1;
	for (nuWORD i=0; i<(nuWORD)_LOGDATAFILE_DATA_MAXNUM; i++)
	{
		table[i] = i;
	}
	if (file.Write(&header, sizeof(header), 1)!=1)
	{
		return nuFALSE;
	}
	if (file.Write(table, sizeof(table), 1)!=1)
	{
		return nuFALSE;
	}
	file.Seek(
		sizeof(header)+sizeof(table)+header.size*header.max, NURO_SEEK_SET);
	if (file.Write(&header.now, 2, 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL nuCLogData::IsOpen()
{
	return m_file.IsOpen();
}
nuBOOL nuCLogData::Open(nuTCHAR *pFileName)
{
	Close();
	if (!m_file.Open(pFileName, NURO_FS_OB)) 
	{
		return nuFALSE;
	}
	// read file info, check data
	if (m_file.Read(&m_header, sizeof(m_header)+sizeof(m_table), 1)!=1
		|| !m_header.max
		|| m_header.max>_LOGDATAFILE_DATA_MAXNUM)
	{
		Close();
		return nuFALSE;
	}
	return nuTRUE;
}

nuVOID nuCLogData::Close()
{
	if (m_file.IsOpen())
	{
		m_file.Close();
	}
}
nuBOOL nuCLogData::GetHeader(LOGDATAFILEHEADER *fileHeader)
{
	if (!IsOpen()
		|| !fileHeader)
	{
		return nuFALSE;
	}
	nuMemcpy(fileHeader, &m_header, sizeof(m_header));
	return nuTRUE;
}

nuBOOL nuCLogData::AddRecord(nuVOID *pBuffer, nuUINT nLen)
{
	if (!pBuffer || !IsOpen() || nLen>m_header.size)
	{
		return nuFALSE;
	}
	nuWORD idx = 0;
	if (m_header.now==m_header.max)
	{
		idx = m_table[0];
		nuMemmove(m_table, &m_table[1], (m_header.now-1)<<1);
		--m_header.now;
		m_table[m_header.now] = idx;
	}
	else
	{
		idx = m_table[m_header.now];
	}

	nuBOOL bRes = WriteAtPos(idx, pBuffer, nLen);
	if (bRes)
	{
		++m_header.now;
		if (FlushFileInfo(m_header.now))
		{
			return nuTRUE;
		}
		else
		{
			--m_header.now;
			return nuFALSE;
		}
	}
	return nuFALSE;
}

nuVOID nuCLogData::DelRecord(nuUINT idx)
{
	if (IsOpen())
	{
		nuMemmove(&m_table[idx], &m_table[idx-1], (m_header.now-idx-1)<<1);
		--m_header.now;
		FlushFileInfo(m_header.now);
	}
}

nuVOID nuCLogData::ClearRecord()
{
	if (IsOpen())
	{
		m_header.now = 0;
		FlushFileInfo(m_header.now);
	}
}

nuUINT nuCLogData::GetRecord(nuUINT sIdx, nuUINT nMax, nuVOID *pBuffer, nuUINT nBufferLen)
{
	if (!IsOpen() || !pBuffer)
	{
		return 0;
	}
	nuBYTE *p = (nuBYTE*)pBuffer;
	nuUINT nRes = 0;
	nMax = NURO_MIN(nBufferLen/m_header.size, nMax);
	while (nRes<nMax)
	{
		if ((sIdx+nRes+1)>(m_header.now))
		{
			break;
		}
		if (!m_file.ReadData(
			sizeof(m_header)+sizeof(m_table)+m_table[sIdx+nRes]*m_header.size
			, p, m_header.size) )
		{
			break;
		}
		++nRes;
		p+=m_header.size;
	}
	return nRes;
}

nuBOOL nuCLogData::WriteAtPos(nuUINT idx, nuVOID *pBuffer, nuUINT nLen)
{
	m_file.Seek(
		sizeof(m_header)+sizeof(m_table)+m_header.size*idx, NURO_SEEK_SET);
	if (m_file.Write(pBuffer, nLen, 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}
nuBOOL nuCLogData::FlushFileInfo(nuUINT num )
{
	m_file.Seek(0, NURO_SEEK_SET);
	if (m_file.Write(&m_header, sizeof(m_header)+(num<<1), 1)!=1)
	{
		return nuFALSE;
	}
	return nuTRUE;
}

#include "NewDes.h"
#include "ActiveCode.h"

/*const */nuBYTE g_fh_Key[9];// = "F392EEF9";
/*const */nuBYTE g_os_Uid[17];// = "000BA00378943451";
// char g_mapver[8];


const char *g_prio_table[] = {"ah", "cq", "gs", "ha", "hi", "jl", "nx", 
                              "sc", "sn", "xj", "zj", "bj", "fj", "gx", 
                              "hb", "hj", "js", "ln", "om", "sd", "sx",  
                              "xz", "gd", "gz", "he", "hn", "jx", "nm",
                              "qh", "sh", "tj", "xg", "yn"};

int to16char(nuBYTE c)
{
    switch (c)
    {
        case 0x41: case 0x61: return 0x0A;
        case 0x42: case 0x62: return 0x0B;
        case 0x43: case 0x63: return 0x0C;
        case 0x44: case 0x64: return 0x0D;
        case 0x45: case 0x65: return 0x0E;
        case 0x46: case 0x66: return 0x0F;
        default: return (c - 0x30);
    }
}

int to16char(nuWCHAR wc)
{
	wc &= 0xFF;
	switch (wc)
	{
	case 0x41: case 0x61: return 0x0A;
	case 0x42: case 0x62: return 0x0B;
	case 0x43: case 0x63: return 0x0C;
	case 0x44: case 0x64: return 0x0D;
	case 0x45: case 0x65: return 0x0E;
	case 0x46: case 0x66: return 0x0F;
	default: return (wc - 0x30);
	}
}

bool DesOperate(const unsigned char *os_Uid, 
                unsigned char *result, 
                int result_datalen, 
                const unsigned char deskey[8], 
                bool type)
{
    DES des;
    
    if (!type)
    {
        // Transform to 8
        unsigned char os_8_Uid[8];
        
        for (int i = 0; i < 8; i++)
        {
            os_8_Uid[i] = 0xFF & (to16char(os_Uid[i << 1]) << 4 | to16char(os_Uid[(i << 1) + 1]));
        }
        
        return des.CDesEnter(os_8_Uid, result, result_datalen, deskey, false); 
    }
    else
    {
        return des.CDesEnter(os_Uid, result, result_datalen, deskey, false); 
    }
}

// nuWCHAR *nuGetMapVersion(void)
// {
//     static nuWCHAR wfh_key[20];
//     int i;
// 
//     for (i = 0; i < 8; i++)
//     {
//         wfh_key[i] = g_fh_Key[i];
//     }
// 
//     wfh_key[i] = 0;
// 
//     return wfh_key;
// }

nuVOID nuSetUID(nuWCHAR wos_Uid[])
{
    nuINT i;

    for (i = 0; i < 16; i++)
    {
         g_os_Uid[i] = (nuBYTE)wos_Uid[i];
    }

    g_os_Uid[i] = 0;
}

nuBOOL nuGetActiveCodeFileFullPath(nuCHAR sFile[], int nLen)
{
	
	static nuCHAR sl_ActFile[NURO_MAX_FILE_PATH] = {0};

	if (sl_ActFile[0])
	{
		nuStrncpy(sFile, sl_ActFile, nLen);
	}
	else
	{
		nuCHAR szPath[NURO_MAX_FILE_PATH];
		if (!nuGetModulePathA(NULL, szPath, NURO_MAX_FILE_PATH))
		{
			return nuFALSE;
		}

		nuStrcpy(sl_ActFile, szPath);
		nuStrcat(sl_ActFile, "setting\\activecode.dat");
		nuStrncpy(sFile, sl_ActFile, nLen);
	}

	return nuTRUE;
}

nuBOOL nuFindFH(nuBYTE sFH[], int nLen)
{
	nuINT i;
	nuCHAR szPath[NURO_MAX_FILE_PATH];
	nuFILE *fp;

	if (!nuGetModulePathA(NULL, szPath, NURO_MAX_FILE_PATH))
	{
		return nuFALSE;
	}

	for (i = 0; i < sizeof (g_prio_table) / sizeof (g_prio_table[0]); i++)
	{
		nuCHAR szTmpFile[NURO_MAX_FILE_PATH];

		nuStrcpy(szTmpFile, szPath);
		nuStrcat(szTmpFile, "map\\");
		nuStrcat(szTmpFile, g_prio_table[i]);
		nuStrcat(szTmpFile, "\\");
		nuStrcat(szTmpFile, g_prio_table[i]);
		nuStrcat(szTmpFile, ".fh");

		fp = nuFopen(szTmpFile, NURO_FS_RB);

		if (fp)
		{
			break;
		}
	}

	if (!fp)
	{
		// now ,fp == NULL sugguests no FH file.
		return nuFALSE;
	}

	if (1 != nuFread(sFH, 8, 1, fp))
	{
		nuFclose(fp);
		return nuFALSE;
	}

	nuFclose(fp);
	return nuTRUE;
}

nuBOOL nuSaveActiveCode(nuBYTE res[], nuINT len)
{
	nuCHAR szTmpFile[NURO_MAX_FILE_PATH];
	nuFILE *fp;

	if (!nuGetActiveCodeFileFullPath(szTmpFile, sizeof (szTmpFile)))
	{
		return nuFALSE;
	}

	if (!(fp = nuFopen(szTmpFile, NURO_FS_WB)))
	{
		return nuFALSE;
	}

	nuFseek(fp, 4, NURO_SEEK_SET);

	if (1 != nuFwrite(res, 8, 1, fp))
	{
		nuFclose(fp);
		return nuFALSE;
	}

	nuFclose(fp);

	return nuTRUE;
}

nuBOOL nuCheckValid(nuBYTE res_to_cmp[8])
{
	nuBYTE res[9] = {0};

	DesOperate((const unsigned char *)g_os_Uid, res, 8, g_fh_Key, false);

	if (nuMemcmp(res, res_to_cmp, 8))
	{
		return nuFALSE;
	}

	return nuTRUE;
}

nuBOOL nuCheckValidKey(const nuWCHAR *wsInputKey, nuUINT nLen)
{
	nuBYTE sInputKey[20] = {0};
	int i;

	if (nLen != 16)
	{
		return nuFALSE;
	}

	for (i = 0; i < 8; i++)
	{
		sInputKey[i] = (nuBYTE)(0xFF & (to16char(wsInputKey[i << 1]) << 4 | to16char(wsInputKey[(i << 1) + 1])));
	}

	sInputKey[i] = 0;

	if (!nuCheckValid(sInputKey))
	{
		return nuFALSE;
	}

	nuSaveActiveCode(sInputKey, 8);

	return nuTRUE;
}

nuBOOL nuCheckIsActived(void)
{
	nuCHAR szActiveFile[NURO_MAX_PATH];
	nuCHAR sFH[9] = {0};
	nuBYTE res[9] = {0};
	nuBOOL bOK = nuFALSE;

	if (!nuGetActiveCodeFileFullPath(szActiveFile, sizeof (szActiveFile)))
	{
		return nuFALSE;
	}
	
	if (!nuFindFH(g_fh_Key, (sizeof (g_fh_Key)) - 1))
	{
		return nuFALSE;
	}

	nuFILE *fp = nuFopen(szActiveFile, NURO_FS_RB);

	if (!fp)
	{
		return nuFALSE;
	}

	nuFseek(fp, 4, NURO_SEEK_SET);
	nuSIZE nActiveFileLen = nuFgetlen(fp);

	while (1 == nuFread(res, 8, 1, fp))
	{
		if (nuCheckValid(res))
		{
			bOK = nuTRUE;
			break;
		}
	}

	if (bOK)
	{
		if (nActiveFileLen > 12)
		{
			nuFclose(fp);
			nuSaveActiveCode(res, 8);	
		}
		else
		{
			nuFclose(fp);
		}		
	}

    return bOK;
}

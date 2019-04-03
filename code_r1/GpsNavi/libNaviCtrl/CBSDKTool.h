#ifndef	__CBSDK_TOOL_H__
#define	__CBSDK_TOOL_H__

#include "base_types.h"
#ifdef __cplusplus
extern "C"{
#endif


nuINT CBSDKTool_SetLibResDir(nuTCHAR* pwcLibPath, nuTCHAR* pwcDataPath);

nuSHORT CBSDKTool_SpecifyLanguage(nuINT nVsrLangID);

nuSHORT CBSDKTool_SpecifyLanguageEx(nuBYTE byPrimaryLang, nuBYTE bySubLang);


#ifdef __cplusplus
}
#endif


#endif
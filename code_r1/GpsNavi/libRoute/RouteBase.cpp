
#include "RouteBase.h"

#ifdef DEF_MAKETHRROUTE
	nuTCHAR				MakeThr_MapPath[NURO_MAX_PATH];
#endif

nuTCHAR	RootPath[NURO_MAX_PATH] = {0};
nuBOOL	b_Drive			           = nuFALSE;
nuBOOL	b_StopRoute		           = nuFALSE;

nuVOID GetRootPathStep2()//?Q?Î¨ç¦?äµ?????|
{
	nuLONG i;
	nuGetModulePath(NULL,RootPath,NURO_MAX_PATH/*sizeof(RootPath)*/);
}

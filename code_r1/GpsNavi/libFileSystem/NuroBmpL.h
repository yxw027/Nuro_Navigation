#ifndef __CNUROBMPL_H__20080324
#define __CNUROBMPL_H__20080324

#include"NuroDefine.h"

class CNuroBmpL
{
public:
	CNuroBmpL();
	~CNuroBmpL();

public:
	static nuBOOL FillNuroBmp(const nuTCHAR *, PNURO_BMP temp);
	static nuBOOL ReadNuroBmp(const nuTCHAR*, PNURO_BMP temp);
};
#endif

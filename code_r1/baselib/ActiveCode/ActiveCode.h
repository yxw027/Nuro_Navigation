#ifndef __ACTIVECODE_H__
#define __ACTIVECODE_H__


#include "../NuroDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

// Check is Actived
nuBOOL nuCheckIsActived(nuVOID);

//nuWCHAR *nuGetMapVersion(nuVOID);

nuVOID nuSetUID(nuWCHAR wos_Uid[]);


nuBOOL nuCheckValidKey(const nuWCHAR *wsInputKey, nuUINT nLen);


#ifdef __cplusplus
}
#endif

#endif


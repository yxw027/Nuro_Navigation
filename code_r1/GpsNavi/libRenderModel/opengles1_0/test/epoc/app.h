// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 12-18-2004   Iwan Junianto       support both s60 and uiq
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#ifndef __APP_H__
#define __APP_H__

#ifdef TARGET_S60
#include <aknapp.h>
#else
#include <eikapp.h>
#endif

class CApaDocument;

class CApp: public 
#ifdef TARGET_S60
    CAknApplication
#else
    CEikApplication
#endif
{
public:
	TUid AppDllUid() const;
	
private:	
	CApaDocument* CreateDocumentL();
};

#endif
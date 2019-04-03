// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#include "app.h"

EXPORT_C CApaApplication* NewApplication()
{
	return new CApp;
}

GLDEF_C TInt E32Dll(TDllReason)
{
	return KErrNone;
}
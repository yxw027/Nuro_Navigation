// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#include <apparc.h>
#include "app.h"
#include "doc.h"

const TUid KUidTest = {0x10202DA0};

TUid CApp::AppDllUid() const
{
	return KUidTest;
}

CApaDocument* CApp::CreateDocumentL()
{
	return CDoc::NewL(*this);
}

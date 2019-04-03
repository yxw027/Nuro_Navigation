// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#include <eikappui.h>
#include <eikapp.h>
#include <e32std.h>
#include "doc.h"
#include "appui.h"


CDoc::CDoc(CEikApplication& aApp)
	: CEikDocument(aApp)
{
}

void CDoc::ConstructL()
{
}

CDoc::~CDoc()
{
}

CDoc* CDoc::NewL(CEikApplication& aApp)
{
	CDoc *self = new (ELeave) CDoc(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
}

CEikAppUi* CDoc::CreateAppUiL()
{
	return new (ELeave) CAppUi;
}
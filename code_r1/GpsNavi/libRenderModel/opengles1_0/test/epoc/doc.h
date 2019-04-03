// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#ifndef __DOC_H__
#define __DOC_H__

#include <eikdoc.h>

class CEikApplication;
class CEikAppUi;

class CDoc: public CEikDocument
{
public:
	static CDoc* NewL(CEikApplication& aApp);
	CDoc(CEikApplication& aApp);
	~CDoc();
	
private:
	CEikAppUi* CreateAppUiL();	
	void ConstructL();
};

#endif
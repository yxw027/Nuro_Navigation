// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 12-18-2004   Iwan Junianto       support both s60 and uiq
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#ifndef __APPUI_H__
#define __APPUI_H__

#ifdef TARGET_S60
#include <aknappui.h>
#else
#include <eikappui.h>
#endif

class CAppView;
class CEikMenuPane;


class CAppUi: public 
#ifdef TARGET_S60
    CAknAppUi
#else
    CEikAppUi
#endif
{
public:
	void ConstructL();
	~CAppUi();
	
	void HandleCommandL(TInt aCmd);	

private:
#ifndef TARGET_S60
	void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
#endif
	
	CAppView* iAppView;	// has

};

#endif
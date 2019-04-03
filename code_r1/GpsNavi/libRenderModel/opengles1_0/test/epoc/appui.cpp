// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 12-18-2004   Iwan Junianto       support both s60 and uiq
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#include <eikon.hrh>
#include <eikenv.h>
#include <coecntrl.h>
#include <eikmenup.h>
#include "appui.h"
#include "appview.h"
#include <test.rsg>

void CAppUi::ConstructL()
{
	BaseConstructL();
	iAppView = CAppView::NewL(ClientRect());
	AddToStackL(iAppView);
}

CAppUi::~CAppUi()
{
	if (iAppView) {
		RemoveFromStack(iAppView);
		delete iAppView;
		iAppView = NULL;
	}
}

void CAppUi::HandleCommandL(TInt aCmd)
{
	switch (aCmd) {
        case 200: // bitmap
            iAppView->ShowBitmap();
            break;
        case 201: // ogles test
            iAppView->ShowTest();
            break;
#ifdef TARGET_S60
        case EAknSoftkeyExit:
#endif
		case EEikCmdExit:
            iAppView->Exit();
			Exit();
			break;
	}
}

#ifndef TARGET_S60
void CAppUi::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
{
	if (aMenuId==R_MENUPANE) {
//#ifdef _DEBUG
		CEikMenuPaneItem::SData itemData;
		TBuf<16> closeCmdTxt;
		iEikonEnv->ReadResource(closeCmdTxt, R_CLOSE_CMD);
		itemData.iText = closeCmdTxt;
		itemData.iCommandId = EEikCmdExit;
		itemData.iFlags = 0;
		itemData.iCascadeId = 0;
		aMenuPane->AddMenuItemL(itemData);
//#endif
	}
}
#endif

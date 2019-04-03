// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 12-18-2004   Iwan Junianto       support both s60 and uiq
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#ifndef __APPVIEW_H__
#define __APPVIEW_H__

#include <e32std.h>
#include <coecntrl.h>
#include "appglobal.h"

class CAppView: public CCoeControl
{
public:
	static CAppView* NewL(const TRect& aRect);
	
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);	
	virtual TCoeInputCapabilities InputCapabilities() const;
    
    CWindowGc* Gc() const;
    RWindow* Win() const;
    
    void Exit();
    void ShowBitmap();
    void ShowTest();

    mutable TAppGlobal iData;	

private:
	void ConstructL(const TRect& aRect);
	virtual void Draw(const TRect&) const;	
    static int OnTimer(void* aArg);

    TBool iShowBitmap;
    TBool iShowTest;
    TCallBack* iCallBack;
    CPeriodic* iTimer;
#ifdef __MARM__
	RLibrary iDll;
#endif
};

#endif
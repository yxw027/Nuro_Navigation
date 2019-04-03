// ====================================================================================
//
// Symbian GUI application template
//
// ------------------------------------------------------------------------------------
//
// 12-18-2004   Iwan Junianto       support both s60 and uiq
// 11-05-2004   Iwan Junianto       initial version
// ====================================================================================

#include <w32std.h>
#include "appview.h"

extern int InitOpenGL(CWindowGc* gc, RWindow* win, TAppGlobal* data);
extern void ShutdownOpenGL(TAppGlobal* data);
extern void PaintProc(CWindowGc* gc, const TRect& aRect, TAppGlobal* data);

#ifdef __MARM__
_LIT(KOglesDll, "gles_cl.dll");
_LIT(KBitmapFile, "c:\\system\\apps\\test\\dodge.mbm");
#else
_LIT(KBitmapFile, "z:\\system\\apps\\test\\dodge.mbm");
#endif

CAppView* CAppView::NewL(const TRect& aRect)
{
	CAppView* self = new (ELeave) CAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	
	return self;
}

void CAppView::ConstructL(const TRect& aRect)
{
	// prevent kernel panic when app run for the first time by pre-loading the dll manually
#ifdef __MARM__
	User::LeaveIfError(iDll.Load(KOglesDll));
#endif
	CreateWindowL();
	SetRect(aRect);
	ActivateL();
    InitOpenGL(Gc(), Win(), &iData);
    iCallBack = new (ELeave) TCallBack(&CAppView::OnTimer, this);
}

void CAppView::Exit()
{
    delete iCallBack;
    delete iTimer;
    iTimer = NULL;
    ShutdownOpenGL(&iData);
#ifdef __MARM__
	iDll.Close();
#endif
}

void CAppView::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();

    if (iShowBitmap)
    {
        CFbsBitmap* bitmap = NULL;
        TRAPD(err, bitmap = new (ELeave) CFbsBitmap);
        if (err!=KErrNone)
            return;

        err = bitmap->Load(KBitmapFile, 0);
        gc.BitBlt(TPoint(0,0), bitmap, aRect);
        delete bitmap;

        return;
    }

    if (iShowTest)
    {
        PaintProc(&gc, aRect, &iData);
        return;
    }

    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(KRgbWhite);
    gc.DrawRect(aRect);
}

void CAppView::ShowBitmap()
{
    delete iTimer;
    iTimer = NULL;
    iShowTest = EFalse;
    iShowBitmap = ETrue;
    DrawNow();
}

void CAppView::ShowTest()
{
    iShowBitmap = EFalse;
    iShowTest = ETrue;
    iTimer = CPeriodic::NewL(0);
    // 40ms timer
    iTimer->Start(TTimeIntervalMicroSeconds32(0),TTimeIntervalMicroSeconds32(40000),*iCallBack);
}

CWindowGc* CAppView::Gc() const
{
    return &SystemGc();
}

RWindow* CAppView::Win() const
{
    return &Window();
}

TCoeInputCapabilities CAppView::InputCapabilities() const
{
	return TCoeInputCapabilities(TCoeInputCapabilities::ENavigation);
}

TKeyResponse CAppView::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
{
	return EKeyWasNotConsumed;
}

int CAppView::OnTimer(void* aArg)
{
    CAppView* self = (CAppView*)aArg;
    self->iData.angle += 0.5f;
    self->DrawNow();
    return 0;
}

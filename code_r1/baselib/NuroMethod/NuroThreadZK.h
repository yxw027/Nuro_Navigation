// NuroThreadZK.h: interface for the CNuroThreadZK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUROTHREADZK_H__A360702C_1ACD_46A6_8071_C7A317B22F7A__INCLUDED_)
#define AFX_NUROTHREADZK_H__A360702C_1ACD_46A6_8071_C7A317B22F7A__INCLUDED_

#include "../NuroDefine.h"

#define _NURO_THRAD_DEFAULT_STACK_SIZE              (64*1024)

class CNuroThreadZK  
{
private:
    static nuDWORD ThdAction(nuPVOID lpVoid);
public:
	CNuroThreadZK();
	virtual ~CNuroThreadZK();

    nuBOOL  CreateNuroThread( nuUINT nStackSize = _NURO_THRAD_DEFAULT_STACK_SIZE, 
                              nuUINT nPriority  = NURO_THREAD_PRIORITY_LOW );
    nuVOID  DeleteNuroThread();

    nuVOID  Sleep(nuDWORD nMinisecond)
    {
        nuSleep(nMinisecond);
    }
    
protected:
    virtual nuDWORD ThreadAction()
    {
        while( m_bThrWork )
        {
            nuSleep(50);
        }
        return 1;
    }
protected:
    nuHANDLE        m_thdHandle;
    nuBOOL          m_bThrWork;
private:
    nuBOOL          m_bOutThread;
};

#endif // !defined(AFX_NUROTHREADZK_H__A360702C_1ACD_46A6_8071_C7A317B22F7A__INCLUDED_)

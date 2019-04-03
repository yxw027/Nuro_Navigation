// Ctrl_RuleSet.h: interface for the CCtrl_RuleSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTRL_RULESET_H__13618A0F_5941_4802_BE90_E97FCCE7BB8C__INCLUDED_)
#define AFX_CTRL_RULESET_H__13618A0F_5941_4802_BE90_E97FCCE7BB8C__INCLUDED_

#define Def_Rule_Character		7

typedef struct Tag_RuleSetItemStru
{
	nuSHORT Speed;
	nuSHORT Character;
}RuleSetItemStru,*pRuleSetItemStru;

typedef struct Tag_RuleSetStru
{
	nuLONG				l_TotalSupportCount;
	nuLONG				l_SupportMode;
	nuLONG				l_HighestSpeed;
	nuLONG				l_3dp3ds;//0:3ds 1:3dp
	nuSHORT				**l_Weighting;
	RuleSetItemStru		**pRuleItem;
}RuleSetStru,*pRuleSetStru;

class CCtrl_RuleSet  : public RuleSetStru
{
public:
	CCtrl_RuleSet();
	virtual ~CCtrl_RuleSet();

	nuBOOL ReadRoutingRule(nuLONG l_RoutingRule);
	nuVOID ReleaseRoutingRule();
	nuTCHAR	FName[NURO_MAX_PATH];
	nuLONG BaseSpeed_KMH2MS;
	nuFILE *RuleSetFile;
	#ifdef _DEBUG
		int  RuleAlocCount;
		int  RuleFreeCount;
	#endif
};
extern CCtrl_RuleSet	*g_pRoutingRule;


#endif // !defined(AFX_CTRL_RULESET_H__13618A0F_5941_4802_BE90_E97FCCE7BB8C__INCLUDED_)

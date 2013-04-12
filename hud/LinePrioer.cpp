#include "LinePrioer.h"

LinePrior::~LinePrior(){
	CC_SAFE_RELEASE_NULL(m_caPinSteped);
	CC_SAFE_RELEASE_NULL(m_cdPins);
	CCLOG(">LinePrior Dec.");
}

void LinePrior::activate()
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(this);
		}
	}
}

void LinePrior::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

bool LinePrior::f_init()
{
	do{
		CCSprite* cs_line = CCSprite::create("Images/Linear.png");
		cs_line->setAnchorPoint(CCPointZero);
		cs_line->setPosition(ccp(0,30));
		addChild(cs_line);
		m_cdPins = new CCDictionary();

		m_caPinSteped = NULL;

		return true;
	}while(0);
	return false;
}

void LinePrior::addPin( Chara* a_chara, string s)
{
	LinePin* t_lp = new LinePin();

	t_lp->name	=	s;
	t_lp->setAnchorPoint(ccp(0.5,0));
	t_lp->m_iFlag = 0;						//TODO: Flag is now of no use.
	t_lp->m_iStep = 160 + rand()%20;		//TODO: Decide step by chara() attributes.
	if(t_lp->m_iStep>180){
		t_lp->m_fFactor = (t_lp->m_iStep - 180)/30.0;
	}

	t_lp->f_init();
	m_cdPins->setObject(t_lp,t_lp->name);

	addChild(t_lp);
}

void LinePrior::refresh_pin()
{
	CCDictElement* t_cde = NULL;
	CCDICT_FOREACH(m_cdPins,t_cde){
		LinePin* t_llp = (LinePin*) t_cde->getObject();
		int t_iStep = t_llp->m_iStep;

		if(t_iStep > 180){
			t_llp->setPosition(ccp((t_iStep - 180)/t_llp->m_fFactor,0));
		}else{
			t_llp->setPosition(ccp(t_iStep,0));
		}
	}
}

void LinePrior::tick()
{
	CC_SAFE_RELEASE_NULL(m_caPinSteped);
	m_caPinSteped = new CCArray();

	CCDictElement* t_cde = NULL;
	CCDICT_FOREACH(m_cdPins,t_cde){
		LinePin* t_llp = (LinePin*) t_cde->getObject();
		t_llp->m_iStep--;

		if(t_llp->m_iStep == 0){
			m_caPinSteped->addObject(t_llp);
		}
	}
	
	if(m_caPinSteped->count() > 0) 
		activate();
	else
		refresh_pin();
}

void LinePrior::removePin( string s)
{
	m_cdPins->removeObjectForKey(s);
}

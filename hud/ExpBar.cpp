#include "ExpBar.h"
#include "Macros.h"

ExpBar::ExpBar(string psz, int m){
	max = m;

	initWithFile("Images/lifebarbac.png");

	m_cpt = CCProgressTimer::create(CCSprite::create(psz.c_str()));
	m_cpt->setType(kCCProgressTimerTypeBar);
	m_cpt->setMidpoint(ccp(0,0.5));
	m_cpt->setBarChangeRate(ccp(1,0));
	m_cpt->setAnchorPoint(ccp(0,0));
	m_cpt->setPosition(ccp(1,1));
	m_cpt->setOpacity(100);
	addChild(m_cpt);
	
	autorelease();
}

ExpBar::ExpBar(const char* psbase,string psz, int m){
	max = m;

	initWithFile(psbase);

	m_cpt = CCProgressTimer::create(CCSprite::create(psz.c_str()));
	m_cpt->setType(kCCProgressTimerTypeBar);
	m_cpt->setMidpoint(ccp(0,0.5));
	m_cpt->setBarChangeRate(ccp(1,0));
	m_cpt->setAnchorPoint(ccp(0,0));
	m_cpt->setPosition(ccp(1,1));
	m_cpt->setOpacity(30);
	addChild(m_cpt);

	autorelease();
}

void ExpBar::revalue(int a_max){
	max = a_max;
	RefreshView();
}

void ExpBar::setval(int a_ival){
	val = a_ival;
	
	RefreshView();
}

void ExpBar::RefreshView(){
	percentage = (float) val/max;
	percentage *= 100;
	m_cpt->setPercentage(percentage);
	
}
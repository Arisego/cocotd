#include "LifeBar.h"
#include "Macros.h"

LifeBar::LifeBar(string psz, int m){
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

	m_clbBase = CCLabelBMFont::create(CCString::createWithFormat("/%d",max)->getCString(), FNT_CHN);
	m_clbBase->setAnchorPoint(CCPointZero);
	m_clbBase->setPosition(ccp(50,-2));
	addChild(m_clbBase,1);

	//m_clbVal = CCLabelBMFont::create(CCString::createWithFormat("%d",max)->getCString(), FNT_CHN);
	m_clbVal = CCLabelBMFont::create();
	m_clbVal->setFntFile(FNT_CHN);
	m_clbVal->setAnchorPoint(ccp(1,0));
	m_clbVal->setPosition(ccp(50,-2));
	addChild(m_clbVal,1);
	
	autorelease();
}

void LifeBar::revalue(int a_max){
	max = a_max;
	m_clbBase->setString(CCString::createWithFormat("/%d",max)->getCString());
	RefreshView();
}

void LifeBar::setval(int a_ival){
	val = a_ival;
	
	RefreshView();
}

void LifeBar::RefreshView(){
	percentage = (float) val/max;
	percentage *= 100;
	m_clbVal->setCString(CCString::createWithFormat("%d",val)->getCString());
	m_cpt->setPercentage(percentage);
		

	
	if(percentage > 15){
		m_clbVal->setColor(ccWHITE);
		return;
	}
	
	m_clbVal->setColor(ccRED);
	
}
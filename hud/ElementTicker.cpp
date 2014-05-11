#include "ElementTicker.h"


bool ElementTicker::f_init()
{
	do{
		m_iStep = 0;
		m_csClock = CCSprite::create("Images/element_clock.png");
		m_csClock->setAnchorPoint(ccp(0.5,0.5));
		m_csClock->setPosition(CCPointZero);
		m_csClock->setRotation(m_iStep);
		addChild(m_csClock);
		

		return true;
	}while(0);
	return false;
}

void ElementTicker::tick()
{
	++m_iStep;
	if(m_iStep>360) m_iStep = 0;
	m_csClock->setRotation(m_iStep);
	//CCLOG(">Element Tick:%d",m_iStep);
}

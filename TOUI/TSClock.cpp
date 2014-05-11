#include "TSClock.h"

#define P_X 800
#define P_Y 88

TSClock::TSClock(CCNode* aParent)
{
	mcParent = aParent;
	init();
}

bool TSClock::init()
{
	do{
		CCLog(">[TSClock] init()");

		m_iStep = 0;
		m_csClock = ColorSprite::createWithSpriteFrameName("TSCLock.bmp");
		m_csClock->setAnchorPoint(ccp(0.5,0.5));
		m_csClock->setPosition(ccp(P_X,P_Y));
		m_csClock->setRotation(m_iStep);
		m_csClock->setScale(0.6);
		mcParent->addChild(m_csClock);

		m_csCLockB = ColorSprite::createWithSpriteFrameName("TSClockB.bmp");
		m_csCLockB->setAnchorPoint(ccp(0.5,0.5));
		m_csCLockB->setPosition(ccp(P_X,P_Y));
		m_csCLockB->setRotation(-m_iStep);
		m_csCLockB->setScale(0.6);
		mcParent->addChild(m_csCLockB);

		Dismiss();
		
		return true;
	}while(0);
	return false;
}

void TSClock::tick()
{
	if(!m_bRotate) return;
	++m_iStep;
	if(m_iStep>360) m_iStep = 0;
	m_csClock->setRotation(m_iStep);
	m_csCLockB->setRotation(-m_iStep);
	//CCLOG(">Element Tick:%d",m_iStep);
}

void TSClock::Dismiss()
{
	m_csClock->setVisible(false);
	m_csCLockB->setVisible(false);
	m_bRotate = false;
}

void TSClock::Showup()
{
	m_csClock->setVisible(true);
	m_csCLockB->setVisible(true);
	m_bRotate = true;
}

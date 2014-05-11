#include "LgStat.h"
#include "EventCenter.h"
#include "SoundManager.h"



LgStat::LgStat()
	:mbStat(false)
{
	init();
	CCLog(">[LgStat] ComPo || Set EventCenter To this");
	EventCenter::sharedEventCenter()->SetDefndLck(this);
}

LgStat::~LgStat()
{
	CCLog(">[LgStat] DeCom || Set EventCenter Null");
	EventCenter::sharedEventCenter()->SetDefndLck(NULL);
}

bool LgStat::init()
{
	mSp_Outer = CCSprite::create("Images/TUI/lg_out.png");
	mSp_Outer->setAnchorPoint(ccp(0.5,0.5));
	mSp_Outer->setPosition(CCPointZero);
	addChild(mSp_Outer);
	mSp_Outer->setVisible(false);

	mSp_Inner = CCSprite::create("Images/TUI/lg_in.png");
	mSp_Inner->setAnchorPoint(ccp(0.5,0.5));
	mSp_Inner->setPosition(CCPointZero);
	addChild(mSp_Inner);
	mSp_Inner->setVisible(false);

	//Sun
	mPs_sun = CCParticleSun::create();
	mPs_sun->setTexture(CCTextureCache::sharedTextureCache()->addImage("Images/fire.png"));
	mPs_sun->setPosition(CCPointZero);
	//mPs_sun->setTotalParticles(300);
	mPs_sun->setLife(0.3f);
	mPs_sun->setScale(2.5);
	mPs_sun->setVisible(false);

	mSp_Tick = CCSprite::create("Images/TUI/lg_tick.png");
	mSp_Tick->setAnchorPoint(ccp(0.5,0.5));
	mSp_Tick->setPosition(CCPointZero);
	addChild(mSp_Tick,-1);
	mSp_Tick->setVisible(false);

	addChild(mPs_sun);
	setPosition(100,100);

	miState = 0;
	m_bIsEnabled = false;
	setVisible(false);
	return true;
}

void LgStat::CountDown(float time, CCObject* target, SEL_MenuHandler selector)
{
	if(miState) return;
	setVisible(true);
	m_bIsEnabled = true;
	miState = 2;

	
	mSp_Outer->setVisible(true);
	mPs_sun->setVisible(true);
	mSp_Tick->setVisible(true);


	m_pListener = target;
	m_pfnSelector = selector;

	mfDel = time;
	mfSum = time;
	scheduleUpdate();
}

void LgStat::update(float dt)
{
	//CCLog(">[LgStat] update");
	switch (miState)
	{
	case 2:
		{
			mfDel -= dt;
			if(mfDel <=0 ) CountOver();

			float ara = mfDel/mfSum;
			//mPs_sun->setTotalParticles(ara*300);
			mPs_sun->setScale(1.5*ara + 1.0);
			mSp_Tick->setRotation(ara*360);

			break;
		}
	default:
		break;
	}
}

void LgStat::activate()
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(this);
		}
	}
}

void LgStat::CountOver()
{
	miState = 0;
	if(mfDel<=0) activate();
	StatOver();
}

void LgStat::GiveUpCont()
{
	if(miState != 2) return;
	CountOver();
}

void LgStat::SetDefend(bool abStat)
{
	if(miState == 2){
		EventCenter::sharedEventCenter()->mbDefendLock = false;
		SoundManager::sharedSoundManager()->PlayDisablSFX();
		return;
	}
	miState = 1;
	
	if(mbStat == abStat) return;
	mbStat = abStat;

	if(mbStat){
		setVisible(true);
		mSp_Outer->setVisible(true);
		mSp_Inner->setVisible(true);
		mSp_Inner->runAction(CCRepeatForever::create( CCSequence::create(CCFadeTo::create(0.57, 104), CCFadeTo::create(0.3, 255), NULL)) );
	}else{
		StatOver();
	}
}

void LgStat::StatOver()
{

	mfDel = 0;
	m_bIsEnabled = false;

	mSp_Inner->setVisible(false);
	mSp_Inner->stopAllActions();
	mSp_Inner->setOpacity(255);
	mSp_Outer->setVisible(false);
	mPs_sun->setScale(2.5);
	mPs_sun->setVisible(false);
	mSp_Tick->setVisible(false);


	setVisible(false);
	unscheduleUpdate();
	miState = 0;
}

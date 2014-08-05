#include "Ab_Next.h"
#include "..\Corrode\Classes\SoundManager.h"

AbNext::AbNext(int aw, int ah)
{
	Container::Container();

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;
	spriteHover		= NULL;

	spriteNormal = CCSprite::create("Images/UI/next.png");
	spriteNormal->setAnchorPoint(CCPointZero);
	spriteNormal->setPosition(CCPointZero);
	addChild(spriteNormal);

	CCActionInterval* tcMove = CCMoveBy::create(0.2f,ccp(40,0));
	spriteNormal->runAction(CCRepeatForever::create(CCSequence::create(
		tcMove
		, CCDelayTime::create(0.3f)
		, tcMove->reverse()
		, NULL
		)));

	msNextText = CCSprite::create("Images/UI/next_text.png");
	msNextText->setAnchorPoint(CCPointZero);
	msNextText->setPosition(ccp(0,70));
	msNextText->setVisible(true);
	addChild(msNextText);

	msNextText->runAction(CCRepeatForever::create(CCSequence::create(
		CCFadeTo::create(0.1f,200)
		, CCDelayTime::create(0.1f)
		, CCFadeTo::create(0.1f,255)
		, NULL
		)));
	
	spriteNormal->runAction(CCRepeatForever::create(CCSequence::create(
		CCFadeTo::create(0.2,100)
		, CCDelayTime::create(0.3)
		, CCFadeTo::create(0.2,255)
		, NULL
		)));
	 
	msNextTextHov = CCSprite::create("Images/UI/next_text_hov.png");
	msNextTextHov->setAnchorPoint(CCPointZero);
	msNextTextHov->setPosition(ccp(0,70));
	addChild(msNextTextHov);
	msNextTextHov->setVisible(false);

	spriteHover = CCSprite::create("Images/UI/next_hov.png");
	spriteHover->setAnchorPoint(CCPointZero);
	spriteHover->setPosition(CCPointZero);
	spriteHover->setVisible(false);
	addChild(spriteHover);

	labelAtlas = new CCLabelBMFont();
	labelAtlas->autorelease();
	labelAtlas->setVisible(false);
	addChild(labelAtlas);

	m_obContentSize.width = aw;
	m_obContentSize.height = ah;

	onNormal();
	m_iState = C_STATE_NORMAL;

	scheduleUpdate();
}

void AbNext::onHover()
{
	Container::onHover();
	msNextText->setVisible(false);
	msNextTextHov->setVisible(true);
}

void AbNext::onSelect()
{
	if(C_STATE_SELECT == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_HOVERD;
	SoundManager::sharedSoundManager()->PlayDecideSFX();
	//spriteNormal->setVisible(true);
}

void AbNext::onNormal()
{
	Container::onNormal();
	msNextText->setVisible(true);
	msNextTextHov->setVisible(false);
}

void AbNext::Pause()
{
	spriteNormal->pauseSchedulerAndActions();
}

void AbNext::Resume()
{
	spriteNormal->resumeSchedulerAndActions();
}


#include "FLLayer.h"
#include "MenuScene.h"
#include "SoundManager.h"
#include "EventCenter.h"

#define START_TIME 0.3f	/* <开始特效的时间长度 */
#define MENU_SCENE	((MenuScene*) getParent())

void FlLayer::Pause()
{
	setTouchEnabled(false);
	EventCenter::sharedEventCenter()->unsetBmCake(this);
}

void FlLayer::Resume()
{
	setTouchEnabled(true);
	EventCenter::sharedEventCenter()->setBmCake(this);
	setVisible(true);
}

void FlLayer::Close()
{
	setTouchEnabled(false);
	SoundManager::sharedSoundManager()->StopMusic();
	EventCenter::sharedEventCenter()->unsetBmCake(this);
	setVisible(false);
}

bool FlLayer::init()
{
	do 
	{
		mcsBackGround = CCSprite::create("Images/UI/fl_bg.jpg");
		mcsBackGround->setAnchorPoint(CCPointZero);
		mcsBackGround->setPosition(CCPointZero);
		//mcsBackGround->setOpacity(0);
		addChild(mcsBackGround,-10);

		mcsLeft		= CCSprite::create("Images/UI/ervs.png");
		mcsLeft->setAnchorPoint(CCPointZero);
		mcsLeft->setPosition(CCPointZero);
		addChild(mcsLeft,1);

		mcsRight	= CCSprite::create("Images/UI/kymu.png");
		mcsRight->setAnchorPoint(CCPointZero);
		mcsRight->setPosition(ccp(460,0));
		addChild(mcsRight,1);

		mcsTitle	= CCSprite::create("Images/UI/fl_title.png");
		mcsTitle->setAnchorPoint(CCPointZero);
		mcsTitle->setPosition(ccp(480,480));
		addChild(mcsTitle,2);

		mcsSLaves	= CCSprite::create("Images/UI/fl_sl.png");
		mcsSLaves->setAnchorPoint(CCPointZero);
		mcsSLaves->setPosition(ccp(10,28));
		addChild(mcsSLaves,2);
		
		mcsEnterIn	= CCSprite::create("Images/UI/enter_in.png");
		mcsEnterIn->setAnchorPoint(CCPointZero);
		mcsEnterIn->setPosition(ccp(366,246));
		addChild(mcsEnterIn,2);

		mBtnEnterIn = new HSButton("Images/UI/enter_in.png","Images/UI/enter_in.png",200,60, false);
		mBtnEnterIn->setactivator(this, menu_selector(FlLayer::BGCallBack));
		mBtnEnterIn->setAnchorPoint(CCPointZero);
		mBtnEnterIn->setPosition(ccp(366,246));
		addChild(mBtnEnterIn,3);

		setContentSize(CCSizeMake(700,600));

		return true;
	} while (false);
	return false;
}

void FlLayer::BeginShow()
{
	MENU_SCENE->LockControl();

	mBtnEnterIn->setVisible(false);
	mBtnEnterIn->onDisable();

	setTouchEnabled(false);
	mcsBackGround->setOpacity(0);
	mcsBackGround->runAction(CCFadeIn::create(START_TIME));

	mcsTitle->setOpacity(0);
	mcsTitle->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.4*START_TIME), CCFadeIn::create(0.6*START_TIME)));

	mcsLeft->setPosition(ccp(100,0));
	mcsLeft->runAction(CCMoveTo::create(START_TIME, ccp(0,0)));
	mcsRight->setPosition(ccp(360,0));
	mcsRight->runAction(CCMoveTo::create(START_TIME, ccp(560,0)));

	mcsSLaves->setVisible(false);
	mcsSLaves->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(START_TIME), CCToggleVisibility::create()));
	mcsEnterIn->setVisible(false);
	mcsEnterIn->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(START_TIME), CCToggleVisibility::create()));

	runAction(CCSequence::createWithTwoActions(CCDelayTime::create(START_TIME+0.1f), CCCallFunc::create(this, callfunc_selector(FlLayer::ELoadFinal))));
	EventCenter::sharedEventCenter()->setBmCake(this);
}

void FlLayer::ELoadFinal()
{
	
	SoundManager::sharedSoundManager()->PlayMusic("sound/bgm/NE.1.ogg");
	
	mcsEnterIn->runAction(CCRepeatForever::create(
		CCSequence::createWithTwoActions(
			CCFadeTo::create(0.7f,88),
			CCFadeTo::create(0.4f,255)
		)
		));

	mBtnEnterIn->setVisible(true);
	mBtnEnterIn->setEnability(true);
	MENU_SCENE->UnLockControl();
	setTouchEnabled(true);
}

void FlLayer::BGCallBack(CCObject* pSender)
{
	MENU_SCENE->GoToGame();
}

void FlLayer::right_click()
{
	MENU_SCENE->ChangeState(1);
}
//
//void FlLayer::byTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
//{
//	exit(99);
//}

void FlLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate( this, kCCMenuHandlerPriority, false );
}

FlLayer::~FlLayer()
{
	CCLog(">[FlLayer] Deco~");
}


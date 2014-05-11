#include "Spliter.h"
#include "GameManager.h"
#include "Component\MapLayerComp.h"
#include "TSRound.h"

//////////////////////////////////////////////////////////////////////////
// Init

//Spliter *Spliter::mSharedSpliter = NULL;

Spliter *Spliter::sharedSpliter()
{
	if (mSharedSpliter == NULL)
	{
		mSharedSpliter = new Spliter();
		if (!mSharedSpliter || !mSharedSpliter->init())
		{
			CC_SAFE_DELETE(mSharedSpliter);
		}

	}

	return mSharedSpliter;

}


//void Spliter::purgeSharedSpliter()
//{
//	CC_SAFE_DELETE(mSharedSpliter);
//
//}

//////////////////////////////////////////////////////////////////////////

bool Spliter::init()
{
	// <使用缓存系统的模版
	std::string pszFileName = "Images/TUI/split";
	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	CCSpriteBatchNode *sheet = CCSpriteBatchNode::create(CCString::createWithFormat("%s.png",pszFileName.c_str())->getCString());
	cache->addSpriteFramesWithFile(CCString::createWithFormat("%s.plist",pszFileName.c_str())->getCString());

	// <载入组件
	mTSClock = new TSClock(this);
	mTSRound = new TSRound(this);
	//mTSClock->setVisible(false);

	// <载入完成
	addChild(sheet);
	return true;
}

Spliter::~Spliter()
{
	delete mTSRound;
	delete mTSClock;
	CCLog(">[Spliter] DeComp.No Leak.");
}

Spliter::Spliter()
	: mbOpened(false)
	, miUpdateCount(0)
	, mbShowed(false)
{
	CCLog(">[Spliter] Building...");
	init();
}

//////////////////////////////////////////////////////////////////////////

void Spliter::update(float delta)
{
	//CCLog(">[Spliter] update()");
	if(getParent() == NULL){
		CCLog(">[Spliter]+++++++++++++++++++++++++++++");
	}
	if(!mbOpened) return;
	//CCLog(">[Spliter] update() opened");
	mTSClock->tick();
}

void Spliter::ShowClock()
{
	if(!mbOpened) return;
	mTSClock->Showup();
	++miUpdateCount;
	scheduleUpdate();
}

void Spliter::DisClock()
{
	if(!mbOpened) return;
	--miUpdateCount;
	mTSClock->Dismiss();
	if(!miUpdateCount) unscheduleUpdate();
}

//////////////////////////////////////////////////////////////////////////

void Spliter::f_open()
{
	mbOpened = true;
	setVisible(true);
}

void Spliter::f_pause()
{
	mbOpened = false;
	setVisible(false);
}

void Spliter::ShowRound()
{
	if(!mbOpened) return;
	mbShowed = true;
	GameManager::sharedLogicCenter()->ml->f_stopcontrol();
	GameManager::sharedLogicCenter()->ml->mbAutoLogic = false;

	mTSRound->show();
} 

void Spliter::RoundOver()
{
	CCLog(">[Spliter] RoundOver() || Title Round is over.");
	GameManager::sharedLogicCenter()->ml->f_resumecon();
	GameManager::sharedLogicCenter()->ml->mbAutoLogic = true;
}

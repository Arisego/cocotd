#include "TSRound.h"
#include "actions\CCActionInstant.h"
#include "Spliter.h"
#include "GameManager.h"
#include "Component\MapLayerComp.h"

#define P_X 450
#define P_Y 300

TSRound::TSRound(CCNode* aParent)
{
	mParent = aParent;
	init();
}

bool TSRound::init()
{
	mcsText = ColorSprite::createWithSpriteFrameName("Fight.png");
	mcsText->setAnchorPoint(ccp(0.5,0.5));
	mcsText->setPosition(ccp(P_X,P_Y));
	mParent->addChild(mcsText);
	mcsText->setVisible(false);

	CCArray *animFrames = CCArray::create();  
	CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();

	char str[64] = {0};  
	for (int i = 2; i <= 11; ++i) {  
		sprintf(str, "crashAa%03d.png", i);  
		CCSpriteFrame *frame = cache->spriteFrameByName(str);  
		CCLog(str);  
		// <添加帧到数组  
		animFrames->addObject(frame);  
	}  

	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.08);  
	animation->setRestoreOriginalFrame(true);  
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "Frame");  

	return true;
}

void TSRound::show()
{
	CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("crashAa002.png");  

	mcsFrame = ColorSprite::createWithSpriteFrame(frame);  
	mcsFrame->setPosition(ccp(P_X, P_Y));  
	mcsFrame->setAnchorPoint(ccp(0.5,0.5));
	mParent->addChild(mcsFrame);

	CCAnimation *anim = CCAnimationCache::sharedAnimationCache()->animationByName("Frame");  
	CCAnimate *animate = CCAnimate::create(anim);  

	CCLog(">[TSRound] show()");
	//mParent->update(0);			// <[Debug]
	mcsFrame->runAction(CCSequence::createWithTwoActions(animate, CCCallFunc::create(this,callfunc_selector(TSRound::Finish))));
	//Finish();

	mcsText->setVisible(true);
}

void TSRound::Finish()
{
	CCLog(">[TSRound] Finish()");
	mcsFrame->removeFromParent();
	mcsText->setVisible(false);
	GameManager::sharedLogicCenter()->ml->m_Spliter->RoundOver();
}


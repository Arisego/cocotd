#include "hud/SoulBall.h"

#define MAXBALLNUM 10

bool SoulBall::init()
{
	do 
	{
		GenerateSB();
		miALive = MAXBALLNUM;
		return true;
	} while (0);
	return false;
}

void SoulBall::setSum( int ai )
{
	if(ai == miSSum) return;
	else if(ai<miSSum){
		for(int i = ai; i < miSSum;++i){
			mics_SBList[i]->setVisible(false);
		}
	}else{
		for(int i = miSSum; i < ai;++i){
			mics_SBList[i]->setVisible(true);
		}
	}

	miSSum = ai;
	
}

void SoulBall::GenerateSB()
{
	removeAllChildren();
	mics_SBList.clear();
	miSSum = 0;
	//////////////////////////////////////////////////////////////////////////
	CCSpriteBatchNode* sheet = CCSpriteBatchNode::create("Images/hud_ball_light.png");
	for(int i = 0; i<MAXBALLNUM; ++i){
		CCSprite* tcs = CCSprite::create("Images/hud_ball_light.png");
		tcs->setAnchorPoint(CCPointZero);
		tcs->setPosition(ccp(i*20,1));
		tcs->setVisible(false);
		sheet->addChild(tcs);
		mics_SBList.push_back(tcs);
	}
	addChild(sheet);
}

void SoulBall::setAli( int ai )
{
	if(ai<0){	
		CCLog(">[SoulBall]Error. Soul can not handle value below zero. Check tho logic.");
		//CCAssert(ai>=0,">[SoulBall]Error. Soul can not handle value below zero. Check tho logic.");
		ai = 0;
	}

	CCLog(">[SoulBall] setAli() | %d | %d", ai, miALive);
	if(ai == miALive) return;
	else if(ai<miALive){
		for(int i = ai; i < miALive;++i){
			mics_SBList[i]->setOpacity(111);
		}
	}else{
		for(int i = miALive; i < ai;++i){
			mics_SBList[i]->setOpacity(255);
		}
	}
	miALive = ai;
	CCLog(">[SoulBall] setAli() | Over");
}

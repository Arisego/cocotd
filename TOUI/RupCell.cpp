#include "RupCell.h"
#include "Macros.h"

RupCell::RupCell(int i)
{
	f_init();
	showStr(CCString::createWithFormat("+%d",i)->getCString());
}

bool RupCell::f_init()
{
	CCLog("1");
	CCSprite* mcs = CCSprite::create("Images/Rugr_value.png");
	mcs->setAnchorPoint(CCPointZero);
	mcs->setPosition(ccp(-50,5));
	addChild(mcs);

	labelAtlas = CCLabelBMFont::create("++", FNT_CHN);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setScale(0.8);
	labelAtlas->setPosition(ccp(60,5));
	labelAtlas->setColor(ccYELLOW);
	addChild(labelAtlas,1);
	CCLog("2");

	setVisible(false);
	setAnchorPoint(CCPointZero);

	mcs->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.2, 150), CCFadeTo::create(0.8, 255), NULL)));
	//autorelease();
	
	return true;
}

void RupCell::showStr(const char* asName)
{
	labelAtlas->setString(asName);
	setVisible(true);
}


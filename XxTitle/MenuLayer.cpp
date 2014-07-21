#include "MenuLayer.h"
#include "XxUI\HSBotton.h"
#include "..\Corrode\Classes\GameManager.h"

TitleMMenu::TitleMMenu()
{
	setTouchEnabled(true);

	BYLayerDescendant* mbArea = new BYLayerDescendant();
	mbArea->autorelease();
	mbArea->setContentSize(ccp(100,600));
	mbArea->setAnchorPoint(CCPointZero);
	mbArea->setPosition(ccp(800,0));
	addChild(mbArea);

	HSButton* tHsB1 = new HSButton("Images/UI/fl.png","",63,96,false);
	tHsB1->setactivator(this, menu_selector(TitleMMenu::menuCallback));
	tHsB1->setAnchorPoint(CCPointZero);
	tHsB1->setPosition(ccp(36,477));
	mbArea->addChild(tHsB1);

	HSButton* tHsB2 = new HSButton("Images/UI/config.png","",64,112,false);
	tHsB2->setactivator(this, menu_selector(TitleMMenu::menuCallback2));
	tHsB2->setAnchorPoint(CCPointZero);
	tHsB2->setPosition(ccp(35,8));
	mbArea->addChild(tHsB2);

}

void TitleMMenu::registerWithTouchDispatcher()
{
	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority+1, true);
	//pDirector->getTouchDispatcher()->addStandardDelegate(this, kCCMenuHandlerPriority+1);
}


TitleMMenu::~TitleMMenu()
{

}

void TitleMMenu::menuCallback(CCObject* sender)
{
	CCLOG("Prepare to new scene.");
	setTouchEnabled(false);
	GameManager::sharedGameManager()->runSceneWithId(GameManager::SCENE_PLAY);
}

void TitleMMenu::menuCallback2(CCObject* sender)
{
	;
}

void TitleMMenu::onQuit(CCObject* sender)
{
	CCDirector::sharedDirector()->end();
	//  getCocosApp()->exit();
}

void TitleMMenu::Pause(){
	//menu->setTouchEnabled(false);
	setTouchEnabled(false);
}

void TitleMMenu::Close(){
	setTouchEnabled(false);
}

void TitleMMenu::Resume(){
	setTouchEnabled(true);
}
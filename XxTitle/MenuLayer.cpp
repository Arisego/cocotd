#include "MenuLayer.h"
#include "GameManager.h"
#include "MenuScene.h"

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
	tHsB1->setTag(2);
	mbArea->addChild(tHsB1);
	mvBtns.push_back(tHsB1);

	HSButton* tHsB2 = new HSButton("Images/UI/config.png","",64,112,false);
	tHsB2->setactivator(this, menu_selector(TitleMMenu::menuCallback2));
	tHsB2->setAnchorPoint(CCPointZero);
	tHsB2->setPosition(ccp(35,8));
	tHsB2->setTag(9);
	mbArea->addChild(tHsB2);
	mvBtns.push_back(tHsB2);

	msMenu = CCSprite::create("Images/UI/menu.png");
	msMenu->setAnchorPoint(CCPointZero);
	msMenu->setPosition(ccp(0,0));
	addChild(msMenu,10);

}

void TitleMMenu::registerWithTouchDispatcher()
{
	CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority+1, true);
}


TitleMMenu::~TitleMMenu()
{
	CCLog(">[MenuLayer] Deco~");
}

void TitleMMenu::menuCallback(CCObject* sender)
{
	CCLOG("Prepare to new scene.");
	((MenuScene*) getParent())->ChangeState(2);
	//setTouchEnabled(false);
	//GameManager::sharedGameManager()->runSceneWithId(GameManager::SCENE_PLAY);
}

void TitleMMenu::menuCallback2(CCObject* sender)
{
	((MenuScene*) getParent())->GoToConfig();
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

void TitleMMenu::EnableAllBtns(int atag)
{
	for(std::vector<HSButton*>::iterator it = mvBtns.begin(); it != mvBtns.end(); ++it){
		if((*it)->getTag() != atag) (*it)->setEnability(true);
		else (*it)->setEnability(false);
	}
}

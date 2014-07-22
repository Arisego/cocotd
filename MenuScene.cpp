#include "System.h"

#include "MenuScene.h"
#include "utils/FileIO.h"
#include "utils/SpriteTextMenuItem.h"  
#include "utils/MoviePlayer.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "CCSpriterX.h"
#include "utils/ALSingle.h"
#include "layers_scenes_transitions_nodes/CCLayer.h"

#include "utils/COShaderNode.h"
#include "XxUI/HSBotton.h"

using namespace cocos2d;


MenuScene::~MenuScene(){
	CCLog(">MenuScece descontruct");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    bool bRet = false;
    do 
    {
		miFlag	= 0;	// StateMachine
		miState = 0;
		CC_BREAK_IF(!CCScene::init());
		mbControlLock = false;
		//////////////////////////////////////////////////////////////////////////
		/* <MainLayer */
		ml = new MainLayer();
		CC_BREAK_IF(!ml->init());
		ml->setAnchorPoint(CCPointZero);
		ml->setPosition(0,0);
		ml->autorelease();
		addChild(ml, 0);
		AddState(ml);

		//////////////////////////////////////////////////////////////////////////
		/* <Chapter 01 */
		mFlLayer = new FlLayer();
		mFlLayer->init();
		mFlLayer->setVisible(false);
		mFlLayer->setAnchorPoint(CCPointZero);
		mFlLayer->setPosition(CCPointZero);
		mFlLayer->autorelease();
		addChild(mFlLayer,1);
		AddState(mFlLayer);


		//////////////////////////////////////////////////////////////////////////
		/* <菜单 */
		menu = new TitleMMenu();
		menu->autorelease();
		menu->setTouchEnabled(false);
		menu->setAnchorPoint(CCPointZero);
		menu->setPosition(0,0);
		CC_BREAK_IF(! menu);
		addChild(menu,9);
		AddState(menu);
		
		bRet = true;
    } while (0);
    return bRet;
}


void MenuScene::update(float dt){

}

void MenuScene::PreQuit(){	
	StatesManager::PreQuit();
}

void MenuScene::SplashOver()
{
	menu->setTouchEnabled(true);
	miState = 1;
}

void MenuScene::ChangeState(int ais)
{
	if(mbControlLock) return;
	if(miState == ais) return;
	switch (ais)
	{
	case(1):
		OpenOnly(ml);
		ml->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0), CCCallFunc::create(ml,  callfunc_selector(MainLayer::PlayOpMusic))));
		menu->EnableAllBtns();
		UnLockControl();
		break;
	case(2):
		OpenOnly(mFlLayer);
		mFlLayer->BeginShow();
		break;
	default:
		break;
	}
	miState = ais;
}

void MenuScene::LockControl()
{
	mbControlLock = true;
	menu->setTouchEnabled(false);
}

void MenuScene::UnLockControl()
{
	mbControlLock = false;
	menu->setTouchEnabled(true);
}

void MenuScene::GoToGame()
{
	LockControl();
	PreQuit();
	GameManager::sharedGameManager()->runSceneWithId(GameManager::SCENE_PLAY);
}

void MenuScene::NoQuit()
{
	StatesManager::NoQuit();
	menu->EnableAllBtns(miState);
	UnLockControl();
}

void MenuScene::GoToConfig()
{
	e_State = 1;
	miFlag	= 0x0010;
	GameManager::sharedGameManager()->goConfig();
}

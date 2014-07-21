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
		miFlag = 0;	// StateMachine
		CC_BREAK_IF(!CCScene::init());

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
		/* <菜单 */
		menu = new TitleMMenu();
		menu->autorelease();
		menu->setTouchEnabled(false);
		menu->setAnchorPoint(CCPointZero);
		menu->setPosition(0,0);
		CC_BREAK_IF(! menu);
		addChild(menu,100);
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
}

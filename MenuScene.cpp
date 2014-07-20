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

static const char s_MenuItem[]            = "Images/menuitemsprite.png";
static const char s_MenuItem2[]			  = "Images/menuitemsprite2.png";
static const char s_MenuItem3[]			  = "Images/menuitemsprite3.png";

using namespace cocos2d;

#define TAG_WYS 6001
#define MV_TLEN 3

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
		 
	    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
		CC_BREAK_IF(! CCScene::init());
        
		msBg = CCSprite::create("Images/UI/bg.jpg");
		msBg->setAnchorPoint(ccp(0,1));
		msBg->setPosition(ccp(0,600));
		addChild(msBg,-1);

		msWys = CCSprite::create("Images/UI/wys.png");
		msWys->setAnchorPoint(ccp(0,0));
		msWys->setPosition(ccp(0,0));
		msWys->setOpacity(48);
		msWys->setTag(TAG_WYS);
		addChild(msWys,0);

		msLogo = CCSprite::create("Images/UI/logo.png");
		msLogo->setAnchorPoint(CCPointZero);
		msLogo->setPosition(ccp(44,0));
		addChild(msLogo,8);

		msLogo2 = CCSprite::create("Images/UI/logo.png");
		msLogo2->setAnchorPoint(CCPointZero);
		msLogo2->setPosition(ccp(44,0));
		msLogo2->setOpacity(188);
		addChild(msLogo2,9);


		msMenu = CCSprite::create("Images/UI/menu.png");
		msMenu->setAnchorPoint(CCPointZero);
		msMenu->setPosition(ccp(0,0));
		addChild(msMenu,10);

		msBg->runAction(CCMoveBy::create(MV_TLEN,ccp(0,251)));
		msWys->runAction(CCSequence::create(CCDelayTime::create(MV_TLEN/2), CCFadeTo::create(MV_TLEN*0.7,255), 
			CCCallFunc::create(this, callfunc_selector(MenuScene::ELoadFinal))
			, NULL));

		GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("Shaders/logo.fsh").c_str())->getCString();
		mglProgLogo = new CCGLProgram();
		mglProgLogo->initWithVertexShaderByteArray(ccPositionTexture_vert, fragSource);
		mglProgLogo->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
		mglProgLogo->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
		mglProgLogo->link();
		mglProgLogo->updateUniforms();
		msLogo->setShaderProgram(mglProgLogo);

		//////////////////////////////////////////////////////////////////////////
		/* <菜单 */
		MenuLayerMainMenu *menu = new MenuLayerMainMenu();
		menu->autorelease();
		CC_BREAK_IF(! menu);
		this->addChild(menu,100);
		AddState(menu);

		//////////////////////////////////////////////////////////////////////////
		/* <播放音乐 */
		runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0), CCCallFunc::create(this, callfunc_selector(MenuScene::PlayOpMusic))));

		scheduleUpdate();
        bRet = true;
    } while (0);

    return bRet;
}

void MenuScene::ELoadFinal()
{
	removeChildByTag(TAG_WYS);
	msWys = CCSprite::create("Images/UI/wys2.png");
	msWys->setAnchorPoint(ccp(0,0));
	msWys->setPosition(ccp(0,0));
	msWys->setTag(TAG_WYS);
	addChild(msWys,0);
}

void MenuScene::PlayOpMusic()
{
	SoundManager::sharedSoundManager()->PlayMusic("sound/bgm/op.ogg");
}


void MenuScene::update(float dt){

}

void MenuScene::PreQuit(){	
	StatesManager::PreQuit();
}
///
//------------------------------------------------------------------
//
// MenuLayerMainMenu
//
//------------------------------------------------------------------
MenuLayerMainMenu::MenuLayerMainMenu()
{
    setTouchEnabled(true);

	BYLayerDescendant* mbArea = new BYLayerDescendant();
	mbArea->autorelease();
	mbArea->setContentSize(ccp(100,600));
	mbArea->setAnchorPoint(CCPointZero);
	mbArea->setPosition(ccp(800,0));
	addChild(mbArea);

	HSButton* tHsB1 = new HSButton("Images/UI/fl.png","",63,96);
	tHsB1->setactivator(this, menu_selector(MenuLayerMainMenu::menuCallback));
	tHsB1->setAnchorPoint(CCPointZero);
	tHsB1->setPosition(ccp(36,477));
	mbArea->addChild(tHsB1);

}

void MenuLayerMainMenu::registerWithTouchDispatcher()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority+1, true);
	//pDirector->getTouchDispatcher()->addStandardDelegate(this, kCCMenuHandlerPriority+1);
}


MenuLayerMainMenu::~MenuLayerMainMenu()
{

}

void MenuLayerMainMenu::menuCallback(CCObject* sender)
{
	CCLOG("Prepare to new scene.");
	GameManager::sharedGameManager()->runSceneWithId(GameManager::SCENE_PLAY);
}

void MenuLayerMainMenu::allowTouches(float dt)
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->setPriority(kCCMenuHandlerPriority+1, this);
    unscheduleAllSelectors();
    CCLOG("TOUCHES ALLOWED AGAIN");
}

void MenuLayerMainMenu::menuCallback2(CCObject* sender)
{
	SoundManager::sharedSoundManager()->PlayMusic("sound/bgm/op.ogg");
	//menu->setEnabled(false);
	//GameManager::sharedGameManager()->preConfig(0x18,0x10,0);

    //((CCLayerMultiplex*)m_pParent)->switchTo(2);
}

void MenuLayerMainMenu::onQuit(CCObject* sender)
{
	CCDirector::sharedDirector()->end();
  //  getCocosApp()->exit();
}

void MenuLayerMainMenu::Pause(){
	//menu->setTouchEnabled(false);
	setTouchEnabled(false);
}

void MenuLayerMainMenu::Close(){
	//menu->setTouchEnabled(false);
	setTouchEnabled(false);
}

void MenuLayerMainMenu::Resume(){
	menu->ReSume();
	setTouchEnabled(true);
}
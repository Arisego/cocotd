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
		msLogo->setPosition(ccp(44,46));
		addChild(msLogo,8);

		msLogo2 = CCSprite::create("Images/UI/logo.png");
		msLogo2->setAnchorPoint(CCPointZero);
		msLogo2->setPosition(ccp(44,46));
		msLogo2->setOpacity(156);
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
    CCMenuItemFont::setFontSize( 40 );
    CCMenuItemFont::setFontName("Courier New");

    setTouchEnabled(true);
	float sf = CCDirector::sharedDirector()->getContentScaleFactor();	// <缩放比例
    
    CCSprite* spriteNormal; 
	CCSprite* spriteSelected;
	CCSprite* spriteDisabled;

	spriteNormal = CCSprite::create(s_MenuItem, CCRectMake(0,23*2/sf,70/sf,23/sf));
    spriteSelected = CCSprite::create(s_MenuItem, CCRectMake(0,23*1/sf,70/sf,23/sf));
    spriteDisabled = CCSprite::create(s_MenuItem, CCRectMake(0,23*0,70/sf,23/sf));

    SpriteTextMenuItem* item1 = new SpriteTextMenuItem();
	item1->autorelease();
	item1->initWithNormalSprite(spriteNormal, spriteSelected, spriteDisabled, this, menu_selector(MenuLayerMainMenu::menuCallback) );
	item1->settext(ConfigManager::sharedConfigManager()->GetConfigS("gm_begin").c_str(), "fonts/corrode.fnt", 24,0,5);
	item1->setTag(100);

	spriteNormal = CCSprite::create(s_MenuItem2, CCRectMake(0,23*2/sf,70/sf,23/sf));
	spriteSelected = CCSprite::create(s_MenuItem2, CCRectMake(0,23*1/sf,70/sf,23/sf));
	spriteDisabled = CCSprite::create(s_MenuItem2, CCRectMake(0,23*0,70/sf,23/sf));

	SpriteTextMenuItem* item2 = new SpriteTextMenuItem();
	item2->autorelease();
	item2->initWithNormalSprite(spriteNormal, spriteSelected, spriteDisabled,this, menu_selector(MenuLayerMainMenu::menuCallback2) );
	item2->settext(ConfigManager::sharedConfigManager()->GetConfigS("gm_continue").c_str(), "fonts/corrode.fnt", 24,0,5);
	item2->setTag(101);

	spriteNormal = CCSprite::create(s_MenuItem3, CCRectMake(0,23*2/sf,70/sf,23/sf));
	spriteSelected = CCSprite::create(s_MenuItem3, CCRectMake(0,23*1/sf,70/sf,23/sf));
	spriteDisabled = CCSprite::create(s_MenuItem3, CCRectMake(0,23*0,70/sf,23/sf));

	SpriteTextMenuItem* item3 = new SpriteTextMenuItem();
	item3->autorelease();
	item3->initWithNormalSprite(spriteNormal, spriteSelected, spriteDisabled,this, menu_selector(MenuLayerMainMenu::onQuit) );
	item3->settext(ConfigManager::sharedConfigManager()->GetConfigS("gm_quit").c_str(), "fonts/corrode.fnt", 24,0,5);
	item3->setTag(102);

    menu = MouseMenu::menuWithItems( item1,item2, item3, NULL);
    menu->alignItemsVertically();


    // elastic effect
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    int i=0;
    CCMenuItem* child;
    CCArray * pArray = menu->getChildren();
    CCObject* pObject = NULL;
    CCARRAY_FOREACH(pArray, pObject)
    {
        if(pObject == NULL)
            break;

        child = (CCMenuItem*)pObject;
		child->setOpacity(0);
        child->runAction( CCFadeIn::create(0.6));
        i++;
    }

    addChild(menu);
	menu->setAnchorPoint(ccp(0.5,0.5));
    menu->setPosition(ccp(s.width/2+135, s.height/2-50));
	menu->setScale(1.3);
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
	menu->setEnabled(false);
	menu->UnRegist();
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
	SoundManager::sharedSoundManager()->PlayMusic("sound/1.ogg");
	menu->setEnabled(false);
	GameManager::sharedGameManager()->preConfig(0x18,0x10,0);

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
#include "System.h"

#include "MenuScene.h"
#include "utils/FileIO.h"
#include "utils/SpriteTextMenuItem.h"  
//#include "utils\VideoPlayer.h"
#include "utils/MoviePlayer.h"

//#include "LogicCenter.h"
#include "GameManager.h"
#include "SoundManager.h"
#include "CCSpriterX.h"
#include "utils/ALSingle.h"
#include "layers_scenes_transitions_nodes/CCLayer.h"


static const char s_MenuItem[]            = "Images/menuitemsprite.png";
static const char s_MenuItem2[]			  = "Images/menuitemsprite2.png";
static const char s_MenuItem3[]			  = "Images/menuitemsprite3.png";

using namespace cocos2d;

MenuScene::~MenuScene(){
	CCLOG(">MenuScece descontruct");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    bool bRet = false;
    do 
    {
		//////////////////////////////////////////////////////////////////////////
		// StateMachine
		//
		miFlag = 0;
		//////////////////////////////////////////////////////////////////////////
		 
	    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
		CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
        //////////////////////////////////////////////////////////////////////////
        // super init first
        //////////////////////////////////////////////////////////////////////////
		CC_BREAK_IF(! CCScene::init());

//		VideoPlayer* vp = new VideoPlayer();
//		this->addChild(vp);
        
		CCImage* image = new CCImage();
		unsigned long filesize;
		FileIO* fi = new FileIO("Images/fg.cop");
		unsigned char* pBuffer = fi->getFileData("fg.png",&filesize);
		image->initWithImageData((void*)pBuffer, filesize, CCImage::kFmtPng);
		delete pBuffer;
		CCTextureCache::sharedTextureCache()->addUIImage(image,"Images/fg.png");	//Mask?
		image->autorelease();
		//CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->addUIImage(image,"Images/background.png"));
		CC_SAFE_DELETE(fi);

		// BackGroudLayer
		CCLayer* bg = CCLayer::create();
		CCSprite* BgImg = CCSprite::create("Images/fg.png");	// <将文件标识与目录设置相同以做调试用，现在没有设置。


		//BgImg->setAnchorPoint(CCPoint(0.5,0.5));
		BgImg->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
		bg->addChild(BgImg);

		 CCParticleSystem* m_pParticleSystem = new CCParticleSystemQuad();
		 m_pParticleSystem->autorelease();
		 m_pParticleSystem->initWithFile("particle/first.plist");
		 bg->addChild(m_pParticleSystem);
		 m_pParticleSystem->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height + origin.y));
		

		//CCParticleSnow* m_Snow = new CCParticleSnow();
		//m_Snow->initWithTotalParticles(500);
		//bg->addChild(m_Snow);
		//m_Snow->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height + origin.y));
		

		this->addChild(bg);

		/* <菜单 */
		MenuLayerMainMenu *menu = new MenuLayerMainMenu();
		menu->autorelease();
		CC_BREAK_IF(! menu);
		this->addChild(menu);
		AddState(menu);

		scheduleUpdate();

        bRet = true;
    } while (0);

    return bRet;
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
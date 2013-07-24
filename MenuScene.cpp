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


static const char s_MenuItem[]            = "Images/menuitemsprite.png";
static const char s_SendScore[]           = "Images/SendScoreButton.png";
static const char s_PressSendScore[]      = "Images/SendScoreButtonPressed.png";

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

		//背景层
		CCLayer* bg = CCLayer::create();
		CCSprite* BgImg = CCSprite::create("Images/fg.png");	//将文件标识与目录设置相同以做调试用，现在没有设置。


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


		//菜单
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
    // Font Item
	float sf = CCDirector::sharedDirector()->getContentScaleFactor();	//缩放比例
    
    CCSprite* spriteNormal = CCSprite::create(s_MenuItem, CCRectMake(0,23*2/sf,115/sf,23/sf));
    CCSprite* spriteSelected = CCSprite::create(s_MenuItem, CCRectMake(0,23*1/sf,115/sf,23/sf));
    CCSprite* spriteDisabled = CCSprite::create(s_MenuItem, CCRectMake(0,23*0,115/sf,23/sf));
	//spriteNormal->draw();
    //dynamic_cast<CCNode*>(mgr)->addChild(spriteNormal);
    //dynamic_cast<CCNode*>(mgr)->addChild(spriteSelected);
    //dynamic_cast<CCNode*>(mgr)->addChild(spriteDisabled);

    SpriteTextMenuItem* item1 = new SpriteTextMenuItem();
	item1->autorelease();
	item1->initWithNormalSprite(spriteNormal, spriteSelected, spriteDisabled, this, menu_selector(MenuLayerMainMenu::menuCallback) );
	item1->settext("StartGame", "fonts/Scissor Cuts.TTF", 24);
	/*item1->setAnchorPoint(CCPoint(0,0));*/
    // Image Item
    CCMenuItem* item2 = CCMenuItemImage::create(s_SendScore, s_PressSendScore, this, menu_selector(MenuLayerMainMenu::menuCallback2) );

    // Label Item (LabelAtlas)
    CCLabelAtlas* labelAtlas = CCLabelAtlas::create("9876543210", "fonts/labelatlas.png",16/sf, 24/sf, '.');
    CCMenuItemLabel* item3 = CCMenuItemLabel::create(labelAtlas, this, menu_selector(MenuLayerMainMenu::menuCallbackDisabled) );
    item3->setDisabledColor( ccc3(32,32,64) );
    item3->setColor( ccc3(200,200,255) );
	
    
    // Font Item
    CCMenuItemFont *item4 = CCMenuItemFont::create("I toggle enable items", this, menu_selector(MenuLayerMainMenu::menuCallbackEnable) );

    item4->setFontSizeObj(20);
    item4->setFontName("Marker Felt");
    
    // Label Item (CCLabelBMFont)
    CCLabelTTF* label = CCLabelTTF::create("配置a", "fonts/Scissor Cuts.TTF", 44);
    CCMenuItemLabel* item5 = CCMenuItemLabel::create(label, this, menu_selector(MenuLayerMainMenu::menuCallbackConfig));

    // Testing issue #500
    item5->setScale( 0.8f );
	

    // Events
    CCMenuItemFont::setFontName("Marker Felt");
    CCMenuItemFont *item6 = CCMenuItemFont::create("Priority Test", this, menu_selector(MenuLayerMainMenu::menuCallbackPriorityTest));
	

    // Font Item
    CCMenuItemFont* item7 = CCMenuItemFont::create("Quit", this, menu_selector(MenuLayerMainMenu::onQuit));
    
    CCActionInterval* color_action = CCTintBy::create(0.5f, 0, -255, -255);
    CCActionInterval* color_back = color_action->reverse();
    CCFiniteTimeAction* seq = CCSequence::create(color_action, color_back, NULL);
    item7->runAction(CCRepeatForever::create((CCActionInterval*)seq));

    menu = MouseMenu::menuWithItems( item1, item2, item3, item4, item5, item6, item7, NULL);;
    menu->alignItemsVertically();
    
    
    // elastic effect
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    
    int i=0;
    CCNode* child;
    CCArray * pArray = menu->getChildren();
    CCObject* pObject = NULL;
    CCARRAY_FOREACH(pArray, pObject)
    {
        if(pObject == NULL)
            break;

        child = (CCNode*)pObject;

        CCPoint dstPoint = child->getPosition();
        int offset = (int) (s.width/2 + 50);
        if( i % 2 == 0)
            offset = -offset;
        
        child->setPosition( ccp( dstPoint.x + offset, dstPoint.y) );
        child->runAction( 
                          CCEaseElasticOut::create(CCMoveBy::create(2, ccp(dstPoint.x - offset,0)), 0.35f) 
                        );
        i++;
    }

    m_disabledItem = item3; item3->retain();
    m_disabledItem->setEnabled( false );
	item3->setVisible(false);

    addChild(menu);
    menu->setPosition(ccp(s.width/2, s.height/2));
}

void MenuLayerMainMenu::registerWithTouchDispatcher()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority+1, true);
	//pDirector->getTouchDispatcher()->addStandardDelegate(this, kCCMenuHandlerPriority+1);
}


MenuLayerMainMenu::~MenuLayerMainMenu()
{
    m_disabledItem->release();
}

void MenuLayerMainMenu::menuCallback(CCObject* sender)
{
	CCLOG("Prepare to new scene.");
	GameManager::sharedGameManager()->runSceneWithId(GameManager::SCENE_PLAY);
}

void MenuLayerMainMenu::menuCallbackConfig(CCObject* sender)
{
    GameManager::sharedGameManager()->preConfig(0x18,0x10,0);
}

void MenuLayerMainMenu::allowTouches(float dt)
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->setPriority(kCCMenuHandlerPriority+1, this);
    unscheduleAllSelectors();
    CCLOG("TOUCHES ALLOWED AGAIN");
}

void MenuLayerMainMenu::menuCallbackDisabled(CCObject* sender) 
{
    // hijack all touch events for 5 seconds
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->setPriority(kCCMenuHandlerPriority-1, this);
    schedule(schedule_selector(MenuLayerMainMenu::allowTouches), 5.0f);
    CCLOG("TOUCHES DISABLED FOR 5 SECONDS");
}

void MenuLayerMainMenu::menuCallbackEnable(CCObject* sender) 
{
    m_disabledItem->setEnabled(! m_disabledItem->isEnabled() );
	m_disabledItem->setVisible(true);
}

void MenuLayerMainMenu::menuCallback2(CCObject* sender)
{
	SoundManager::sharedSoundManager()->PlayMusic("sound/1.ogg");
    //((CCLayerMultiplex*)m_pParent)->switchTo(2);
}

void MenuLayerMainMenu::menuCallbackPriorityTest(CCObject* pSender)
{
	ALSingle::sharedALSingle()->StopAll();
    //((CCLayerMultiplex*)m_pParent)->switchTo(4);
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
	//menu->setTouchEnabled(true);
	setTouchEnabled(true);
}
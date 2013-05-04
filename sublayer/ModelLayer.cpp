#include "ModelLayer.h"
#include "utils/MouseMenu.h"
#include "cocos-ext.h"
#include "utils/SpriteTextMenuItem.h"  

#include "GameManager.h"
#include "packui/ConfigManager.h"

USING_NS_CC_EXT;

#define NUM_BTN 5
#define CONTENT_TAG 101

static const char s_MenuItem[]            = "Images/menuitemsprite.png";
static const char s_Button[]			  = "Images/button.png";

ModelLayer::ModelLayer(){
	b_StateRunning = true;
	mtTab = NULL;
}

ModelLayer::~ModelLayer(){
	if(mtTab) mtTab->CleanStates();
	//removeAllChildren();
	CCLOG(">Model_Layer_DEC");
	//std::vector<Container*>::iterator it;
	//for(it = m_vBtns.begin(); it != m_vBtns.end(); ++it){
	//	(*it)->removeFromParent();
	//	(*it)->release();
	//}
}


//对于ModelLayer -- 
//目前不作状态机使用
void ModelLayer::Pause(){
	Close();
}

void ModelLayer::Close(){
	if(!b_StateRunning) return;
	b_StateRunning = false;
	setVisible(false);
	setTouchEnabled(false);
}

void ModelLayer::Resume(){
	b_StateRunning = true;
}

//////////////////////////////////////////////////////////////////////////
// Quit
bool ModelLayer::m_qlock = false;

void ModelLayer::preQuit(){
	if(!b_StateRunning || m_qlock) return;
	setTouchEnabled(true);
	m_qlock = true;

	float m_height = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	BYLayerDescendant *mb = new BYLayerDescendant();
	mb->autorelease();
	
	mb->setAnchorPoint(ccp(0.5,0.5));
	mb->setPosition(ccp(vs.width/2,vs.height/2));
	float sf = 1;
	CCSprite* spriteNormal = CCSprite::create(s_MenuItem, CCRectMake(0,23*2/sf,115/sf,23/sf));
	CCSprite* spriteSelected = CCSprite::create(s_MenuItem, CCRectMake(0,23*1/sf,115/sf,23/sf));
	CCSprite* spriteDisabled = CCSprite::create(s_MenuItem, CCRectMake(0,23*0,115/sf,23/sf));
	CCSprite* spriteNormal1 = CCSprite::create(s_MenuItem, CCRectMake(0,23*2/sf,115/sf,23/sf));
	CCSprite* spriteSelected1 = CCSprite::create(s_MenuItem, CCRectMake(0,23*1/sf,115/sf,23/sf));
	CCSprite* spriteDisabled1 = CCSprite::create(s_MenuItem, CCRectMake(0,23*0,115/sf,23/sf));
	//spriteNormal->draw();
	//dynamic_cast<CCNode*>(mgr)->addChild(spriteNormal);
	//dynamic_cast<CCNode*>(mgr)->addChild(spriteSelected);
	//dynamic_cast<CCNode*>(mgr)->addChild(spriteDisabled);

	SpriteTextMenuItem* mif_Yes = new SpriteTextMenuItem();
	mif_Yes->initWithNormalSprite(spriteNormal, spriteSelected, spriteDisabled, this, menu_selector(ModelLayer::c_quit_yes) );
	SpriteTextMenuItem* mif_No = new SpriteTextMenuItem();
	mif_No->initWithNormalSprite(spriteNormal1, spriteSelected1, spriteDisabled1, this, menu_selector(ModelLayer::c_quit_no) );
	mif_No->autorelease();
	mif_Yes->autorelease();

	//CCMenuItemFont *mif_Yes = CCMenuItemFont::create("Yes",  this, menu_selector(ModelLayer::c_quit_yes) );
	//CCMenuItemFont *mif_No = CCMenuItemFont::create("No",  this, menu_selector(ModelLayer::c_quit_no) );
	MouseMenu *mbmm = MouseMenu::menuWithItems(mif_Yes,mif_No,NULL);
	mbmm->setAnchorPoint(ccp(0.5,0));
	mbmm->setPosition(ccp(0,mif_Yes->getContentSize().height));
	mbmm->alignItemsHorizontally();
	m_height += 2 * mif_Yes->getContentSize().height;
	mb->addChild(mbmm);
	

	float m_textwidth = 300;
	CCLabelBMFont* m_Text = CCLabelBMFont::create(ConfigManager::sharedConfigManager()->GetConfigS("pre_quit").c_str(), "fonts/CocoTd.fnt");
	//CCLabelTTF *m_Text = CCLabelTTF::create(, "fonts/simhei.ttf", 18, CCSize(m_textwidth, 0), kCCTextAlignmentLeft);
	m_Text->setAnchorPoint(ccp(0.5,0));
	m_Text->setPosition(ccp(0,m_height));
	m_height += m_Text->getContentSize().height;
	mb->addChild(m_Text);
	
	this->addChild(mb);

	CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
	nback->setContentSize(CCSize(m_textwidth+ 33,m_height+ 22));
	nback->setAnchorPoint(ccp(0.5,0));
	nback->setOpacity(100);
	mb->addChild(nback,-1);

	
	
}

void ModelLayer::c_quit_yes(CCObject* sender){
	
	GameManager::purgeSharedGameManager();
}

void ModelLayer::c_quit_no(CCObject* sender){
	removeFromParent();
	m_qlock = false;
	GameManager::sharedGameManager()->noQuit();
}

//////////////////////////////////////////////////////////////////////////
// Config
bool ModelLayer::m_clock = false;

void ModelLayer::preConfig(int type, int flag, int tab){
	if(m_clock) return;			//TODO:切换Flag和Tab
	setTouchEnabled(true);
	mb = NULL;
	mtTab = NULL;

	m_vBtns.clear();
	m_clock = true;
	miType = type;
	miFlag = flag;
	miTab = tab;
	
	if(! (miFlag & miType)) return;	//当前按钮列表不提供这个功能则退出

	m_fHeight = 50;
	m_fWidth = 2.5;

	Add_Button("back",0);
	//根据mType the bit 进行按钮的显示
	m_fHeight = 550 - 30;
	int tiFlag = 1;
	for(int i = 1; i <= NUM_BTN; i++){
		if(miType&tiFlag){
			Add_Button((CCString::createWithFormat("button_%d",i))->getCString(),tiFlag);
			m_fHeight -= 30;
		}
		tiFlag = tiFlag<<1;
	}

	m_bLockSave = m_fHeight > 450;
	Refresh_Button();
	Show_Content();

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/config_back.png"); 
	nback->setContentSize(CCSize(800,600));
	nback->setAnchorPoint(ccp(0,0));
	nback->setOpacity(199);
	addChild(nback,-1);

	CCScale9Sprite* nsplit = CCScale9Sprite::create(s_Button); 
	nsplit->setContentSize(CCSize(5,500));
	nsplit->setAnchorPoint(ccp(0,0));
	nsplit->setPosition(ccp(105,50));
	addChild(nsplit,-1);
}

void ModelLayer::Refresh_Button(){
	std::vector<Container*>::iterator it;
	for(it = m_vBtns.begin(); it != m_vBtns.end(); ++it){
		if((*it)->getTag() != miFlag) (*it)->onNormal();
		else (*it)->onSelect();
	}
}

void ModelLayer::Add_Button(const char* name,int tag){
	string s = ConfigManager::sharedConfigManager()->GetConfigS(name);
	Container* tabc = new Container();
	//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));
	
	tabc->initString(s.c_str(),100,30,"Images/button",this,menu_selector(ModelLayer::buttonback));
	tabc->setTag(tag);
	tabc->setAnchorPoint(ccp(0,0));
	tabc->setPosition(ccp(m_fWidth,m_fHeight));
	addChild(tabc);
	
	m_vBtns.push_back(tabc);
}

void ModelLayer::buttonback(CCObject* sender){
	int itag = ((Container *) sender)->getTag();
	if(miFlag == itag) {
		//CCLOG(">Test clean.");
		return;
	}
	switch(itag){
	case 0:
		{
			
			schedule(schedule_selector(ModelLayer::noConfig),0,1,0.1);
			return;
		}

	}
	
	miFlag = itag;
	
	Show_Content();
	Refresh_Button();
}

/* 显示右侧内容栏 */
void ModelLayer::Show_Content(){
	float m_height = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	if(!mb){
		mb = new BYLayerDescendant();
		mb->autorelease();
		mb->setAnchorPoint(ccp(0,0));
		mb->setPosition(ccp(110,50));
		

		CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
		nback->setAnchorPoint(ccp(0,0));
		nback->setContentSize(CCSize(690,500));
		nback->setOpacity(100);
		mb->addChild(nback,-1);
		addChild(mb);
	}

	mb->removeChildByTag(CONTENT_TAG);

	switch(miFlag){
	case(0x0010):
		{
			mtTab = new ConfigTab();
			break;
		 }
	case(0x0008):
		{
			
			mtTab = new SLTab(m_bLockSave);
			break;
		}
	case(0x0004):
		{
			mtTab = new INFTab();
			break;
		}
	case(0x0002):
		{
			mtTab = new PACTab();
			break;
		}
	case(0x0001):
		{
			mtTab = new STATab();
			break;
		}
	default:
		break;
	}

	mb->addChild(mtTab);
	mtTab->setTag(CONTENT_TAG);
	mtTab->ShowTab(miTab);
	miTab = -2;			//防止跳转后出现bug

}

void ModelLayer::noConfig(float dt){
	m_clock = false;
	GameManager::sharedGameManager()->noConfig();
}
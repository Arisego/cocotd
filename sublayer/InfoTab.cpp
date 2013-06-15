#include "InfoTab.h"

#include "GameManager.h"
#include "packui/MouseController.h"
#include "packui/ListDBView.h"
#include "packui/TOChara.h"


static const char s_Button[]			  = "Images/button.png";
static const char s_MenuItem[]            = "Images/menuitemsprite.png";

InfoTab::~InfoTab(){
	CCLOG(">Info_Tab_DEC");
}

//////////////////////////////////////////////////////////////////////////
InfoTab* InfoTab::m_sharedInfoTab = NULL;

InfoTab* InfoTab::sharedInfoTab(){
	CCLOG(">Prepare for SIT. If no init below, then there might be something wrong.");
	if (m_sharedInfoTab == NULL)
	{
		m_sharedInfoTab = new InfoTab();
		if (!m_sharedInfoTab || !m_sharedInfoTab->init())
		{
			CC_SAFE_DELETE(m_sharedInfoTab);
		}

	}

	return m_sharedInfoTab;
}

void InfoTab::purgeSharedInfoTab(){
	CC_SAFE_RELEASE_NULL(m_sharedInfoTab);
}

bool InfoTab::init(){
	m_bIsSimple = false;			//姑且保护一下。
	CCLOG(">Init for SystemInfoTab.");
	return true;
}

void InfoTab::showinfo(string s){
	setTouchEnabled(true);
	m_iType = 1;
	m_bIsSimple = true;

	float m_height = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	BYLayerDescendant *mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0.5,0.5));
	mb->setPosition(ccp(vs.width/2,vs.height/2));


	float m_textwidth = 300;

	CCLabelTTF *m_Text = CCLabelTTF::create(s.c_str(), FNT_UI_LABEL, 18, CCSize(m_textwidth, 0), kCCTextAlignmentLeft);
	m_Text->setAnchorPoint(ccp(0.5,0));
	m_Text->setPosition(ccp(0,m_height+5));
	m_height += m_Text->getContentSize().height;
	mb->addChild(m_Text);

	this->addChild(mb);

	CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
	nback->setContentSize(CCSize(m_textwidth+ 33,m_height+ 12));
	nback->setAnchorPoint(ccp(0.5,0));
	nback->setOpacity(100);
	mb->addChild(nback,-1);

	GameManager::sharedGameManager()->showinfo();

	
}

void InfoTab::Add_Button(const char* name,int tag){
	string s = ConfigManager::sharedConfigManager()->GetConfigS(name);
	Container* tabc = new Container();
	//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));

	tabc->initString(s.c_str(),100,30,"Images/button",this,menu_selector(InfoTab::selectrediret));
	tabc->setTag(tag);
	tabc->setAnchorPoint(ccp(0.5,0));
	tabc->setPosition(ccp(m_fWidth,m_fHeight));
	mb->addChild(tabc);
	//m_vBtns.push_back(tabc);
}

/* <显示弹出选项 */
void InfoTab::showselect(string s, vector<string> sellis, CCObject* target, SEL_MenuHandler selector){
	setTouchEnabled(true);
	m_bIsSimple = false;
	m_iType = 2;

	m_fHeight = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	mb = new BYLayerDescendant();
	mb->autorelease();

	mb->setAnchorPoint(ccp(0.5,0.5));
	mb->setPosition(ccp(vs.width/2,vs.height/2));
	//////////////////////////////////////////////////////////////////////////
	m_fHeight = 5;
	m_fWidth = -105;

	int i = 1;
	for(vector<string>::iterator it = sellis.begin(); it != sellis.end(); ++it){
		Add_Button(it->c_str(),i);
		++i;
		m_fWidth += 105;
	}

	m_fHeight += 35;
	//////////////////////////////////////////////////////////////////////////
	float m_textwidth = 300;
	CCLabelTTF *m_Text = CCLabelTTF::create(ConfigManager::sharedConfigManager()->GetConfigS(s.c_str()).c_str(), FNT_UI_LABEL, 18, CCSize(m_textwidth, 0), kCCTextAlignmentLeft);
	m_Text->setAnchorPoint(ccp(0.5,0));
	m_Text->setPosition(ccp(0,m_fHeight+5));
	m_fHeight += m_Text->getContentSize().height;
	mb->addChild(m_Text);

	this->addChild(mb);

	CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
	nback->setContentSize(CCSize(m_textwidth+ 33,m_fHeight+ 12));
	nback->setAnchorPoint(ccp(0.5,0));
	nback->setOpacity(100);
	mb->addChild(nback,-1);

	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;

	GameManager::sharedGameManager()->showinfo();
}

bool InfoTab::byTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
	if(m_bIsSimple) handletouch(pTouch);					//如果有需要的话可以判断点击是否在整个控件的范围内
	BYLayerAncestor::byTouchBegan(pTouch,pEvent);
	
	return true;
}

bool InfoTab::handletouch(CCTouch* pt){
	do 
	{
		removeFromParent();
		purgeSharedInfoTab();
		GameManager::sharedGameManager()->dismissinifo();
		return true;
	} while (0);
	return false;
}

void InfoTab::selectrediret(CCObject* pSender){
	CCLOG(">Redirecting for InfoTab. Block this to off if needed..");			//内部按钮转接至出口函数并dismiss
	switch(m_iType){
	case(4):{
			Container* t_ct = ((Container*) pSender);
			if(t_ct->getTag()>0) t_ct->setTag(m_icount);
			break;
		}
	}
	activate(pSender);
	handletouch(NULL);
}

void InfoTab::activate(CCObject* pSender){
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(pSender);
		}
	}
}

/* <显示提示文字 */
void InfoTab::showbmfl(string s,CCObject* target, SEL_MenuHandler selector,string fls){
	setTouchEnabled(true);
	m_bIsSimple = false;
	m_iType = 5;
	//m_icount = sum;

	m_fHeight = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0.5,0.5));
	mb->setPosition(ccp(vs.width/2,vs.height/2));
	//////////////////////////////////////////////////////////////////////////
	m_fHeight = 5;
	m_fWidth = -55;

	Add_Button("yes",1);
	//m_fWidth += 105;
	//Add_Button("no",0);


	m_fHeight += 35;
	//////////////////////////////////////////////////////////////////////////	

	CCLabelBMFont* lbf = CCLabelBMFont::create(fls.c_str(),"fonts/CocoTd.fnt",300);
	lbf->setAnchorPoint(CCPointZero);
	lbf->setPosition(ccp(-150,m_fHeight));
	lbf->setColor(ccYELLOW);
	mb->addChild(lbf);

	m_fHeight += lbf->getContentSize().height;

	//////////////////////////////////////////////////////////////////////////
	float m_textwidth = 300;
	CCLabelTTF *m_Text = CCLabelTTF::create(ConfigManager::sharedConfigManager()->GetConfigS(s.c_str()).c_str(), FNT_UI_LABEL, 18, CCSize(m_textwidth, 0), kCCTextAlignmentLeft);
	m_Text->setAnchorPoint(ccp(0.5,0));
	m_Text->setPosition(ccp(0,m_fHeight+5));
	m_fHeight += m_Text->getContentSize().height;
	mb->addChild(m_Text);

	this->addChild(mb);

	CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
	nback->setContentSize(CCSize(m_textwidth+ 33,m_fHeight+ 12));
	nback->setAnchorPoint(ccp(0.5,0));
	nback->setOpacity(100);
	mb->addChild(nback,-1);

	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;

	GameManager::sharedGameManager()->showinfo();
}

/* <Stepper个数统计 */
void InfoTab::showsteper(string s, CCObject* target, SEL_MenuHandler selector,int sum /* = 0 */)
{
	setTouchEnabled(true);
	m_bIsSimple = false;
	m_iType = 4;
	m_icount = sum;

	m_fHeight = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0.5,0.5));
	mb->setPosition(ccp(vs.width/2,vs.height/2));
	//////////////////////////////////////////////////////////////////////////
	m_fHeight = 5;
	m_fWidth = -105;

		Add_Button("yes",1);
		m_fWidth += 105;
		Add_Button("no",0);


	m_fHeight += 35;
	//////////////////////////////////////////////////////////////////////////	
	m_bmlable = CCLabelBMFont::create(CCString::createWithFormat("%d",m_icount)->getCString(),"fonts/CocoTd.fnt");
	m_bmlable->setAnchorPoint(ccp(1,0));
	m_bmlable->setPosition(ccp (m_fWidth, m_fHeight));
	m_bmlable->setScale(1.5);
	mb->addChild(m_bmlable);

	CCSprite *minusSprite       = CCSprite::create("extensions/stepper-minus.png");
	CCSprite *plusSprite        = CCSprite::create("extensions/stepper-plus.png");

	mControlStepper *stepper   = new mControlStepper();
	stepper->initWithMinusSpriteAndPlusSprite(minusSprite,plusSprite);
	stepper->setAnchorPoint(CCPointZero);
	stepper->setPosition(ccp (m_fWidth, m_fHeight));
	stepper->setValue(m_icount);
	stepper->addTargetWithActionForControlEvents(this, cccontrol_selector(InfoTab::stepchanged), CCControlEventValueChanged);
	stepper->setMinimumValue(0);
	stepper->setMaximumValue(sum);
	mb->addChild(stepper);



	m_fHeight += stepper->getContentSize().height;

	//////////////////////////////////////////////////////////////////////////
	float m_textwidth = 300;
	CCLabelTTF *m_Text = CCLabelTTF::create(ConfigManager::sharedConfigManager()->GetConfigS(s.c_str()).c_str(), FNT_UI_LABEL, 18, CCSize(m_textwidth, 0), kCCTextAlignmentLeft);
	m_Text->setAnchorPoint(ccp(0.5,0));
	m_Text->setPosition(ccp(0,m_fHeight+5));
	m_fHeight += m_Text->getContentSize().height;
	mb->addChild(m_Text);

	this->addChild(mb);

	CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
	nback->setContentSize(CCSize(m_textwidth+ 33,m_fHeight+ 12));
	nback->setAnchorPoint(ccp(0.5,0));
	nback->setOpacity(100);
	mb->addChild(nback,-1);

	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;

	GameManager::sharedGameManager()->showinfo();
}

void InfoTab::stepchanged( CCObject *sender, CCControlEvent controlEvent )
{
	CCControlStepper* pControl = (CCControlStepper*)sender;
	m_icount = pControl->getValue();
	m_bmlable->setString(CCString::createWithFormat("%d",m_icount)->getCString());
	CCLOG(">DF:%d",m_icount );
	// Change value of label.
	//m_pDisplayValueLabel->setString(CCString::createWithFormat("%0.02f", (float)pControl->getValue())->getCString());	
}

/* <显示一个列表 */
void InfoTab::showldb( string s, CCObject* target, SEL_MenuHandler selector, string sql, CCDictionary* a_icd, int type /*= 0*/ )
{
	setTouchEnabled(true);
	m_bIsSimple = false;
	m_iType = 3;
	//m_icount = sum;

	m_fHeight = 0;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0.5,0.5));
	mb->setPosition(ccp(vs.width/2,vs.height/2));
	//////////////////////////////////////////////////////////////////////////
	m_fHeight = 5;
	m_fWidth = -55;

	Add_Button("yes",1);
	//m_fWidth += 105;
	//Add_Button("no",0);


	m_fHeight += 35;
	//////////////////////////////////////////////////////////////////////////	
	
	ListDBView* ldb = new ListDBView(300,88, sql.c_str(),a_icd, this,menu_selector(InfoTab::eatout));
	if(ldb->init()){
		ldb->setAnchorPoint(ccp(0,0));
		ldb->setPosition(ccp(-150,m_fHeight));
		ldb->setContentSize(CCSizeMake(300,88));
		ldb->autorelease();
		mb->addChild(ldb);
	}else{
		CC_SAFE_RELEASE_NULL(ldb);
	}


	m_fHeight += 88;
	//////////////////////////////////////////////////////////////////////////
	float m_textwidth = 300;
	CCLabelTTF *m_Text = CCLabelTTF::create(ConfigManager::sharedConfigManager()->GetConfigS(s.c_str()).c_str(), FNT_UI_LABEL, 18, CCSize(m_textwidth, 0), kCCTextAlignmentLeft);
	m_Text->setAnchorPoint(ccp(0.5,0));
	m_Text->setPosition(ccp(0,m_fHeight+5));
	m_fHeight += m_Text->getContentSize().height;
	mb->addChild(m_Text);

	this->addChild(mb);

	CCScale9Sprite* nback = CCScale9Sprite::create(s_Button); 
	nback->setContentSize(CCSize(m_textwidth+ 33,m_fHeight+ 12));
	nback->setAnchorPoint(ccp(0.5,0));
	nback->setOpacity(100);
	mb->addChild(nback,-1);

	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;

	GameManager::sharedGameManager()->showinfo();
}

void InfoTab::eatout( CCObject* pSender )
{
	return;
}

/* <TO角色信息表 */
void InfoTab::showcharasav( int a_iId,CCObject* target, SEL_MenuHandler selector )
{
	setTouchEnabled(true);
	m_bIsSimple = false;
	m_iType = 10;

	CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
	mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(CCPointZero);
	mb->setPosition(CCPointZero);
	
	//////////////////////////////////////////////////////////////////////////
	TOChara* m_toc = new TOChara(a_iId,this,menu_selector(InfoTab::selectrediret));
	m_toc->setAnchorPoint(CCPointZero);
	m_toc->setPosition(CCPointZero);
	mb->addChild(m_toc);


	//////////////////////////////////////////////////////////////////////////

	this->addChild(mb);

	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;

	GameManager::sharedGameManager()->showinfo();
}

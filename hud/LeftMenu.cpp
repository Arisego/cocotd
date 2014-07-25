#include "hud/LeftMenu.h"

#include "packui/ConfigManager.h"
#include "Macros.h"

//////////////////////////////////////////////////////////////////////////
// <LFBtn
#define LFBtnWidth  136
#define LFBtnHeight	25

LFBtn::LFBtn(const char* at)
{
	spriteNormal	= NULL;

	spriteSelected	= NULL;

	spriteDisable	= NULL;

	spriteHover	= CCSprite::create("Images/icon_select.png");
	spriteHover->setAnchorPoint(CCPointZero);
	spriteHover->setPosition(ccp(0,0));
	spriteHover->setVisible(false);
	addChild(spriteHover);

	labelAtlas = CCLabelBMFont::create(at, FNT_CHN);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setScale(1.3);
	labelAtlas->setPosition(ccp(30,0));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);
	
	//labelAtlas->setVertexZ(true);

	m_obContentSize.width = LFBtnWidth;
	m_obContentSize.height = LFBtnHeight;

	m_bIsEnabled = true;
	scheduleUpdate();
}

void LFBtn::ccTouchEnded( CCTouch *pTouch, CCEvent *pEvent )
{
	if(!isVisible()) return;
	//if(checkTouch(pTouch)){
		onSelect();activate();	

	//}
	return;
}




//////////////////////////////////////////////////////////////////////////
// <Left Menu
#define NUM_BTN 11

void LeftMenu::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

void LeftMenu::activate(CCObject* pSender)
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(pSender);
		}
	}
}

bool LeftMenu::f_init()
{
	do{
		bitLock = 0;					//If you do not change this, nothing will be shown in battle menu.
		m_bIsEnabled = false;
		return true;
	}while(0);
	return false;
}

void LeftMenu::Add_Button(const char* name,int tag){
	string s = ConfigManager::sharedConfigManager()->GetConfigS(name);
	LFBtn* tabc = new LFBtn(s.c_str());
	//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));

	tabc->setactivator(this,menu_selector(LeftMenu::buttonback));
	tabc->setTag(tag);
	tabc->setAnchorPoint(ccp(0,0));
	tabc->setPosition(ccp(m_fWidth,m_fHeight));
	addChild(tabc);
	m_vBtns.push_back(tabc);
}

void LeftMenu::dismiss()
{
	bitLock = 0;					
	m_bIsEnabled = false;
	removeAllChildren();
//	removeFromParent();
}

void LeftMenu::show()
{
	m_bIsEnabled = true;
	setTouchEnabled(true);
	removeAllChildren();
	m_vBtns.clear();


	m_fHeight = 400;
	m_fWidth =	20;

	int tiFlag = 1;
	for(int i = 1; i <= NUM_BTN; ++i){
		if(bitLock&tiFlag){
			Add_Button((CCString::createWithFormat("lefbtn_%d",i))->getCString(),tiFlag);
			m_fHeight -= LFBtnHeight;
			m_fHeight -= 6;
		}
		tiFlag = tiFlag<<1;
	}

	Refresh_Button();

	CCSprite* sback = CCSprite::create("Images/menu_main_base.png"); 
	sback->setContentSize(CCSize(105,m_fHeight+5));
	sback->setAnchorPoint(ccp(0,0));
	sback->setPosition(ccp(-26,-25));
	//sback->setOpacity(199);
	addChild(sback,-1);

	//CCSprite* nback = CCSprite::create("Images/hud_menu_base.png"); 
	//nback->setContentSize(CCSize(105,m_fHeight+5));
	//nback->setAnchorPoint(ccp(0,0));
	//nback->setPosition(ccp(-26,-25));
	////nback->setOpacity(199);
	//addChild(nback,3);
	m_obContentSize.width = 200;
	m_obContentSize.height =600;
}

void LeftMenu::Refresh_Button(){
	std::vector<LFBtn*>::iterator it;
	for(it = m_vBtns.begin(); it != m_vBtns.end(); ++it){
		if((*it)->getTag() != miFlag) (*it)->onNormal();
		else (*it)->onSelect();
	}
}

void LeftMenu::buttonback( CCObject* pSender )
{
	CCLog(">Redirect:%d",((Container*) pSender)->getTag());
	miFlag = ((Container*) pSender)->getTag();
	activate(pSender);
	Refresh_Button();

}

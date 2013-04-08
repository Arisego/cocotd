#include "BattleMenu.h"
#include "packui/ConfigManager.h"

#define NUM_BTN 6

void BattleMenu::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

void BattleMenu::activate(CCObject* pSender)
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(pSender);
		}
	}
}

bool BattleMenu::f_init()
{
	do{
		bitLock = 0;					//If you do not change this, nothing will be shown in battle menu.
		m_bIsEnabled = false;
		return true;
	}while(0);
	return false;
}

void BattleMenu::Add_Button(const char* name,int tag){
	string s = ConfigManager::sharedConfigManager()->GetConfigS(name);
	Container* tabc = new Container();
	//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));

	tabc->initString(s.c_str(),100,30,"Images/button",this,menu_selector(BattleMenu::buttonback));
	tabc->setTag(tag);
	tabc->setAnchorPoint(ccp(0,0));
	tabc->setPosition(ccp(m_fWidth,m_fHeight));
	addChild(tabc);
	m_vBtns.push_back(tabc);
}

void BattleMenu::dismiss()
{
	bitLock = 0;					
	m_bIsEnabled = false;
	removeAllChildren();
//	removeFromParent();
}

void BattleMenu::show()
{
	m_bIsEnabled = true;
	setTouchEnabled(true);
	removeAllChildren();
	m_vBtns.clear();


	m_fHeight = 1;
	m_fWidth =	1;

	int tiFlag = 1;
	for(int i = NUM_BTN; i >= 1; --i){
		if(bitLock&tiFlag){
			Add_Button((CCString::createWithFormat("bmbtn_%d",i))->getCString(),tiFlag);
			m_fHeight += 30;
		}
		tiFlag = tiFlag<<1;
	}

	Refresh_Button();

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/config_back.png"); 
	nback->setContentSize(CCSize(105,m_fHeight+5));
	nback->setAnchorPoint(ccp(0,0));
	nback->setOpacity(199);
	addChild(nback,-1);

}

void BattleMenu::Refresh_Button(){
	std::vector<Container*>::iterator it;
	for(it = m_vBtns.begin(); it != m_vBtns.end(); ++it){
		if((*it)->getTag() != miFlag) (*it)->onNormal();
		else (*it)->onSelect();
	}
}

void BattleMenu::buttonback( CCObject* pSender )
{
	CCLOG(">Redirect:%d",((Container*) pSender)->getTag());
	activate(pSender);
}

#include "CharaListView.h"
#include "packui/CharaS.h"
#include "packui/ConfigManager.h"
#include "sublayer/InfoTab.h"

#include "Macros.h"

#define ANIMATE_TIME_VAL 0.1

CharaListView::~CharaListView(){
	CC_SAFE_RELEASE_NULL(charalist);
}

CharaListView::CharaListView(){
	m_iMiniType = 0;
}

bool CharaListView::init(){
	do 
	{
		g_charas = CharaS::sharedCharaS();
		m_iNums = g_charas->m_iNumber;
		m_iBegin = 0;
		m_iCurs = -1;

		charalist = CCArray::createWithCapacity(m_iNums);
		charalist->retain();
		
		float t_y;
		if(m_iCLVState>0) t_y = 450;
		else {
			t_y = 140;
			Container* t_cbBack = new Container();
			t_cbBack->initString(ConfigManager::sharedConfigManager()->GetConfigS("back").c_str(),100,20,"Images/Button",this,menu_selector(CharaListView::activate));
			t_cbBack->setAnchorPoint(CCPointZero);
			t_cbBack->setPosition(ccp(140,t_y));
			t_cbBack->setTag(-9);
			addChild(t_cbBack);


			
			if(m_iMiniType == 1){
				m_ic = CCLabelBMFont::create(CCString::createWithFormat("%d",m_iItemCount)->getCString(),FNT_CHN);
				m_ic->setAnchorPoint(CCPointZero);
				m_ic->setPosition(240,t_y);
				addChild(m_ic);
			}

		}

		t_cbLeft = new Container();
		t_cbLeft->initString("[",20,20,"Images/Button",this,menu_selector(CharaListView::lrBack));
		t_cbLeft->setAnchorPoint(CCPointZero);
		t_cbLeft->setPosition(ccp(0,t_y));
		t_cbLeft->setTag(0);
		addChild(t_cbLeft);
		//lrbtns.push_back(t_cbLeft);

		t_cbRight = new Container();
		t_cbRight->initString("]",20,20,"Images/Button",this,menu_selector(CharaListView::lrBack));
		t_cbRight->setAnchorPoint(CCPointZero);
		t_cbRight->setPosition(ccp(573,t_y));
		t_cbRight->setTag(1);
		addChild(t_cbRight);
		//lrbtns.push_back(t_cbRight);


		int i = 0;
		for(vector<int>::iterator it = g_charas->m_viCharas.begin(); it != g_charas->m_viCharas.end(); ++it){
			CharaState* t_cs = new CharaState();
			t_cs->setcontent((Chara*) g_charas->m_caCharas->objectForKey(*it));
			charalist->insertObject(t_cs,i);

			t_cs->setactivator(this,menu_selector(CharaListView::CharasCBack));
			t_cs->setAnchorPoint(CCPointZero);
			addChild(t_cs);
			t_cs->setTag(i);

			++i;
		}

		return true;
	} while (0);
	return false;
}

void CharaListView::setbegin(int a_iBe){
	m_iBegin = a_iBe;
	RefreshList();
}

void CharaListView::RefreshList(){
	if(abs(m_iCLVState) != 1) return;
	//m_iCLVState = 1;

	float t_y = 1;
	float t_x = 0;
	for(int i = 0; i< m_iNums; ++i){
		CharaState* t_cs = (CharaState*) charalist->objectAtIndex(i);
		if(i < m_iBegin || i> m_iBegin + 3) {
			t_cs->setVisible(false);
			continue;
		}
		CCLOG(">Refresh for:%d",i);
		if(m_iCLVState<0) t_cs->show_mini();
		else t_cs->show_long();
		t_cs->setPosition(ccp(t_x,t_y));
		t_cs->setVisible(true);
		t_cs->setScale(1);
		t_cs->settouchstate(true);
		t_cs->RefreshValues();
		t_x += 150;

	}
	RefreshLR();
}

void CharaListView::RefreshListValue(){
	for(int i = m_iBegin; i< m_iBegin + 4; ++i){
		CharaState* t_cs = (CharaState*) charalist->objectAtIndex(i);
		t_cs->RefreshValues();

	}
}

void CharaListView::CharasCBack( CCObject* pSender )
{
	int t_iTag = ((CharaState*) pSender)->getTag();
	switch(m_iCLVState){
	case (-1):
		{
			if(m_iMiniType == 1 ){
				
				if(m_iItemCount == 0) {
					break;
				}
				else{
					--m_iItemCount;
					m_ic->setCString(CCString::createWithFormat("Left Item:%d",m_iItemCount)->getCString());
				}
			}
			m_tcsTarget = (CharaState*) pSender;
			activate(pSender);
			break;
		}
	case(1):
		{
			m_iCurs = t_iTag;
#if (CHARA_TYPE == CHARA_TD_TYPE)
			PrepareSingle();
			m_iCLVState = 2;
			RefreshLR();
#elif (CHARA_TYPE == CHARA_TO_TYPE)			//TO<弹出属性界面
			InfoTab::sharedInfoTab()->showcharasav(m_iCurs,this,menu_selector(CharaListView::PopupBack));
#endif
			break;
		}
	case(2):
		{
			m_iCLVState = 1;
			RefreshList();
			
			break;
		}
	}
}

void CharaListView::RefreshLR(){
	t_cbLeft->setVisible(abs(m_iCLVState) == 1);
	t_cbRight->setVisible(abs(m_iCLVState) == 1);

	t_cbLeft->setEnability(m_iBegin > 0);
	t_cbRight->setEnability(m_iBegin + 4 < m_iNums);
}

void CharaListView::PrepareBack( CCObject* pSender )
{
	((CharaState*) pSender)->show_all();
	//((CharaState*) pSender)->settouchstate(true);
}

void CharaListView::PrepareSingle()
{
	for(int i = m_iBegin; i< (m_iBegin + 4); ++i){
		CharaState* t_cs = (CharaState*) charalist->objectAtIndex(i);
		t_cs->settouchstate(false);
		if(i == m_iCurs){
			t_cs->runAction(CCSequence::createWithTwoActions(CCMoveTo::create(ANIMATE_TIME_VAL,ccp(0,1)),CCCallFuncO::actionWithTarget(this, callfuncO_selector(CharaListView::PrepareBack),t_cs)));
		}else{
			t_cs->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCFadeOut::create(ANIMATE_TIME_VAL),CCScaleTo::create(ANIMATE_TIME_VAL,0.8)),CCSpawn::createWithTwoActions(CCToggleVisibility::create(),CCScaleTo::create(0.0,1.25))));
		}
	}
}

void CharaListView::lrBack( CCObject* pSender )
{
	int t_iTag = ((Container*) pSender)->getTag();

	if(t_iTag>0){
		m_iBegin++;
	}else{
		m_iBegin = m_iBegin-1;
	}

	RefreshList();

}

void CharaListView::activate(CCObject* pSender)
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(pSender);
		}
	}
}

void CharaListView::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

void CharaListView::showtargettext(string s, int type){
	float t_x = m_tcsTarget->getPositionX() + 20;
	m_tcsTarget->RefreshValues();

	removeChildByTag(0x299);

	CCLabelBMFont* c_ttlbmf = CCLabelBMFont::create(s.c_str(),FNT_CHN);
	c_ttlbmf->setAnchorPoint(CCPointZero);
	c_ttlbmf->setPosition(ccp(t_x ,10));
	c_ttlbmf->setTag(0x299);
	addChild(c_ttlbmf,11);

	c_ttlbmf->runAction(CCSpawn::createWithTwoActions(CCMoveTo::create(0.3,ccp(t_x, 100)),CCFadeOut::create(0.3)));
}

void CharaListView::seticc( int i )
{
	m_iItemCount = i;
}

void CharaListView::PopupBack( CCObject* pSender )
{	
	setbegin(((CCNode*) pSender)->getTag());
	RefreshList();
}

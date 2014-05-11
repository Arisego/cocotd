#include "packui/TOPopup.h"
#include "StateCenter.h"
#include "byeven/BYCocos.h"
#include "packui/ConfigManager.h"


#define CONFIRM -3
#define PACKDOW -1
#define CANCELL -2

TOPopup::TOPopup(int width, int height)
{
	m_ldbEquList = NULL;
	m_ldbSkilList = NULL;
	miFlag		 = -3;

	miWidth		=	width;
	miHeight	=	height;

	mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0,0));
	mb->setPosition(ccp(0,1));
	mb->setContentSize(CCSizeMake(miWidth,miHeight));
	addChild(mb,-1);

	mTB_confirm = new TOButton(80,20);
	mTB_confirm->setactivator(this,menu_selector(TOPopup::ConfirmBack));
	mTB_confirm->f_setstring(ConfigManager::sharedConfigManager()->GetConfigS("change").c_str());
	mTB_confirm->setAnchorPoint(ccp(0,0));
	mTB_confirm->setPosition(miWidth-81,1);
	mTB_confirm->setTag(CONFIRM);
	mTB_confirm->setEnability(false);
	mb->addChild(mTB_confirm,1);

	tTB_packdown = new TOButton(miWidth,20);
	tTB_packdown->f_setstring(ConfigManager::sharedConfigManager()->GetConfigS("pack_down").c_str());
	tTB_packdown->setactivator(this,menu_selector(TOPopup::ItemBack));
	tTB_packdown->setAnchorPoint(ccp(0,0));
	tTB_packdown->setPosition(1,miHeight-21);
	tTB_packdown->setTag(PACKDOW);
	mb->addChild(tTB_packdown,1);

	mTB_cancell = new TOButton(80,20);
	mTB_cancell->setactivator(this,menu_selector(TOPopup::ConfirmBack));
	mTB_cancell->f_setstring(ConfigManager::sharedConfigManager()->GetConfigS("bmbtn_6").c_str());
	mTB_cancell->setAnchorPoint(ccp(0,0));
	mTB_cancell->setPosition(1,1);
	mTB_cancell->setTag(CANCELL);
	mb->addChild(mTB_cancell,1);

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/config_back.png"); 
	nback->setContentSize(CCSize(miWidth,miHeight));
	nback->setAnchorPoint(ccp(0,0));
	nback->setOpacity(100);
	addChild(nback,-2);
}

TOPopup::~TOPopup()
{
	CCLOG(">TOPopup Dec");
}

//////////////////////////////////////////////////////////////////////////
void TOPopup::ItemBack( CCObject* pSender )
{
	switch (miPage)
	{
	case 1:
		{
			miFlag = ((CCNode*) pSender)->getTag();
			mTB_confirm->setEnability(true);
			if(miFlag>0){
				m_sEffect = m_ldbEquList->getval("effect",miFlag);
				CCLOG(">[Effect]:%s",m_sEffect.c_str());
				tTB_packdown->onNormal();
			}else{
				m_sEffect = "";
				if(m_ldbEquList) m_ldbEquList->cellselect(-1);
			}
			break;
		}
	case 2:
		{
			miFlag = ((CCNode*) pSender)->getTag();
			mTB_confirm->setEnability(true);
			if(miFlag>0){
				tTB_packdown->onNormal();
			}else{
				if(m_ldbSkilList) m_ldbSkilList->cellselect(-1);
			}
			break;
		}
	default:
		break;
	}


}

bool TOPopup::refresh_ldb( int tag )
{
	CCLog(">[TOPopup] refresh_ldb || BUG. No Equip will cause no desplay and no way to pack down equip....");
	CCDictionary* t_caEqui	 = StateCenter::sharedStateCenter()->f_get_itemlist(4);		//Group 4 -- Equips.
	string t_sMask;


	if(t_caEqui){

		CCDictElement* pCe = NULL;


		CCDICT_FOREACH(t_caEqui,pCe){
			t_sMask +=  CCString::createWithFormat("%d,",((ItemCellData*) pCe->getObject())->type_id)->getCString();
		}
		if(t_sMask.length() > 0){
			t_sMask.erase(t_sMask.length()-1);
			CCString* t_csSql = CCString::createWithFormat("select * from equip_list where itemid IN (%s) and position = %d", t_sMask.c_str(), tag);

			if(m_ldbSkilList){m_ldbSkilList->removeFromParent();m_ldbSkilList = NULL;}
			if(m_ldbEquList) {m_ldbEquList->removeFromParent();m_ldbEquList = NULL;}
			m_ldbEquList = new ListDBView<TOEquipCell>(miWidth,miHeight - 42, t_csSql->getCString(),t_caEqui, this,menu_selector(TOPopup::ItemBack),2);
			if(m_ldbEquList->init()){
				m_ldbEquList->setAnchorPoint(ccp(0,0));
				m_ldbEquList->setPosition(ccp(1,21));
				m_ldbEquList->setContentSize(CCSizeMake(miWidth,miHeight - 42));
				m_ldbEquList->autorelease();
				addChild(m_ldbEquList);
				if(m_ldbEquList->m_iNumber == 0) mbZeroList = true;
				else mbZeroList = false;
			}else{
				CC_SAFE_RELEASE_NULL(m_ldbEquList);
				mbZeroList = false;
			}



			setVisible(true);
			setTouchEnabled(false);
			mTB_cancell->onNormal();
			tTB_packdown->onNormal();
			mTB_confirm->setEnability(false);
			mb->m_bIsEnabled = true;
			miFlag = -3;
			miPage = 1;
			return true;
		}

		// <测试|在没有装备时不显示装备列表，导致无法卸载装备|返回true测试
		setVisible(true);
		setTouchEnabled(false);
		mTB_cancell->onNormal();
		tTB_packdown->onNormal();
		mTB_confirm->setEnability(false);
		mb->m_bIsEnabled = true;
		miFlag = -3;
		miPage = 1;
		return true;
		
	}
	return false;
}

void TOPopup::ConfirmBack( CCObject* pSender )
{
	
	int t_iTag = ((CCNode*) pSender)->getTag();
	switch(t_iTag){
	case CANCELL:
		miFlag = t_iTag;
		break;
	default:
		break;
	}

	setTag(miFlag);
	CCLOG(">[TOPopup]Prepare for confirm out.");
	activate(this);
}

void TOPopup::inform( Equip* s )
{
	s->effect	=	m_ldbEquList->getval("effect",miFlag);
	s->name		=	m_ldbEquList->getval("name",miFlag);//t_ssm.at("name");
	s->discript	=	m_ldbEquList->getval("discription",miFlag);//t_ssm.at("discription");
	s->position	=	stoi(m_ldbEquList->getval("position",miFlag));//stoi(t_ssm.at("position"));
}

void TOPopup::clear()
{
	m_sEffect = "";
	//if(m_ldbEquList){
	//	m_ldbEquList->autorelease();
	//	m_ldbEquList = NULL;
	//}

	mb->m_bIsEnabled = false;
	setVisible(false);
	setTouchEnabled(false);
}

void TOPopup::w_press()
{	
	if(!m_ldbEquList && !m_ldbSkilList){
		miIdx = -1;
		tTB_packdown->activate();
		tTB_packdown->onSelect();
		return;
	}
	switch (miPage)
	{
	case 1:
		{
			miIdx = m_ldbEquList->f_get_viewid(miFlag);
			CCLOG(">w_press|%d",miIdx);

			if(miIdx<=0){
				tTB_packdown->activate();
				tTB_packdown->onSelect();
				if(miIdx == 0)
					m_ldbEquList->cellselect(-1);
			}else{
				tTB_packdown->onNormal();
				m_ldbEquList->f_swithc_id(miIdx-1,0);
			}
			break;
		}
	case 2:
		{
			miIdx = m_ldbSkilList->f_get_viewid(miFlag);
			CCLOG(">w_press|%d",miIdx);

			if(miIdx<=0){
				tTB_packdown->activate();
				tTB_packdown->onSelect();
				if(miIdx == 0)
					m_ldbSkilList->cellselect(-1);
			}else{
				tTB_packdown->onNormal();
				m_ldbSkilList->f_swithc_id(miIdx-1,0);
			}
			break;
		}
	default:
		break;
	}

}

void TOPopup::s_press()
{
	if(!m_ldbEquList && !m_ldbSkilList){
		miIdx = -1;
		tTB_packdown->activate();
		tTB_packdown->onSelect();
		return;
	}
	switch (miPage)
	{
	case 1:
		{
			miIdx = m_ldbEquList->f_get_viewid(miFlag);
			CCLOG(">s_press|%d",miIdx);
			if(miIdx<-1){
				tTB_packdown->activate();
				tTB_packdown->onSelect();
			}else{
				tTB_packdown->onNormal();
				m_ldbEquList->f_swithc_id(miIdx+1,0);
			}
			break;
		}
	case 2:
		{
			miIdx = m_ldbSkilList->f_get_viewid(miFlag);
			CCLOG(">s_press|%d",miIdx);
			if(miIdx<-1){
				tTB_packdown->activate();
				tTB_packdown->onSelect();
			}else{
				tTB_packdown->onNormal();
				m_ldbSkilList->f_swithc_id(miIdx+1,0);
			}
			break;
		}
	default:
		break;
	}


}

void TOPopup::z_press()
{
	CCLOG(">z_press:%d",miFlag);
	if(miFlag < -2) return;
	if(mbZeroList&&(miFlag != -1)) return;
	setTag(miFlag);
	activate(this);
}

void TOPopup::x_press()
{
	CCLOG(">x_press:%d",miFlag);
	setTag(CANCELL);
	activate(this);
}

bool TOPopup::refresh_sks(const char* msk, CCDictionary* mld)
{


	if(m_ldbSkilList){m_ldbSkilList->removeFromParent();m_ldbSkilList = NULL;}
	if(m_ldbEquList) {m_ldbEquList->removeFromParent();m_ldbEquList = NULL;}
	m_ldbSkilList = new ListDBView<TOSkillCell>(miWidth,miHeight - 42, msk,mld, this,menu_selector(TOPopup::ItemBack),2);
	if(m_ldbSkilList->init()){
		m_ldbSkilList->setAnchorPoint(ccp(0,0));
		m_ldbSkilList->setPosition(ccp(1,21));
		m_ldbSkilList->setContentSize(CCSizeMake(miWidth,miHeight - 42));
		m_ldbSkilList->autorelease();
		addChild(m_ldbSkilList);
		if(m_ldbSkilList->m_iNumber == 0) mbZeroList = true;
		else mbZeroList = false;

	}else{
		CC_SAFE_RELEASE_NULL(m_ldbSkilList);
		mbZeroList = false;
	}
	


	setVisible(true);
	setTouchEnabled(false);
	mTB_cancell->onNormal();
	tTB_packdown->onNormal();
	mTB_confirm->setEnability(false);
	mb->m_bIsEnabled = true;
	miPage = 2;
	miFlag = -3;
	return true;




}

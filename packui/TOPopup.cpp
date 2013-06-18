#include "packui/TOPopup.h"
#include "StateCenter.h"
#include "byeven/BYCocos.h"
#include "packui/ConfigManager.h"

#define CONFIRM -3
#define PACKDOW -4

TOPopup::TOPopup(int width, int height)
{
	m_ldbEquList = NULL;
	miFlag		 = 0;

	miWidth		=	width;
	miHeight	=	height;

	BYLayerDescendant* mb = new BYLayerDescendant();
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

	TOButton* tTB_packdown = new TOButton(miWidth,20);
	tTB_packdown->f_setstring(ConfigManager::sharedConfigManager()->GetConfigS("pack_down").c_str());
	tTB_packdown->setactivator(this,menu_selector(TOPopup::ItemBack));
	tTB_packdown->setAnchorPoint(ccp(0,0));
	tTB_packdown->setPosition(1,miHeight-21);
	tTB_packdown->setTag(PACKDOW);
	mb->addChild(tTB_packdown,1);

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/config_back.png"); 
	nback->setContentSize(CCSize(miWidth,miHeight));
	nback->setAnchorPoint(ccp(0,0));
	nback->setOpacity(100);
	addChild(nback,-2);
}

TOPopup::~TOPopup()
{
	;
}

//////////////////////////////////////////////////////////////////////////
void TOPopup::ItemBack( CCObject* pSender )
{
	miFlag = ((CCNode*) pSender)->getTag();
	mTB_confirm->setEnability(true);
	if(miFlag>0){
		m_sEffect = m_ldbEquList->getval("effect",miFlag);
		CCLOG(">[Effect]:%s",m_sEffect.c_str());
	}else{
		m_sEffect = "";
	}
}

void TOPopup::refresh_ldb( int tag )
{
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

			CC_SAFE_RELEASE_NULL(m_ldbEquList);
			m_ldbEquList = new ListDBView<TOEquipCell>(miWidth,miHeight - 42, t_csSql->getCString(),t_caEqui, this,menu_selector(TOPopup::ItemBack),2);
			if(m_ldbEquList->init()){
				m_ldbEquList->setAnchorPoint(ccp(0,0));
				m_ldbEquList->setPosition(ccp(1,21));
				m_ldbEquList->setContentSize(CCSizeMake(miWidth,miHeight - 42));
				addChild(m_ldbEquList);
				m_ldbEquList->autorelease();


			}else{
				CC_SAFE_RELEASE_NULL(m_ldbEquList);
			}

		}
	}
}

void TOPopup::ConfirmBack( CCObject* pSender )
{
	CC_UNUSED_PARAM(pSender);
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

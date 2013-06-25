#include "ItemPicker.h"
#include "StateCenter.h"
#include "DBUtil.h"
#include "EventCenter.h"

#define IP_WIDTH 99
#define IP_HEITH 132

ItemPicker::~ItemPicker(){
	CCLOG(">ItemPicker Desc.");
}

bool ItemPicker::refresh_itemlist(){
	do 
	{

		m_iCurItem = -1;
		m_iSum = 0;
		m_viItemList.clear();
		m_cdItemList = StateCenter::sharedStateCenter()->f_get_itemlist(2);			//group = 2 || WalkMap Item | Usecase = 0b010
		CC_BREAK_IF(m_cdItemList->count() < 1);

		m_miiId2Idx.clear();
		//int i = 0;
		string t_sMask;
		CCDictElement* pCe = NULL;
		ItemCellData* ticd = NULL;
		CCDICT_FOREACH(m_cdItemList,pCe){
			ticd = (ItemCellData*) pCe->getObject();
			t_sMask += CCString::createWithFormat("%d,",ticd->type_id)->getCString();
			m_miiId2Idx[ticd->type_id] = pCe->getIntKey();
			//m_viItemList.push_back((ItemCellData*) pCe->getObject());
		}

		t_sMask.erase(t_sMask.length()-1);
		CCString* t_csSql = CCString::createWithFormat("select * from item_list where itemid IN (%s) and groupid = 2",t_sMask.c_str());
		DBUtil::initDB("save.db");
		vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
		DBUtil::closeDB(); 
		m_iSum = vdata.size();
		CC_BREAK_IF(vdata.size() < 1);

		for(int i = 0;i<m_iSum;i++){
			map<string,string> t_ssm = (map<string,string>) vdata.at(i);
			int item_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
			ItemCellData* t_icd = (ItemCellData*) m_cdItemList->objectForKey(m_miiId2Idx[item_id]);

			t_icd->name			=	t_ssm.at("name");
			t_icd->icon_mask	=	t_ssm.at("icon");

			if(item_id == m_iCItemID) m_iCurItem = i;
			CCLOG(">Read for group 2 with itemid:%d.", item_id);
			m_viItemList.push_back(t_icd);
		}
		if(m_iCurItem<0) m_iCurItem = 0;
		return true;
	} while (0);
return false;
}

bool ItemPicker::init()
{
	do{
		m_bIsEnabled = false;
		CCScale9Sprite* nback = CCScale9Sprite::create("Images/life_bar1.png"); 
		nback->setAnchorPoint(ccp(0,0));
		nback->setContentSize(CCSize(IP_WIDTH,IP_HEITH));
		nback->setOpacity(30);
		nback->setPosition(ccp(3,1));
		addChild(nback,-1);
		m_iCItemID = -1;

		m_bcItem = new Container();
		m_bcItem->initwithPre("Images/button",IP_WIDTH,IP_HEITH);
		m_bcItem->setstringnull();
		m_bcItem->setTag(-1);
		m_bcItem->setAnchorPoint(ccp(0,0));
		m_bcItem->setPosition(ccp(10,1));
		m_bcItem->setOpacity(99);
		m_bcItem->setactivator(this,menu_selector(ItemPicker::ItemSelect));

		m_bcItem->labelAtlas->setAnchorPoint(ccp(1,0));
		m_bcItem->labelAtlas->setColor(ccORANGE);
		m_bcItem->labelAtlas->setVisible(true);
		m_bcItem->labelAtlas->setPosition(ccp(IP_WIDTH - 1,0));

		addChild(m_bcItem);
		CC_BREAK_IF(!refresh_itemlist());



		//m_iCurItem = 0;
		show_item();
		return true;
	}while(0);
	return false;
}

void ItemPicker::ItemSelect(CCObject* pSender){
	CCLOG(">ItemPick || ItemSelect");
}

void ItemPicker::show_item(){
	// if(m_iCurItem > m_iSum) exit(0x5006);
	m_bcItem->removeChildByTag(0x99);
	m_bcItem->removeChildByTag(0x9a);
	if(m_iSum == 0) {
		m_bcItem->labelAtlas->setCString("NO ITEM");
		return;
	}
	if(m_iCurItem < 0) return;
	


	ItemCellData* a_icd = m_viItemList.at(m_iCurItem);
	
	CCSprite* t_cs = CCSprite::create(CCString::createWithFormat("Images/bigicon_%s.png",a_icd->icon_mask.c_str())->getCString());
	t_cs->setAnchorPoint(ccp(0,0));
	t_cs->setPosition(ccp(1,1));
	t_cs->setTag(0x99);
	m_bcItem->addChild(t_cs);


	CCLabelTTF* t_ct = CCLabelTTF::create(a_icd->name.c_str(),FNT_UI_LABEL, 18,CCSize(IP_WIDTH,0), kCCTextAlignmentLeft);
	t_ct->setAnchorPoint(ccp(0,0));
	t_ct->setPosition(ccp(1,21));
	t_ct->setTag(0x9a);
	t_ct->setColor(ccYELLOW);
	m_bcItem->addChild(t_ct);

	m_bcItem->labelAtlas->setCString((CCString::createWithFormat("Left:%d",a_icd->sum)->getCString()));


}

void ItemPicker::onShow()
{
	setVisible(true);
	if(m_iSum == 0){
		if(!refresh_itemlist()){
			show_item();
			return;
		}
		show_item();
	}
	EventCenter::sharedEventCenter()->setSelector(this); 
	ItemCellData* a_icd = m_viItemList.at(m_iCurItem);
	if(a_icd->sum < 1){
		//m_viItemList.erase(m_viItemList.at(m_iCurItem));			//Reinit the list for further opreation such as add or....
		m_iCItemID = a_icd->type_id;
		StateCenter::sharedStateCenter()->f_get_itemlist(2)->removeObjectForKey(m_miiId2Idx[m_iCItemID]);
		
		refresh_itemlist();
		show_item();
	}else
		m_bcItem->labelAtlas->setCString((CCString::createWithFormat("Left:%d",a_icd->sum)->getCString()));
}

void ItemPicker::onDismiss()
{
	setVisible(false);
	if(m_iSum == 0) return;
	EventCenter::sharedEventCenter()->setSelector();
}

void ItemPicker::q_press()
{
	--m_iCurItem;
	if(m_iCurItem < 0) m_iCurItem = m_iSum-1;
	show_item();
}

void ItemPicker::e_press()
{
	++m_iCurItem;
	if(m_iCurItem >= m_iSum) m_iCurItem = 0;
	show_item();
}

void ItemPicker::c_press()
{
	if(m_iSum == 0) return;
	CCLOG(">Use Item in case 2:%d",m_viItemList[m_iCurItem]->type_id);
	activate(m_viItemList[m_iCurItem]);
}

void ItemPicker::setactivator( CCObject* target, SEL_MenuHandler selector )
{
	m_pListener = target;
	m_pfnSelector = selector;
	m_bIsEnabled = true;
}

void ItemPicker::activate(CCObject* pSender)
{
	if (m_bIsEnabled)
	{
		if (m_pListener && m_pfnSelector)
		{
			(m_pListener->*m_pfnSelector)(pSender);
		}
	}
}


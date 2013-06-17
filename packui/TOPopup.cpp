#include "packui/TOPopup.h"
#include "StateCenter.h"
#include "byeven/BYCocos.h"

TOPopup::TOPopup(int width, int height)
{
	m_ldbEquList = NULL;

	BYLayerDescendant* mb = new BYLayerDescendant();
	mb->autorelease();
	mb->setAnchorPoint(ccp(0,0));
	mb->setPosition(ccp(0,1));
	mb->setContentSize(CCSizeMake(500,400));
	addChild(mb,-1);

}

TOPopup::~TOPopup()
{
	;
}

//////////////////////////////////////////////////////////////////////////
void TOPopup::ItemBack( CCObject* pSender )
{
	;
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
			m_ldbEquList = new ListDBView<ItemCell>(300,250, t_csSql->getCString(),t_caEqui, this,menu_selector(TOPopup::ItemBack),2);
			if(m_ldbEquList->init()){
				m_ldbEquList->setAnchorPoint(ccp(0,1));
				m_ldbEquList->setPosition(ccp(2,0));
				m_ldbEquList->setContentSize(CCSizeMake(300,250));
				addChild(m_ldbEquList);
				m_ldbEquList->autorelease();


			}else{
				CC_SAFE_RELEASE_NULL(m_ldbEquList);
			}

		}
	}
}

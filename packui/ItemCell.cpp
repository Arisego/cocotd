#include "ItemCell.h"

ItemCell::ItemCell(float w, float h, int type, ItemCellData* a_icd)
{
	f_init(w,h,type,a_icd);
}

void ItemCell::f_init(float w, float h, int type, ItemCellData* a_icd){
	removeAllChildren();

	sum = a_icd->sum;
	lock = a_icd->lock;
	initwithPre("Images/button",w,h);
	string name = a_icd->name;
	CCLOG("Ready for item:%d,%s",a_icd->type_id,name.c_str());

	m_iType = type;

	CCSprite* t_cs = CCSprite::create(CCString::createWithFormat("Images/icon_%s.png",a_icd->icon_mask.c_str())->getCString());
	t_cs->setAnchorPoint(ccp(0,0));
	t_cs->setPosition(ccp(1,1));
	addChild(t_cs);

	CCLabelTTF* t_ct = CCLabelTTF::create(name.c_str(), FNT_UI_LABEL, 20,CCSize(w,0), kCCTextAlignmentLeft);
	t_ct->setAnchorPoint(ccp(0,0));
	t_ct->setPosition(ccp(31,1));
	addChild(t_ct);
	switch(type)
	{
	case(0):
		{
			labelAtlas = CCLabelBMFont::create(CCString::createWithFormat("%d",a_icd->sum)->getCString(), "fonts/CocoTd.fnt");
			labelAtlas->setAnchorPoint(ccp(0,0));
			labelAtlas->setPosition(ccp(w-50,1));
			addChild(labelAtlas);
			break;
		}
	case(1):
		{
			setstringnull();
			break;
		}
	case(2):
		{
			labelAtlas = CCLabelBMFont::create(CCString::createWithFormat("%d/%d",a_icd->lock,a_icd->sum)->getCString(), "fonts/CocoTd.fnt");
			labelAtlas->setAnchorPoint(ccp(0,0));
			labelAtlas->setPosition(ccp(w-50,1));

			addChild(labelAtlas);	
			if(a_icd->sum == a_icd->lock)			//All Equip is Equipped.
			{
				t_ct->setColor(ccGRAY);
				labelAtlas->setColor(ccGRAY);
			}
			break;
		}
	}

	//settouchstate(false);
}

void ItemCell::onNormal(){
	if(m_iState == C_STATE_SELECT) return;
	Container::onNormal();
}

void ItemCell::onHover(){
	if(m_iState == C_STATE_SELECT) return;
	Container::onHover();
}

void ItemCell::setNormal(){
	Container::onNormal();
}

void ItemCell::f_setsum( int i )
{
	switch(m_iType){
	case(0):
		{
			labelAtlas->setString(CCString::createWithFormat("%d",i)->getCString());
			break;
		}

	}
}

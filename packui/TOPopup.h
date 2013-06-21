#ifndef __TO_POPUP_H__
#define __TO_POPUP_H__

#include "cocos2d.h"
#include "ListDBView.h"
#include "packui/Activator.h"
#include "packui/TOSkillVA.h"

USING_NS_CC;

/*
	TOEquipCell
	<由Ldb作为列表的单元使用
*/
class TOEquipCell:public ItemCell{
private:
	CCLabelTTF* mLT_Name;
	CCLabelBMFont* mLB_NJFront;
	CCLabelBMFont* mLB_NJBack;

public:

	TOEquipCell(float w, float h, int type, ItemCellData* a_icd){
		f_init(w,h,type,a_icd);
	}

	virtual void f_init(float w, float h, int type, ItemCellData* a_icd)
	{
		removeAllChildren();
		//////////////////////////////////////////////////////////////////////////
		spriteNormal	= NULL;

		spriteHover		= NULL;

		spriteDisable	= NULL;

		spriteSelected	= CCSprite::create("Images/icon_select.png");
		spriteSelected->setAnchorPoint(CCPointZero);
		spriteSelected->setPosition(ccp(0,0));
		spriteSelected->setVisible(false);
		addChild(spriteSelected);

		m_obContentSize.width = w;
		m_obContentSize.height = h;

		m_bIsEnabled = true;
		scheduleUpdate();

		//////////////////////////////////////////////////////////////////////////

		sum = a_icd->sum;
		lock = a_icd->lock;
		string name = a_icd->name;
		m_iType = type;

		CCSprite* t_cs = CCSprite::create(CCString::createWithFormat("Images/icon_%s.png",a_icd->icon_mask.c_str())->getCString());
		t_cs->setAnchorPoint(ccp(0,0));
		t_cs->setPosition(ccp(31,1));
		addChild(t_cs);

		mLT_Name = CCLabelTTF::create(name.c_str(), FNT_UI_LABEL, 20,CCSize(w,0), kCCTextAlignmentLeft);
		mLT_Name->setAnchorPoint(ccp(0,0));
		mLT_Name->setPosition(ccp(61,1));
		mLT_Name->setColor(COLOUR_NORMAL);
		addChild(mLT_Name);

		labelAtlas = CCLabelBMFont::create("/", FNT_CHN);
		labelAtlas->setAnchorPoint(ccp(0,0));
		labelAtlas->setPosition(ccp(w-50,1));
		labelAtlas->setColor(COLOUR_NORMAL);
		addChild(labelAtlas);

		mLB_NJFront = CCLabelBMFont::create(CCString::createWithFormat("%d",a_icd->lock)->getCString(), FNT_ENG_NUM);
		mLB_NJFront->setScale(0.8);
		mLB_NJFront->setAnchorPoint(ccp(1,0));
		mLB_NJFront->setPosition(ccp(w-50,0));
		mLB_NJFront->setColor(ccc3(215,169,119));
		this->addChild(mLB_NJFront,1);

		mLB_NJBack = CCLabelBMFont::create(CCString::createWithFormat("%d",abs(a_icd->sum))->getCString(), FNT_ENG_NUM);
		mLB_NJBack->setScale(0.8);
		mLB_NJBack->setAnchorPoint(CCPointZero);
		mLB_NJBack->setPosition(ccp(w-43,0));
		mLB_NJBack->setColor(ccc3(215,169,119));
		this->addChild(mLB_NJBack,1);
	}

	virtual void onHover(){
		ItemCell::onHover();
		mLB_NJBack->setColor(COLOUR_HOVER);
		mLB_NJFront->setColor(COLOUR_HOVER);
		mLT_Name->setColor(COLOUR_HOVER);
	}

	virtual void onSelect(){
		ItemCell::onSelect();
		mLB_NJBack->setColor(COLOUR_SELECT);
		mLB_NJFront->setColor(COLOUR_SELECT);
		mLT_Name->setColor(COLOUR_SELECT);
	}

	virtual void onNormal(){
		ItemCell::onHover();
		mLB_NJBack->setColor(COLOUR_NORMAL);
		mLB_NJFront->setColor(COLOUR_NORMAL);
		mLT_Name->setColor(COLOUR_NORMAL);
	}
};

class  TOButton : public Container
{
public:
	TOButton(int width , int height){
		f_init(width,height);
	}

private:
	void f_init(int width , int height){
		initwithPre("Images/button",width,height);
	}

public:
	virtual void f_setstring(const char* s_content){
		labelAtlas = CCLabelBMFont::create(s_content, FNT_CHN);
		labelAtlas->setAnchorPoint(ccp(0.5,0.5));
		labelAtlas->setScale(0.8);
		labelAtlas->setPosition(ccp(m_obContentSize.width/2,m_obContentSize.height/2));
		labelAtlas->setVisible(true);
		labelAtlas->setColor(COLOUR_NORMAL);
		addChild(labelAtlas);
	}
};

class TOPopup:public CCLayer,public Activator
{
public:
	TOPopup(int width, int height);
	~TOPopup();
	TOButton* mTB_confirm;
	TOButton* mTB_cancell;
	TOButton* tTB_packdown;

	//For Equips
	bool refresh_ldb(int tag);
	//For Skills
	bool refresh_sks(const char* msk, CCDictionary* mld);

	void ItemBack(CCObject* pSender);
	void ConfirmBack(CCObject* pSender);
	string m_sEffect;		//Effect String for selected item.

	void inform(Equip* s);
	void clear();			//clear the states

	void w_press();
	void s_press();
	void z_press();
	void x_press();
private:
	ListDBView<TOEquipCell>* m_ldbEquList;
	ListDBView<TOSkillCell>* m_ldbSkilList;
	int miHeight,miWidth;
	int miFlag;
	int miIdx;
	BYLayerDescendant* mb;
	int miPage;
};


#endif
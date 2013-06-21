#ifndef __TO_SKILL_V_A_H__
#define __TO_SKILL_V_A_H__

#include "Container.h"
#include "ItemCell.h"

class TOSkillCell:public ItemCell{
private:
	CCLabelTTF* mLT_Name;

public:

	TOSkillCell(float w, float h, int type, ItemCellData* a_icd){
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

		labelAtlas = CCLabelBMFont::create("", FNT_CHN);
		labelAtlas->setAnchorPoint(ccp(0,0));
		labelAtlas->setPosition(ccp(w-50,1));
		labelAtlas->setColor(COLOUR_NORMAL);
		addChild(labelAtlas);
	}

	virtual void onHover(){
		ItemCell::onHover();
		mLT_Name->setColor(COLOUR_HOVER);
	}

	virtual void onSelect(){
		ItemCell::onSelect();
		mLT_Name->setColor(COLOUR_SELECT);
	}

	virtual void onNormal(){
		ItemCell::onHover();
		mLT_Name->setColor(COLOUR_NORMAL);
	}
};

/* SkillMeta */
struct SkillMeta:ItemCellData
{
	string discription;
	int zbtype;	// <1ÎªÖ÷¶¯

	SkillMeta(){};
};

class TOSkill :public Container
{
public:
	TOSkill();
	void setcontent(SkillMeta* sm);

	virtual void onNormal();
	virtual void onHover();
	virtual void onSelect();
	virtual void onDisable();

	SkillMeta* m_smData;
private:
	CCLabelTTF* mLT_Name;
	
};

class TOTama : public Container
{
	;
};

class TOVoice : public Container
{
	;
};

class TOASkill : public Container
{
public:
	TOASkill();
	void setcontent(SkillMeta* sm);

	virtual void onNormal();
	virtual void onHover();
	virtual void onSelect();
	virtual void onDisable();

	SkillMeta* m_smData;
private:
	CCLabelTTF* mLT_Name;
};

#endif
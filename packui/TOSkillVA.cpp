#include "packui/TOSkillVA.h"
#include "Macros.h"

static const char* tlevel[] = {"E","D","C","B","A","S"};

TOSkill::TOSkill()
{
	mLT_Name = CCLabelTTF::create("",FNT_UI_TTF,15);
	mLT_Name->setAnchorPoint(ccp(0,0));
	mLT_Name->setPosition(ccp(20,2));
	mLT_Name->setColor(COLOUR_NORMAL);
	addChild(mLT_Name,1);

	spriteNormal	= CCSprite::create("Images/ui_tab_chara_sa_btn.png");
	spriteNormal->setAnchorPoint(CCPointZero);
	spriteNormal->setPosition(ccp(0,0));
	spriteNormal->setVisible(false);
	addChild(spriteNormal);

	spriteHover		= CCSprite::create("Images/ui_tab_chara_sa_btnhover.png");
	spriteHover->setAnchorPoint(CCPointZero);
	spriteHover->setPosition(ccp(0,0));
	spriteHover->setVisible(false);
	addChild(spriteHover);

	spriteDisable	= NULL;

	spriteSelected	= CCSprite::create("Images/ui_tab_chara_sa_button.png");
	spriteSelected->setAnchorPoint(CCPointZero);
	spriteSelected->setPosition(ccp(0,0));
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	labelAtlas = CCLabelBMFont::create("", FNT_ENG_NUM);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(0,0));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);

	m_obContentSize.width = 136;
	m_obContentSize.height = 30;

	m_bIsEnabled = true;
	scheduleUpdate();
}

void TOSkill::onNormal()
{
	Container::onNormal();
	mLT_Name->setColor(COLOUR_NORMAL);
}

void TOSkill::onHover()
{
	Container::onHover();
	mLT_Name->setColor(COLOUR_HOVER);
}

void TOSkill::onSelect()
{
	m_iState = C_STATE_HOVERD;
	Container::onSelect();
}

void TOSkill::onDisable()
{
	Container::onDisable();
	mLT_Name->setColor(ccGRAY);
}

void TOSkill::setcontent( SkillMeta* sm )
{
	m_smData = sm;
	if(sm){
		mLT_Name->setString(sm->name.c_str());
	}else{
		mLT_Name->setString("NONE");
	}
	onNormal();
}

TOASkill::TOASkill()
{
	mLT_Name = CCLabelTTF::create("-----",FNT_UI_TTF,17);
	mLT_Name->setAnchorPoint(CCPointZero);
	mLT_Name->setPosition(ccp(0,0));
	mLT_Name->setColor(COLOUR_NORMAL);
	addChild(mLT_Name,1);

	spriteNormal	= NULL;

	spriteHover		= NULL;

	spriteDisable	= NULL;

	spriteSelected	= CCSprite::create("Images/icon_select.png");
	spriteSelected->setAnchorPoint(CCPointZero);
	spriteSelected->setPosition(ccp(-28,0));
	spriteSelected->setScale(0.8);
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	labelAtlas = CCLabelBMFont::create("", FNT_ENG_NUM);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(0,0));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);

	m_obContentSize.width = 50;
	m_obContentSize.height = 18;

	m_bIsEnabled = true;
	scheduleUpdate();
}

void TOASkill::onNormal()
{
	Container::onNormal();
	mLT_Name->setColor(COLOUR_NORMAL);
}

void TOASkill::onHover()
{
	Container::onHover();
	mLT_Name->setColor(COLOUR_HOVER);
}

void TOASkill::onSelect()
{
	m_iState = C_STATE_HOVERD;
	Container::onSelect();
}

void TOASkill::onDisable()
{
	Container::onDisable();
	mLT_Name->setColor(ccGRAY);
}

void TOASkill::setcontent( SkillMeta* sm )
{
	m_smData = sm;
	if(sm){
		mLT_Name->setString(sm->name.c_str());
	}else{
		mLT_Name->setString("NONE");
	}
	onNormal();
}

TOTama::TOTama()
{
	spriteNormal	= NULL;

	spriteHover		= NULL;

	spriteDisable	= NULL;

	spriteSelected	= CCSprite::create("Images/icon_select.png");
	spriteSelected->setAnchorPoint(CCPointZero);
	spriteSelected->setPosition(ccp(-15,3));
	spriteSelected->setScale(0.8);
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	labelAtlas = CCLabelBMFont::create("", FNT_ENG_NUM);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(100,0));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);

	m_obContentSize.width = 50;
	m_obContentSize.height = 18;

	m_bIsEnabled = true;
	scheduleUpdate();
}

void TOTama::onNormal()
{
	Container::onNormal();
}

void TOTama::onHover()
{
	onSelect();
}

void TOTama::onSelect()
{
	m_iState = C_STATE_HOVERD;
	Container::onSelect();
}

void TOTama::onDisable()
{
	Container::onDisable();
}

void TOTama::setcontent( int ail )
{
	m_iLevel = ail;
	//CCAssert(ail<6,"ail<6");
	//CCAssert(ail>-1,"ail>=0");
	labelAtlas->setString(tlevel[ail]);
	onNormal();
}
#include "packui/TOSkillVA.h"
#include "Macros.h"

TOSkill::TOSkill()
{
	mLT_Name = CCLabelTTF::create("-----",FNT_UI_TTF,18);
	mLT_Name->setAnchorPoint(CCPointZero);
	mLT_Name->setPosition(ccp(20,0));
	mLT_Name->setColor(COLOUR_NORMAL);
	addChild(mLT_Name);

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


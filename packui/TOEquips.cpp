#include "packui/TOEquips.h"
#include "Macros.h"

TOEquips::TOEquips()
{
	mLT_Name = CCLabelTTF::create("Empty",FNT_UI_TTF,28);
	mLT_Name->setAnchorPoint(CCPointZero);
	mLT_Name->setPosition(ccp(50,0));
	mLT_Name->setColor(ccc3(215,169,119));
	addChild(mLT_Name);

	spriteNormal	= NULL;

	spriteHover		= NULL;

	spriteDisable	= NULL;

	spriteSelected	= CCSprite::create("Images/icon_select.png");
	spriteSelected->setAnchorPoint(CCPointZero);
	spriteSelected->setPosition(ccp(0,0));
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	labelAtlas = CCLabelBMFont::create("0/0", FNT_ENG_NUM);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(280,0));
	labelAtlas->setColor(ccc3(215,169,119));
	this->addChild(labelAtlas,1);

	m_obContentSize.width = 350;
	m_obContentSize.height = 30;

	m_bIsEnabled = true;
	scheduleUpdate();
}

TOEquips::~TOEquips()
{
	;
}

void TOEquips::setcontent( Equip* aeq )
{
	eq = aeq;
	if(!aeq)
	{
		labelAtlas->setString("0/0");
		mLT_Name->setString("Empty");

	}else
	{
		labelAtlas->setString(CCString::createWithFormat("%d/%d",eq->lock,eq->sum)->getCString());
		mLT_Name->setString(eq->name.c_str());
	}
}



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

	labelAtlas = CCLabelBMFont::create("/", FNT_ENG_NUM);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(280,0));
	labelAtlas->setColor(ccc3(215,169,119));
	this->addChild(labelAtlas,1);

	mLB_NJFront = CCLabelBMFont::create("--", FNT_ENG_NUM);
	mLB_NJFront->setAnchorPoint(ccp(1,0));
	mLB_NJFront->setPosition(ccp(280,0));
	mLB_NJFront->setColor(ccc3(215,169,119));
	this->addChild(mLB_NJFront,1);

	mLB_NJBack = CCLabelBMFont::create("--", FNT_ENG_NUM);
	mLB_NJBack->setAnchorPoint(CCPointZero);
	mLB_NJBack->setPosition(ccp(287,0));
	mLB_NJBack->setColor(ccc3(215,169,119));
	this->addChild(mLB_NJBack,1);

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
		//labelAtlas->setString("0/0");
		mLB_NJBack->setString("--");
		mLB_NJFront->setString("--");
		mLT_Name->setString("Empty");

	}else
	{
		//labelAtlas->setString(CCString::createWithFormat("%d/%d",eq->lock,eq->sum)->getCString());
		mLB_NJBack->setString(CCString::createWithFormat("%d",eq->sum)->getCString());
		mLB_NJFront->setString(CCString::createWithFormat("%d",eq->lock)->getCString());
		mLT_Name->setString(eq->name.c_str());
	}
}

void TOEquips::onNormal()
{
	Container::onNormal();
	mLB_NJFront->setColor(COLOUR_NORMAL);
	mLB_NJBack->setColor(COLOUR_NORMAL);
	mLT_Name->setColor(COLOUR_NORMAL);
}

void TOEquips::onHover()
{
	if(!m_bIsEnabled) return;
	Container::onHover();
	mLB_NJFront->setColor(COLOUR_HOVER);
	mLB_NJBack->setColor(COLOUR_HOVER);
	mLT_Name->setColor(COLOUR_HOVER);
}

void TOEquips::onSelect()
{
	Container::onSelect();
}

void TOEquips::onDisable()
{
	Container::onDisable();
	mLB_NJFront->setColor(ccGRAY);
	mLB_NJBack->setColor(ccGRAY);
	mLT_Name->setColor(ccGRAY);
}
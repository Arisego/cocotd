#include "sublayer/TextLayerBtn.h"
#include "Macros.h"

TlBtn::TlBtn()
{
	spriteNormal	= NULL;

	spriteHover		= NULL;

	spriteDisable	= NULL;

	spriteSelected	= CCSprite::create("Images/ui_tlb_sel.png");
	spriteSelected->setAnchorPoint(CCPointZero);
	spriteSelected->setPosition(ccp(0,0));
	spriteSelected->setVisible(false);
	addChild(spriteSelected);

	//spriteNormal	= CCSprite::create("Images/ui_tlb_sel.png");
	//spriteNormal->setAnchorPoint(CCPointZero);
	//spriteNormal->setPosition(ccp(0,0));
	//spriteNormal->setVisible(false);
	//addChild(spriteNormal);

	labelAtlas = CCLabelBMFont::create("", FNT_CHN);	
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(0,-6));
	labelAtlas->setScale(0.8);
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);

	m_obContentSize.width = 40;
	m_obContentSize.height = 15;

	m_bIsEnabled = true;
	scheduleUpdate();
}

TlBtn::~TlBtn()
{
	;
}

void TlBtn::onNormal()
{
	Container::onNormal();
}

void TlBtn::onHover()
{
	Container::onHover();
	labelAtlas->setColor(ccRED);
}

void TlBtn::onSelect()
{
	Container::onSelect();
	labelAtlas->setColor(ccBLUE);
}

void TlBtn::onDisable()
{
	Container::onDisable();
}
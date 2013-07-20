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

	labelAtlas = CCLabelBMFont::create("", FNT_ENG_NUM);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setPosition(ccp(0,0));
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
}

void TlBtn::onSelect()
{
	Container::onSelect();
}

void TlBtn::onDisable()
{
	Container::onDisable();
}
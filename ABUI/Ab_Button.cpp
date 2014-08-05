#include "Ab_Button.h"
#include "..\Corrode\Classes\SoundManager.h"

#define OFF_SET ccp(-5,-3)

AbButton::AbButton(const char* aPth, int aw, int ah)
{
	Container::Container();

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;
	spriteHover		= NULL;

	std::stringstream tss;
	tss << "Images/UI/btn_";
	tss << aPth;

	spriteNormal = CCSprite::create( (tss.str()+".png").c_str());
	spriteNormal->setAnchorPoint(CCPointZero);
	spriteNormal->setPosition(OFF_SET);
	addChild(spriteNormal);

	spriteHover = CCSprite::create( (tss.str()+"_hov.png").c_str());
	spriteHover->setAnchorPoint(CCPointZero);
	spriteHover->setPosition(OFF_SET);
	addChild(spriteHover);

	labelAtlas = new CCLabelBMFont();
	labelAtlas->autorelease();
	labelAtlas->setVisible(false);
	addChild(labelAtlas);

	m_obContentSize.width = aw;
	m_obContentSize.height = ah;

	onNormal();
	m_iState = C_STATE_NORMAL;

	scheduleUpdate();
}

void AbButton::onSelect()
{
	if(C_STATE_SELECT == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_HOVERD;
	SoundManager::sharedSoundManager()->PlayDecideSFX();
	spriteNormal->setVisible(true);
}


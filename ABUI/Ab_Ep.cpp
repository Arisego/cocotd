#include "Ab_Ep.h"
#include "..\Corrode\Classes\SoundManager.h"

AbEpisod::AbEpisod(int eid, int aw, int ah)
{
	Container::Container();
	miEID = eid;

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;
	spriteHover		= NULL;

	std::stringstream tss;
	tss << "Images/UI/ep_";
	tss << eid;
	
	spriteNormal = CCSprite::create( (tss.str()+".png").c_str());
	spriteNormal->setAnchorPoint(CCPointZero);
	spriteNormal->setPosition(CCPointZero);
	addChild(spriteNormal);

	spriteHover = CCSprite::create( (tss.str()+"_hov.png").c_str());
	spriteHover->setAnchorPoint(CCPointZero);
	spriteHover->setPosition(CCPointZero);
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

void AbEpisod::onSelect()
{
	if(C_STATE_SELECT == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_HOVERD;
	SoundManager::sharedSoundManager()->PlayDecideSFX();
	spriteNormal->setVisible(true);
}


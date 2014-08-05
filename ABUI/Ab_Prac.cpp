#include "Ab_Prac.h"
#include "..\Corrode\Classes\SoundManager.h"

ABPrac::ABPrac(int cid, int aw, int ah)
{
	Container::Container();

	miPracID = cid;

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;
	spriteHover		= NULL;

	stringstream tss;
	tss << "Images/UI/prac_";
	tss << cid;
	string asPre = tss.str();

	std::string		tsPath;
	tsPath = asPre;
	tsPath += ".png";
	spriteNormal = CCSprite::create(tsPath.c_str());
	if(spriteNormal){		
		spriteNormal->setContentSize(CCSize(aw,ah));
		spriteNormal->setAnchorPoint(ccp(0,0));
		//spriteNormal->setVisible(false);
		addChild(spriteNormal);
	}

	tsPath = asPre;
	tsPath += "_hov.png";
	spriteHover = CCSprite::create(tsPath.c_str());
	if(spriteHover){		
		spriteHover->setContentSize(CCSize(aw,ah));
		spriteHover->setAnchorPoint(ccp(0,0));
		spriteHover->setVisible(false);
		addChild(spriteHover);
	}

	tsPath = asPre;
	tsPath += "_dis.png";
	spriteDisable = CCSprite::create(tsPath.c_str());
	if(spriteDisable){		
		spriteDisable->setContentSize(CCSize(aw,ah));
		spriteDisable->setAnchorPoint(ccp(0,0));
		spriteDisable->setVisible(false);
		addChild(spriteDisable);
	}

	labelAtlas = new CCLabelBMFont();
	labelAtlas->autorelease();
	labelAtlas->setVisible(false);
	addChild(labelAtlas);

	m_obContentSize.width = aw;
	m_obContentSize.height = ah;

	m_iState = C_STATE_NORMAL;
	
	scheduleUpdate();
}

void ABPrac::onSelect()
{
	if(C_STATE_SELECT == m_iState || !m_bIsEnabled) return;
	m_iState = C_STATE_HOVERD;
	SoundManager::sharedSoundManager()->PlayDecideSFX();
	spriteNormal->setVisible(true);
}


#include "Ab_Head.h"

ABHead::ABHead(int cid, int aw, int ah)
{
	Container::Container();
	miCID = cid;

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;
	spriteHover		= NULL;

	stringstream tss;
	tss << "Images/UI/head_";
	tss << cid;
	string asPre = tss.str();

	std::string		tsPath;
	tsPath = asPre;
	tsPath += "_nor.png";
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
	tsPath += "_sel.png";
	spriteSelected = CCSprite::create(tsPath.c_str());
	if(spriteSelected){		
		spriteSelected->setContentSize(CCSize(aw,ah));
		spriteSelected->setAnchorPoint(ccp(0,0));
		spriteSelected->setVisible(false);
		addChild(spriteSelected);
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


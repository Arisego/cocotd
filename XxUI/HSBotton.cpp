#include "HSBotton.h"

HSButton::HSButton(const char* aHover, const char* aSelect,int aw, int ah, bool aem)
{
	Container::Container();

	mbEmerge = aem;

	spriteNormal	= NULL;
	spriteDisable	= NULL;
	spriteSelected	= NULL;

	spriteHover = CCSprite::create(aHover); 
	spriteHover->setContentSize(CCSize(aw,ah));
	spriteHover->setAnchorPoint(ccp(0,0));
	spriteHover->setVisible(false);
	addChild(spriteHover);

	if(strlen(aSelect)>0){
		spriteSelected = CCSprite::create(aHover); 
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

void HSButton::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if(!isVisible()) return;
	if(checkTouch(pTouch)){
		onSelect();
		if(mbEmerge) activate();
		else runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0), CCCallFunc::create(this, callfunc_selector(Container::activate))));	

	}
	return;
}


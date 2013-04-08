#include "hud/LtLifeBlock.h"

#define LT_AVA_WIDTH 100
#define LT_AVA_HEITH 100

bool LTLifeBlock::init()
{
	do{
		//HP and MP and an avatar , providing refresh method for view.
		mpChara = NULL;
		m_lbhp = NULL;
		m_lbmp = NULL;
		m_avatar = NULL;

		return true;
	}while(0);
	return false;
}

void LTLifeBlock::setChara()				
{
	Chara* a_chara = CharaS::sharedCharaS()->getdispchara();
	mpChara = NULL;
	if(!a_chara) return;
	removeAllChildren();
	mpChara = a_chara;
	CC_SAFE_RELEASE_NULL(m_lbhp);
	CC_SAFE_RELEASE_NULL(m_lbmp);
	CC_SAFE_RELEASE_NULL(m_avatar);

	onShow();

}

void LTLifeBlock::RefreshValue()
{
	m_lbhp->revalue(mpChara->getvalue("hp"));
	m_lbmp->revalue(mpChara->getvalue("mp"));

	m_lbhp->setval(mpChara->gethp());
	m_lbmp->setval(mpChara->getmp());
}

void LTLifeBlock::onShow()
{
	setVisible(true);
	if(!mpChara) {
		setChara();
		return;
	}
		
	if(!m_lbhp){
		m_lbhp = new LifeBar("Images/life_bar.png",mpChara->getvalue("hp"));
		m_lbhp->setAnchorPoint(CCPointZero);
		m_lbhp->setPosition(ccp(LT_AVA_WIDTH + 5,20));
		addChild(m_lbhp,1);
	}

	if(!m_lbmp){
		m_lbmp = new LifeBar("Images/life_bar1.png",mpChara->getvalue("hp"));
		m_lbmp->setAnchorPoint(CCPointZero);
		m_lbmp->setPosition(ccp(LT_AVA_WIDTH + 5,0));
		addChild(m_lbmp,1);
	}

	if(!m_avatar){
		m_avatar = CCSprite::create("Images/avatar.png");			//TODO: Read Avatar from db.
		m_avatar->setAnchorPoint(CCPointZero);
		m_avatar->setPosition(ccp(1,1));
		addChild(m_avatar);
	}

	RefreshValue();
}

void LTLifeBlock::onDismiss()
{
	setVisible(false);
}

#include "hud/LStatBar.h"

bool LStatBar::init()
{
	do 
	{
		CCSprite* tcs = CCSprite::create("Images/hud_ls_base.png");
		tcs->setAnchorPoint(CCPointZero);
		tcs->setPosition(CCPointZero);
		addChild(tcs);

		ml_Hit = CCLabelBMFont::create("--", FNT_ENG_NUM);
		ml_Hit->setScale(0.7);
		ml_Hit->setAnchorPoint(CCPointZero);
		ml_Hit->setPosition(ccp(83,114));
		ml_Hit->setColor(ccc3(215,169,119));
		this->addChild(ml_Hit,1);

		ml_Dmg = CCLabelBMFont::create("--", FNT_ENG_NUM);
		ml_Dmg->setScale(0.7);
		ml_Dmg->setAnchorPoint(CCPointZero);
		ml_Dmg->setPosition(ccp(160,114));
		ml_Dmg->setColor(ccc3(215,169,119));
		this->addChild(ml_Dmg,1);

		ml_Crt = CCLabelBMFont::create("--", FNT_ENG_NUM);
		ml_Crt->setScale(0.7);
		ml_Crt->setAnchorPoint(CCPointZero);
		ml_Crt->setPosition(ccp(224,114));
		ml_Crt->setColor(ccc3(215,169,119));
		this->addChild(ml_Crt,1);

		ml_Par = CCLabelBMFont::create("--", FNT_ENG_NUM);
		ml_Par->setScale(0.7);
		ml_Par->setAnchorPoint(CCPointZero);
		ml_Par->setPosition(ccp(89,96));
		ml_Par->setColor(ccc3(215,169,119));
		this->addChild(ml_Par,1);

		ml_Ctn = CCLabelBMFont::create("--", FNT_ENG_NUM);
		ml_Ctn->setScale(0.7);
		ml_Ctn->setAnchorPoint(CCPointZero);
		ml_Ctn->setPosition(ccp(158,96));
		ml_Ctn->setColor(ccc3(215,169,119));
		this->addChild(ml_Ctn,1);

		ml_Level = CCLabelBMFont::create("--", FNT_ENG_NUM);
		ml_Level->setScale(0.85);
		ml_Level->setOpacity(100);
		ml_Level->setAnchorPoint(CCPointZero);
		ml_Level->setPosition(ccp(42,134));
		ml_Level->setColor(ccWHITE);
		this->addChild(ml_Level,1);

		mb_Exp = new ExpBar("Images/hud_ls_expbase.png","Images/hud_exp_content.png",100);
		mb_Exp->setAnchorPoint(CCPointZero);
		mb_Exp->setPosition(ccp(88,88));
		this->addChild(mb_Exp,2);

		//mb_Exp->setval(48);

		ms_BSBase = CCSprite::create("Images/hud_bl_base.png");
		ms_BSBase->setAnchorPoint(CCPointZero);
		ms_BSBase->setPosition(ccp(62.4,133));
		this->addChild(ms_BSBase,3);

		mb_BloodSecond = new ExpBar("Images/blank.png","Images/hud_ls_bloodbase.png",100);
		mb_BloodSecond->setAnchorPoint(CCPointZero);
		mb_BloodSecond->setPosition(ccp(77.4,134.5));
		this->addChild(mb_BloodSecond,3);

		mb_BloodSecond->setval(100);

		mb_BloodOne = new ExpBar("Images/blank.png","Images/hud_ls_bloodbase.png",100);
		mb_BloodOne->setAnchorPoint(CCPointZero);
		mb_BloodOne->setPosition(ccp(77.4,147));
		this->addChild(mb_BloodOne,3);

		mb_BloodOne->setval(69);

		msb_SoulBall = SoulBall::create();
		msb_SoulBall->setAnchorPoint(CCPointZero);
		msb_SoulBall->setPosition(ccp(58,160));
		this->addChild(msb_SoulBall,4);

		msb_SoulBall->setSum(9);
		msb_SoulBall->setAli(5);


		return true;
	} while (0);
	return false;

}

void LStatBar::SetContent( EChesses* aec )
{
	if(m_ec == aec){
		setVisible(true);
		return;
	}
	m_ec = aec;
	RefreshAll();
	//setVisible(true);
}

void LStatBar::RefreshAll()
{
	Chara* t_owner = m_ec->m_pChara;
	mb_Exp->setval(t_owner->getvalue("exp")%100);
}

void LStatBar::RefreshAct( int a1, int a2, int a3 )
{
	ml_Hit->setString(CCString::createWithFormat("%d",a1)->getCString());
	ml_Crt->setString(CCString::createWithFormat("%d",a2)->getCString());
	ml_Dmg->setString(CCString::createWithFormat("%d",a3)->getCString());
}

void LStatBar::SetNullAct(){
	ml_Hit->setString("--");
	ml_Crt->setString("--");
	ml_Dmg->setString("--");
}
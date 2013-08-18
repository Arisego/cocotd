#include "hud/RStatBar.h"

bool RStatBar::init()
{
	do 
	{
		CCSprite* tcs = CCSprite::create("Images/hud_rs_base.png");
		tcs->setAnchorPoint(CCPointZero);
		tcs->setPosition(CCPointZero);
		addChild(tcs);

		ml_Hit = CCLabelBMFont::create("99", FNT_ENG_NUM);
		ml_Hit->setScale(0.7);
		ml_Hit->setAnchorPoint(CCPointZero);
		ml_Hit->setPosition(ccp(209,114));
		ml_Hit->setColor(ccc3(215,169,119));
		this->addChild(ml_Hit,1);

		ml_Dmg = CCLabelBMFont::create("99", FNT_ENG_NUM);
		ml_Dmg->setScale(0.7);
		ml_Dmg->setAnchorPoint(CCPointZero);
		ml_Dmg->setPosition(ccp(285,114));
		ml_Dmg->setColor(ccc3(215,169,119));
		this->addChild(ml_Dmg,1);

		ml_Crt = CCLabelBMFont::create("99", FNT_ENG_NUM);
		ml_Crt->setScale(0.7);
		ml_Crt->setAnchorPoint(CCPointZero);
		ml_Crt->setPosition(ccp(349,114));
		ml_Crt->setColor(ccc3(215,169,119));
		this->addChild(ml_Crt,1);

		ml_Par = CCLabelBMFont::create("99", FNT_ENG_NUM);
		ml_Par->setScale(0.7);
		ml_Par->setAnchorPoint(CCPointZero);
		ml_Par->setPosition(ccp(217,96));
		ml_Par->setColor(ccc3(215,169,119));
		this->addChild(ml_Par,1);

		ml_Ctn = CCLabelBMFont::create("99", FNT_ENG_NUM);
		ml_Ctn->setScale(0.7);
		ml_Ctn->setAnchorPoint(CCPointZero);
		ml_Ctn->setPosition(ccp(282,96));
		ml_Ctn->setColor(ccc3(215,169,119));
		this->addChild(ml_Ctn,1);

		ml_Level = CCLabelBMFont::create("99", FNT_ENG_NUM);
		ml_Level->setScale(0.85);
		ml_Level->setOpacity(100);
		ml_Level->setAnchorPoint(CCPointZero);
		ml_Level->setPosition(ccp(359,134));
		ml_Level->setColor(ccWHITE);
		this->addChild(ml_Level,1);

		mb_Exp = new ExpBar("Images/hud_ls_expbase.png","Images/hud_exp_content.png",100);
		mb_Exp->setAnchorPoint(CCPointZero);
		mb_Exp->setPosition(ccp(213,88));
		this->addChild(mb_Exp,2);

		mb_Exp->setval(48);

		ms_BSBase = CCSprite::create("Images/hud_bl_base.png");
		ms_BSBase->setAnchorPoint(CCPointZero);
		ms_BSBase->setPosition(ccp(176,133));
		ms_BSBase->setFlipX(true);
		this->addChild(ms_BSBase,3);

		mb_BloodSecond = new ExpBar("Images/blank.png","Images/hud_ls_bloodbase.png",100);
		mb_BloodSecond->setAnchorPoint(CCPointZero);
		mb_BloodSecond->setPosition(ccp(178,134.5));
		this->addChild(mb_BloodSecond,3);

		mb_BloodSecond->setval(100);

		mb_BloodOne = new ExpBar("Images/blank.png","Images/hud_ls_bloodbase.png",100);
		mb_BloodOne->setAnchorPoint(CCPointZero);
		mb_BloodOne->setPosition(ccp(178,147));
		this->addChild(mb_BloodOne,3);

		mb_BloodOne->setval(69);

		msb_SoulBall = SoulBall::create();
		msb_SoulBall->setAnchorPoint(CCPointZero);
		msb_SoulBall->setPosition(ccp(178,160));
		this->addChild(msb_SoulBall,4);

		msb_SoulBall->setSum(9);
		msb_SoulBall->setAli(5);


		return true;
	} while (0);
	return false;

}

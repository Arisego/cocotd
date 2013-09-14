#ifndef __STAT_BAR_H__
#define __STAT_BAR_H__

#include "cocos2d.h"
#include "utils/EChess.h"
#include "hud/ExpBar.h"
#include "hud/SoulBall.h"

USING_NS_CC;

/*
* StatBar || <两边状态显示的原型类
*/
class StatBar: public CCLayer
{
public:
	void SetContent(EChesses* aec);

	void RefreshAll();
	void RefreshAct(int a1, int a2, int a3);
	void SetNullAct();

	StatBar();
protected:
	EChesses* m_ec;

	CCLabelBMFont* ml_Hit;
	CCLabelBMFont* ml_Dmg;
	CCLabelBMFont* ml_Crt;
	CCLabelBMFont* ml_Par;
	CCLabelBMFont* ml_Ctn;

	CCLabelBMFont* ml_Hp;

	CCLabelBMFont* ml_Level;

	ExpBar*		   mb_Exp;
	ExpBar*		   mb_BloodOne;		// <第一根血条
	ExpBar*		   mb_BloodSecond;	// <第二根血条
	CCSprite*	   ms_BSBase;		// <第二根血条的背景

	SoulBall*	   msb_SoulBall;	// <
};

#endif
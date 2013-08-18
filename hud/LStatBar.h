#ifndef __L_STAT_BAR_H__
#define __L_STAT_BAR_H__

#include "cocos2d.h"
#include "utils/EChess.h"
#include "hud/ExpBar.h"
#include "hud/SoulBall.h"

USING_NS_CC;

//Left Status Bar
class LStatBar : public CCLayer
{
public:
	virtual bool init();
	void SetContent(EChesses* aec);

	void RefreshAll();
	void RefreshAct(int a1, int a2, int a3);
	void SetNullAct();
private:
	EChesses* m_ec;

	CCLabelBMFont* ml_Hit;
	CCLabelBMFont* ml_Dmg;
	CCLabelBMFont* ml_Crt;
	CCLabelBMFont* ml_Par;
	CCLabelBMFont* ml_Ctn;

	CCLabelBMFont* ml_Level;

	ExpBar*		   mb_Exp;
	ExpBar*		   mb_BloodOne;		// <第一根血条
	ExpBar*		   mb_BloodSecond;	// <第二根血条
	CCSprite*	   ms_BSBase;		// <第二根血条的背景

	SoulBall*	   msb_SoulBall;	// <
};


#endif
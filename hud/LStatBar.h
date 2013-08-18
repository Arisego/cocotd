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
	ExpBar*		   mb_BloodOne;		// <��һ��Ѫ��
	ExpBar*		   mb_BloodSecond;	// <�ڶ���Ѫ��
	CCSprite*	   ms_BSBase;		// <�ڶ���Ѫ���ı���

	SoulBall*	   msb_SoulBall;	// <
};


#endif
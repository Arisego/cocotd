#ifndef __LG_STAT_H__
#define __LG_STAT_H__

#include "cocos2d.h"
USING_NS_CC;

class LgStat : public CCLayer
{
public:
	LgStat();
	~LgStat();
	void activate();

private:
	CCSprite* mSp_Outer;
	CCSprite* mSp_Inner;
	CCParticleSystem* mPs_sun;
	CCSprite* mSp_Tick;

	virtual bool init();
	int miState;	//  < [1 - ��] - [2 - ����ʱ]

	virtual void update(float dt) override;

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;

	bool m_bIsEnabled;
	void CountOver();

	float mfDel;
	float mfSum;
	bool	mbStat;	// <Defend State

	void StatOver();

public:
	void CountDown(float time,CCObject* target, SEL_MenuHandler selector);
	void GiveUpCont();		// <ֱ��ȡ������ʱ

	void SetDefend(bool abStat);	// <����״̬����		
};


#endif
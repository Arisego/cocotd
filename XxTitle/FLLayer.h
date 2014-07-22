#ifndef __FL_LAYER_H__
#define __FL_LAYER_H__

#include "cocos2d.h"
#include "utils\States.h"
#include "XxUI\HSBotton.h"
#include "utils\Controller.h"


USING_NS_CC;

class FlLayer :  public BYLayerDescendant, public StateMachine, public Cake
{

public:
	virtual void Pause();
	virtual void Resume();
	virtual void Close();
	~FlLayer();

	virtual bool init() override;

	virtual void right_click() override;
private:
	CCSprite*	mcsBackGround;
	CCSprite*	mcsLeft;
	CCSprite*	mcsRight;
	CCSprite*	mcsTitle;

	CCSprite*	mcsSLaves;
	CCSprite*	mcsEnterIn;

	HSButton*	mBtnEnterIn;

public:
	void		BeginShow();	/* <������Ч */
	void		ELoadFinal();	/* <��Ч�ս� */

	void		BGCallBack(CCObject* pSender);	/* <������ť�ص� */
	//virtual void byTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	virtual void registerWithTouchDispatcher() override;
};

#endif
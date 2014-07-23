#ifndef __FL_LAYER_H__
#define __FL_LAYER_H__

#include "cocos2d.h"
#include "utils\States.h"
#include "XxUI\HSBotton.h"
#include "utils\Controller.h"
#include "XxUI\FLButton.h"
#include "XxUI\SLTab.h"


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

	HSButton*	mBtnEnterIn;			/* <EnterIn��ť */
	std::vector<FLButton*>	mvsBtnSLs;	/* <������ť�� */
	BYLayerDescendant*	mBaSLArea;

public:
	void		BeginShow();	/* <������Ч */
	void		ELoadFinal();	/* <��Ч�ս� */

	void		BGCallBack(CCObject* pSender);	/* <������ť�ص� */
	void		SLBtnBack(CCObject* pSender);	/* <������ť�ص� */

	void		SetActiveBtn(FLButton* tfl);
	void		UnSetActive(FLButton* tfl);
	FLButton*	mFlBtnActive;					/* <��ǰ�Ǽǻ�Ծ��FLBtn */
	SLCell*		mSLCell;						/* <չʾ��SL */

	virtual void registerWithTouchDispatcher() override;
};

#endif
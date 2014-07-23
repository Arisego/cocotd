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

	HSButton*	mBtnEnterIn;			/* <EnterIn按钮 */
	std::vector<FLButton*>	mvsBtnSLs;	/* <读档按钮组 */
	BYLayerDescendant*	mBaSLArea;

public:
	void		BeginShow();	/* <开场特效 */
	void		ELoadFinal();	/* <特效终结 */

	void		BGCallBack(CCObject* pSender);	/* <开场按钮回调 */
	void		SLBtnBack(CCObject* pSender);	/* <读档按钮回调 */

	void		SetActiveBtn(FLButton* tfl);
	void		UnSetActive(FLButton* tfl);
	FLButton*	mFlBtnActive;					/* <当前登记活跃的FLBtn */
	SLCell*		mSLCell;						/* <展示的SL */

	virtual void registerWithTouchDispatcher() override;
};

#endif
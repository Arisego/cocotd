#ifndef __AB_ASICLAYER_H__
#define __AB_ASICLAYER_H__

/*
	<AB BasicLayer
	< 最低层|背景和按钮
*/

#include "cocos2d.h"
#include "byeven/BYCocos.h"
#include "Ab_Head.h"
#include "utils/States.h"
#include "Ab_Next.h"
USING_NS_CC;

class ABasicLayer: public BYLayerDescendant, public StateMachine
{
public:
	ABasicLayer();
	~ABasicLayer();

	CREATE_FUNC(ABasicLayer);

	void	HdBack(CCObject* sender);		/* <头像点击返回 */
	void	PopupBack(CCObject* sender);	/* <弹出人物属性返回 */
	void	SCBack(CCObject* sender);		/* <系统相关三按钮返回 */
private:
	std::vector<ABHead*>	mvHeads;	// <左侧头像列表
	virtual bool init();
	virtual void registerWithTouchDispatcher();

	virtual void Close();	
	virtual void Pause();	
	virtual void Resume();

	int	miCid;	// <被选中的chara的ID

protected:
	CCLabelBMFont* mlbMoney;	// <金钱
	AbNext*	mAbNext;			// <下一个
};


#endif
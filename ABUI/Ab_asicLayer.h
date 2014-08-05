#ifndef __AB_ASICLAYER_H__
#define __AB_ASICLAYER_H__

/*
	<AB BasicLayer
	< ��Ͳ�|�����Ͱ�ť
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

	void	HdBack(CCObject* sender);		/* <ͷ�������� */
	void	PopupBack(CCObject* sender);	/* <�����������Է��� */
	void	SCBack(CCObject* sender);		/* <ϵͳ�������ť���� */
private:
	std::vector<ABHead*>	mvHeads;	// <���ͷ���б�
	virtual bool init();
	virtual void registerWithTouchDispatcher();

	virtual void Close();	
	virtual void Pause();	
	virtual void Resume();

	int	miCid;	// <��ѡ�е�chara��ID

protected:
	CCLabelBMFont* mlbMoney;	// <��Ǯ
	AbNext*	mAbNext;			// <��һ��
};


#endif
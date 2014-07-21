#ifndef __MAIN_LAYER_H__
#define __MAIN_LAYER_H__

#include "cocos2d.h"
#include "MenuLayer.h"

USING_NS_CC;

class MainLayer :  public CCLayer, public StateMachine
{
public:
	MainLayer();
	~MainLayer();
	virtual bool init() override;

	bool	mbLoading;
public:
	void	ELoadFinal();	// <�����������ʵͼƬ
	void	PlayOpMusic();	// <����OP1��������

private:
	CCSprite* msLogo;
	CCSprite* msLogo2;

	CCSprite* msBg;
	CCSprite* msMenu;
	CCSprite* msWys;	// <Tag:WYS

	CCGLProgram *mglProgLogo;

public:
	virtual void Close() override;	// <�ر���ʾ
	virtual void Pause() override;	// <��ͣ�¼���
	virtual void Resume() override;

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
};


#endif
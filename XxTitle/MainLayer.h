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
	void	ELoadFinal();	// <清除和载入真实图片
	void	PlayOpMusic();	// <播放OP1背景音乐

private:
	CCSprite* msLogo;
	CCSprite* msLogo2;

	CCSprite* msBg;
	CCSprite* msMenu;
	CCSprite* msWys;	// <Tag:WYS

	CCGLProgram *mglProgLogo;

public:
	virtual void Close() override;	// <关闭显示
	virtual void Pause() override;	// <暂停事件流
	virtual void Resume() override;

	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
};


#endif
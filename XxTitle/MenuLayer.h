#ifndef __MENU_LAYER_H__
#define __MENU_LAYER_H__

#include "byeven\BYCocos.h"
#include "utils\States.h"
#include "XxUI\HSBotton.h"

class TitleMMenu : public BYLayerDescendant, public StateMachine
{
protected:
	CCSprite* msMenu;
	std::vector<HSButton*> mvBtns;

public:
	TitleMMenu(void);
	~TitleMMenu();

	void	EnableAllBtns();

public:
	virtual void registerWithTouchDispatcher();

	void menuCallback(CCObject* pSender);
	void menuCallback2(CCObject* pSender);
	void onQuit(CCObject* pSender);

	virtual void Pause();
	virtual void Resume();
	virtual void Close();

};

#endif
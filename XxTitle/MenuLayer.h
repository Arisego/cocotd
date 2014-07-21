#ifndef __MENU_LAYER_H__
#define __MENU_LAYER_H__

#include "byeven\BYCocos.h"
#include "utils\States.h"

class TitleMMenu : public BYLayerDescendant, public StateMachine
{
protected:
	CCMenuItem*    m_disabledItem;

public:
	TitleMMenu(void);
	~TitleMMenu();

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
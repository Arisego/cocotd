#ifndef __BATTLE_MENU_H__
#define __BATTLE_MENU_H__

#include "packui/Container.h"
//BattleMenu, A simple pop up menu for battle map.
class BattleMenu : public BYLayerDescendant
{
public:
	int bitLock;

	bool f_init();
	void show();
	void dismiss();
	

private:
	void Add_Button(const char* name,int tag);
	std::vector<Container*> m_vBtns;
	float m_fHeight,m_fWidth;
	

	void buttonback(CCObject* pSender);

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate(CCObject* pSender);
	

public:
	void setactivator(CCObject* target, SEL_MenuHandler selector);
	bool m_bIsEnabled;
	void Refresh_Button();
	int miFlag;					//The Select one.
};


#endif
#ifndef __CHARA_STATE_H__
#define __CHARA_STATE_H__


#include "StateCenter.h"

#include "packui/Container.h"
#include "packui/LifeBar.h"
#include "packui/ListDBView.h"
#include "packui/CharaS.h"
#include "packui/InterfaceEffect.h"

/*
	CharaState
	<用于单个角色的显示
	<longstate和listdns分别对应简略和详细模式。ministate只被用于小的charalistview
*/

class CharaState : public Container, public InterfaceEffect
{
protected:
	bool m_bKill3333;

	Chara* mpChara;
	CCSprite* mpChraSprite;
	int m_iChState;		//The State for long or full
	LifeBar* lbhp;
	LifeBar* lbmp;

	ListDBView* m_ldbEquList;		//m_ldbEquList -> Equ List
	CCLabelTTF* m_tDis;		//member TTF description

	float m_x,m_y;
	void flush_value(string name, int value);

	BYLayerDescendant* r_mb;	//right side for showall;
	vector<map<string,string>> vdata;
	CCDictionary* m_cdEquips;	//Reade equips from database; It's content Equip* is order by item_id not the p_id.
	//CCArray* m_ltEquips;
	vector<Container*> m_vEBtns;	//Refresh the EQU list.
	int m_icurE;					//flag for current q&e buttons. It's duplicate with m_iGroup.
	int m_iItem, m_iGroup;			//flags for m_ldbEquList. the group and item. -1 groups skill. 0~6 for positions. Get item additional information through m_ldbEquList interface.
	Container* m_cbIClick;			//button for item selected.
	Container* m_cbSChange;			//button for change skill view.
	Container* m_cbSPackD;			//button for equip down.
	BYLayerDescendant* r_inBox;			//for right in box.
	CCLabelBMFont* m_testdeffer;	//differ showing ,test only.
	BYLayerDescendant* l_mb;		//Left main box.

public:
	void setcontent(Chara* apChara);
	Chara* getcontent();

	bool f_init();
	void setsprite();

	void RefreshValues();			//Refresh Digital Values || hp , mp -- not calling reval() provide by lifebar, change it if needed.
	void RefreshEBtns();			//Refresh EQU list. Control by m_iCurE. Data stored in m_cdEquips. It's button list, not m_ldbEquList.

	void show_content();
	void ItemBack(CCObject* pSender);
	void eqBack(CCObject* pSender);			//One of the EQU is clicked, prepare for change & call show_listndis(). || 0~6 for normal eq back && -1 for Ski request.
	void useItemB(CCObject* pSender);
	void showEquDiffer();			//TODO:Show out the differs.
	void changeEqui(CCObject* pSender);				//The EQUIP has changed.
	void skiback(CCObject* pSender);				//The skill is going to showback.

	void setmbclicks(string title);
	void setscclicks(string title);

	void show_long();
	void show_all();
	void show_listndis();			//Show the list and description of EQU or Ski. || Control by m_iGroup.
	void show_mini();				//Showing in mini m_ldbEquList, never change to other state.

	//////////////////////////////////////////////////////////////////////////
	virtual void get_target();
	void targetBack(CCObject*);
	virtual void effect_over();
	virtual void show_text(string s);
	virtual void refresh_view();

	//////////////////////////////////////////////////////////////////////////

	virtual void onSelect();
	virtual void update(float fDelta);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
		Container::ccTouchMoved(pTouch,pEvent);
		BYLayer::ccTouchMoved(pTouch,pEvent);
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
		Container::ccTouchBegan(pTouch,pEvent);
		return BYLayer::ccTouchBegan(pTouch,pEvent);
	}
	//////////////////////////////////////////////////////////////////////////
	void refresh_attrtable();
	void refresh_equlist();

	~CharaState();
};

#endif
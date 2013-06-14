#ifndef __CHARA_LIST_VIEW_H__
#define __CHARA_LIST_VIEW_H__

#include "packui/CharaState.h"
#include "packui/CharaS.h"

/*
	CharaListView
	<用于显示角色列表。
	<Prepare_xxx作转换动画控制
*/
class CharaListView : public BYLayerDescendant
{
public:
	virtual bool init();
	int m_iNums, m_iCurs, m_iBegin;
	CCArray* charalist;
	int m_iCLVState;

	void setbegin(int a_iBe);
	CharaListView();
	~CharaListView();

protected:
	CharaS* g_charas;
	//vector<Container*> lrbtns;
	Container* t_cbLeft;
	Container* t_cbRight;

private:
	void RefreshList();
	void CharasCBack(CCObject* pSender);
	void lrBack(CCObject* pSender);
	void RefreshLR();

	void PrepareSingle();		//Prepare showing for single
	void PrepareBack(CCObject* pSender);

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;

	CharaState* m_tcsTarget;
	
public:
	void RefreshListValue();
	void showtargettext(string s, int type = 0);
	void activate(CCObject*);	//CallBack
	void setactivator(CCObject* target, SEL_MenuHandler selector);

	int m_iItemCount,m_iMiniType;			//Items should not goto -1
	CCLabelBMFont* m_ic;
	void seticc(int i);
};

#endif
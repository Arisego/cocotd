#ifndef __CHARA_LIST_VIEW_H__
#define __CHARA_LIST_VIEW_H__

#include "packui/CharaState.h"
#include "packui/CharaS.h"

/*
	CharaListView
	<用于显示角色列表。
	<Prepare_xxx作转换动画控制
	<目前所拥有的状态
		1<Mini状态 --	-1
			<为技能和物品使用提供弹出的选择,通过activate进行返回
		2<列表状态 --	1，2
			展示CharaState的列表
			1：大列表
			2：单个展开

*/
class CharaListView : public BYLayerDescendant
{
public:
	virtual bool init();
	int m_iNums, m_iCurs, m_iBegin;
	CCArray* charalist;
	
	/* 标识当前CharaListView的状态
			-1	--	<Mini状态
			1	--	<大列表状态
			2	--	<列表中的单个CharaState展开
	*/
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
	/* <更新CharaListView，包括数值，位置，按钮 */
	void RefreshList();

	/*	<负责状态转发的函数 */
	void CharasCBack(CCObject* pSender);
	/* Left Right <切换按钮返回 */
	void lrBack(CCObject* pSender);
	/* LR按钮显示状态刷新 */
	void RefreshLR();
	/* 准备进行1和2的状态切换<并播放动画 */
	void PrepareSingle();	
	/* 1和2切换完成，进行CharaState的<放大操作 */
	void PrepareBack(CCObject* pSender);
	/* <[TO_USE]顺序发生改变的弹出用回调 */
	void PopupBack(CCObject* pSender);

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;

	CharaState* m_tcsTarget;
	
public:
	void RefreshListValue();
	void showtargettext(string s, int type = 0);
	void activate(CCObject*);	//CallBack
	void setactivator(CCObject* target, SEL_MenuHandler selector);

	/*
		[state:-1]		m_iItemCount<物品使用的剩余数量
		[state_flag]	m_iMiniType<是否是小列表模式的标志位
	*/
	int m_iItemCount,m_iMiniType;			
	CCLabelBMFont* m_ic;

	/* [state:-1]<用于设置物品数量的接口 */
	void seticc(int i);
};

#endif
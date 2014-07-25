#ifndef __H_MENU_H__
#define __H_MENU_H__

/*
	<菜单层
		< 内部：RightUpgr - 右侧升级栏
		< 外部：LeftFm	  - 左侧菜单栏 [只负责管理]
*/

#include "cocos2d.h"
#include "hud\RightUpgr.h"
USING_NS_CC;

class HMenu : public CCLayer
{
public:
	HMenu();
	~HMenu();

	/*static */HMenu *sharedHMenu();
	//static void purgeSharedHMenu();

	void f_open();
	void f_pause();

	void showLeftFM();
	void dissmissLeftFM();

	bool mbShown;		// <是否处于显示内容的状态

	void showRightUpg(EChesses* aeTar);
	void dissmissRightUpg();

	void dissMiss();	// <通用关闭显示接口
	
	void	showSB();		// <miState = 2;
	void	DissMissSB();	// <消除胜败条件
	
private:
	/*static */HMenu* mSharedHMenu;
	RightUpgr* mRightUp;
	
public:
	int miState;			// < 0 - 左边菜单显示 || 1 - 显示升级界面 || 2 - 显示胜败条件
};

#endif
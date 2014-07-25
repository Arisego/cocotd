#ifndef __H_MENU_H__
#define __H_MENU_H__

/*
	<�˵���
		< �ڲ���RightUpgr - �Ҳ�������
		< �ⲿ��LeftFm	  - ���˵��� [ֻ�������]
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

	bool mbShown;		// <�Ƿ�����ʾ���ݵ�״̬

	void showRightUpg(EChesses* aeTar);
	void dissmissRightUpg();

	void dissMiss();	// <ͨ�ùر���ʾ�ӿ�
	
	void	showSB();		// <miState = 2;
	void	DissMissSB();	// <����ʤ������
	
private:
	/*static */HMenu* mSharedHMenu;
	RightUpgr* mRightUp;
	
public:
	int miState;			// < 0 - ��߲˵���ʾ || 1 - ��ʾ�������� || 2 - ��ʾʤ������
};

#endif
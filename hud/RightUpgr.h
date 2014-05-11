#ifndef __RIGHT_UPGR_H__
#define __RIGHT_UPGR_H__

/*
	< 右边升级栏
*/

#include "byeven\BYCocos.h"
#include "utils\EChess.h"

class RightUpgr : public BYLayerDescendant{
public:
	void f_init();
	void showUpgr(EChesses* aeTar);

private:
	vector<CCLabelBMFont*> mvLabels;
	EChesses* meTar;
	void showOver();

	void showHub();														// <如果无须继续显示则结束
	virtual void byTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	int miState;	// < [目前无逻辑] 0 - 未填充内容 | 1 - 显示了一个内容 | 2 - 内容等待回复[自主升级？]

	map<string,int> mUpvals;			// <升级数值对
	vector<map<string,string>> vdata;	//	<数据库中介 -- 攻击类型多是固定的，将不会被保存在数据库中。
};


#endif
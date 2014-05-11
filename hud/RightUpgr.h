#ifndef __RIGHT_UPGR_H__
#define __RIGHT_UPGR_H__

/*
	< �ұ�������
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

	void showHub();														// <������������ʾ�����
	virtual void byTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	int miState;	// < [Ŀǰ���߼�] 0 - δ������� | 1 - ��ʾ��һ������ | 2 - ���ݵȴ��ظ�[����������]

	map<string,int> mUpvals;			// <������ֵ��
	vector<map<string,string>> vdata;	//	<���ݿ��н� -- �������Ͷ��ǹ̶��ģ������ᱻ���������ݿ��С�
};


#endif
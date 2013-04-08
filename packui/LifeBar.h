#ifndef __LIFE_BAR_H__
#define __LIFE_BAR_H__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class LifeBar : public CCSprite 
{
public:
	LifeBar(string psz, int max);

	int max, val;
	float percentage;
	void setval(int a_ival);
	void revalue(int a_max);		//hp,mp���޷����䶯ʱ���õ���ʾ�������
	
private:
	CCProgressTimer* m_cpt;
	CCLabelBMFont* m_clbBase;
	CCLabelBMFont* m_clbVal;

	void RefreshView();

};

#endif
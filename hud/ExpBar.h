#ifndef __EXP_BAR_H__
#define __EXP_BAR_H__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class ExpBar : public CCSprite 
{
public:
	ExpBar(string psz, int max);
	ExpBar(const char* psbase,string psz, int m);
	int max, val;
	float percentage;
	void setval(int a_ival);
	void revalue(int a_max);		//hp,mp上限发生变动时调用的显示变更函数
	
private:
	CCProgressTimer* m_cpt;

	void RefreshView();

};

#endif
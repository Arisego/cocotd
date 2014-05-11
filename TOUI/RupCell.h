#ifndef __RUP_CELL_H__
#define __RUP_CELL_H__

#include "cocos2d.h"
#include "byeven\BYLayer.h"
USING_NS_CC;

class RupCell : public BYLayerDescendant
{
public:
	RupCell(int i);

	bool f_init();
	void showStr(const char* asName);

private:
	CCLabelBMFont* labelAtlas;
	
};

#endif
#ifndef __ELEMENT_TICKER_H__
#define __ELEMENT_TICKER_H__

#include "packui/Container.h"
//LinePrioer -- For BattleMap to decid the next Controlling Entile.
class ElementTicker : public BYLayerDescendant
{
public:
	bool f_init();
	void tick();
private:
	CCSprite* m_csClock;
	int m_iStep;
};


#endif
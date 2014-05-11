#ifndef __L_STAT_BAR_H__
#define __L_STAT_BAR_H__

#include "hud/StatBar.h"

//Left Status Bar
class LStatBar: public StatBar
{
public:
	virtual bool init();
	virtual void SetContent(EChesses* aec);

protected:
	virtual void setHead(const char* asH) override;
};


#endif
#ifndef __R_STAT_BAR_H__
#define __R_STAT_BAR_H__

#include "hud/StatBar.h"

//Right Status Bar
class RStatBar : public StatBar
{
public:
	virtual bool init();
	virtual void SetContent(EChesses* aec);

protected:
	virtual void setHead(const char* asH) override;
};


#endif
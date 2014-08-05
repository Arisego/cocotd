#ifndef __AB_NEXT_H__
#define __AB_NEXT_H__

#include "packui\Container.h"

class AbNext : public Container
{
private:
	CCSprite* msNextText;
	CCSprite* msNextTextHov;

public:
	AbNext(int aw, int ah);

	virtual void onHover() override;
	virtual void onSelect() override;
	virtual void onNormal() override;

	void Pause();
	void Resume();
};

#endif
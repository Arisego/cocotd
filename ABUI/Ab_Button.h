#ifndef __AB_BUTTON_H__
#define __AB_BUTTON_H__

/*
	<��ť
*/

#include "packui\Container.h"

class AbButton: public Container
{
public:
	AbButton(const char* aPth, int aw, int ah);

	virtual void onSelect() override;
};

#endif
#ifndef __HS_BUTTON_H__
#define __HS_BUTTON_H__

#include "packui\Container.h"
/*
	<HS_Button
	| < ֻ��Hover��Select��button
*/

class HSButton : public Container
{
public:
	HSButton(const char* aHover, const char* aSelect, int aw, int ah);
};

#endif
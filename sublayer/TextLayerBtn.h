#ifndef __TEXT_LAYER_BTN_H__
#define __TEXT_LAYER_BTN_H__

#include "packui/Container.h"

class TlBtn : public Container
{
private:

public:
	virtual void onNormal();
	virtual void onHover();
	virtual void onSelect();
	virtual void onDisable();

	//////////////////////////////////////////////////////////////////////////

	TlBtn();
	~TlBtn();


};

#endif
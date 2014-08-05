#ifndef __AB_EP_H__
#define __AB_EP_H__

/* 
	<AbEpisod
	< ÏÔÊ¾ÕÂ½ÚÓÃ
*/

#include "packui/Container.h"

class AbEpisod: public Container
{
public:
	int miEID;

	AbEpisod(int eid, int aw, int ah);
	virtual void onSelect() override;
};

#endif
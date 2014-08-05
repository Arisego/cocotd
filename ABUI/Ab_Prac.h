#ifndef __AB_PRAC_H__
#define __AB_PRAC_H__

/*  
	ABPrac
	< Á·Ï°
*/

#include "packui/Container.h"

class ABPrac: public Container
{
public:
	ABPrac(int cid, int aw, int ah);
	int miPracID;

	virtual void onSelect() override;
};

#endif
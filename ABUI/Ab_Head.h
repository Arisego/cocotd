#ifndef __AB_HEAD_H__
#define __AB_HEAD_H__

/*  
	ABHead
	< ��ߵ�ͷ����
*/

#include "packui/Container.h"

class ABHead: public Container
{
public:
	ABHead(int cid, int aw, int ah);
	int	miCID;	// CHara ID
};

#endif
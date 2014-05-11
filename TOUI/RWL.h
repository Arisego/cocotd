#ifndef __RWL_H__
#define __RWL_H__

#include "byeven/BYCocos.h"

class Rwl : public BYLayerDescendant{
private:
	float mfWidth, mfHeight;

public:
	Rwl(float aW, float aH);
};

#endif
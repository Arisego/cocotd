#ifndef __MAP_LAYER_COMP_H__
#define __MAP_LAYER_COMP_H__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class MapLayerComp : public CCComponent
{
public:
	MapLayerComp();
	void ActRetain();
	void ActRelease();

	void MlLock();
	void MlUnLock();

private:
	int miActCount;
	int miFLockType;

};


#endif
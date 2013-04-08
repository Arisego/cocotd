#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "cocos2d.h"

using namespace cocos2d;

class System
{
    
public:
    static void Init();
    static bool IsPad();
    static CCPoint PointMake(float x, float y);
    static float GetRealSize(float size);

private:
    static bool mIsPad;
    
};

#endif	// __SYSTEM_H__

#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "cocos2d.h"




class SplashScene : public cocos2d::CCLayer
{
private:
	void update(float dt);

public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    

    // implement the "static node()" method manually
    CREATE_FUNC(SplashScene);


	
};



#endif	// __TILEMAP_H__
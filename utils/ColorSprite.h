#ifndef __COLOR_SPRITE_H__
#define __COLOR_SPRITE_H__

/*
	<ColorSprite - 提供透明色功能
*/

#include "cocos2d.h"
USING_NS_CC;

class ColorSprite : public CCSprite  
{  
public:    
	static ColorSprite* create(const char* pszFileName);    
	static ColorSprite* createWithSpriteFrameName(const char* sprName);
	static ColorSprite* createWithSpriteFrame(CCSpriteFrame *pSpriteFrame);

	//virtual bool initWithTexture(CCTexture2D *pTexture);    
	virtual bool initWithSpriteFrame(CCSpriteFrame *pSpriteFrame);   
	virtual void draw(void);   
};  

#endif
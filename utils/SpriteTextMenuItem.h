#ifndef __SPRITETEXTMENUITEM_H__
#define __SPRITETEXTMENUITEM_H__



#include "cocos2d.h"


USING_NS_CC;



class SpriteTextMenuItem : public cocos2d::CCMenuItemSprite
{
private:
	static const unsigned int    kTextTag = 0xc0c05002;
public:
	
	void settext(const char *string, const char *fontName, float fontSize){		
		this->removeChildByTag(kTextTag);
		CCLabelTTF* label = CCLabelTTF::create(string, fontName, fontSize);	//CCLabelAtlas
		label->setAnchorPoint(ccp(0, 0));
		label->setTag(kTextTag);
		this->addChild(label);
	}


};

#endif
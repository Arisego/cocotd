#ifndef __SPRITETEXTMENUITEM_H__
#define __SPRITETEXTMENUITEM_H__



#include "cocos2d.h"


USING_NS_CC;



class SpriteTextMenuItem : public cocos2d::CCMenuItemSprite
{
private:
	static const unsigned int    kTextTag = 0xc0c05002;
	int		miData;	/* <»º´æÊý¾Ý */
public:

	void settext(const char *string, const char *fontName, float fontSize, int tx = 0, int ty=0){		
		this->removeChildByTag(kTextTag);
		CCLabelBMFont* label = CCLabelBMFont::create(string, fontName, fontSize);
		label->setAnchorPoint(ccp(0.5, 0.5));
		label->setTag(kTextTag);
		CCSize s = getContentSize();
		label->setPosition(s.width/2 + tx,s.height/2 + ty);
		label->setColor(ccBLACK);
		this->addChild(label);
	}

	int getSData(){
		return miData;
	}

	void setSData(int ai){
		miData = ai;
	}

};

#endif
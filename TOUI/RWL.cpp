#include "RWL.h"

#include "cocos-ext.h"
#include "RWLView.h"

USING_NS_CC;
USING_NS_CC_EXT;

Rwl::Rwl(float aW, float aH)
{
	mfWidth = aW;
	mfHeight = aH;

	CCScale9Sprite* nback = CCScale9Sprite::create("Images/config_back.png"); 
	nback->setContentSize(CCSize(mfWidth,mfHeight));
	nback->setAnchorPoint(ccp(0,0));
	nback->setOpacity(200);
	addChild(nback,-2);

	RWLView* tRwlView = new RWLView(mfWidth, mfHeight,0);
	tRwlView->setPosition(ccp(1,1));
	addChild(tRwlView);
}


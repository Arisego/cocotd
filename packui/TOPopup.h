#ifndef __TO_POPUP_H__
#define __TO_POPUP_H__

#include "cocos2d.h"
#include "ListDBView.h"
USING_NS_CC;

class TOPopup:public CCLayer
{
public:
	TOPopup(int width, int height);
	~TOPopup();

	void refresh_ldb(int tag);

	void ItemBack(CCObject* pSender);
private:
	ListDBView<ItemCell>* m_ldbEquList;
};


#endif
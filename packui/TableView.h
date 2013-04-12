#ifndef __TABLE_VIEW_H__
#define __TABLE_VIEW_H__


#include "cocos-ext.h"
#include "DBSource.h"

#include "packui/ItemCell.h"


USING_NS_CC_EXT;

//this TableView is for variable top bottom use only.
class TableView : public CCTableView
{
protected:
	CCTouch* lasttouch;

public:
	virtual void onExit(){
		unscheduleUpdate();
	}

	void f_init(){
		lasttouch = NULL;
		setTouchEnabled(false);
		scheduleUpdate();
	}

	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
		//if(pTouch != lasttouch) 
		//CCLOG("TableViewTouchMoved");
		lasttouch = pTouch;
		CCScrollView::ccTouchMoved(pTouch,pEvent);
		
	}

	virtual void update(float fDelta);
	
	virtual void updateCellAtIndex(unsigned int idx){
		CCTableView::updateCellAtIndex(idx);
		//CCLOG(">Idx Update:%d",idx);
	}

	int f_offsettoidx(CCPoint offset){
		int ret = -1;
		int index = 0;
		const int maxIdx = m_pDataSource->numberOfCellsInTableView(this)-1;

		const CCSize cellSize = m_pDataSource->cellSizeForTable(this);
		if (m_eVordering == kCCTableViewFillTopDown) {
			offset.y = this->getContainer()->getContentSize().height - offset.y - cellSize.height;
		}

		ret = this->__indexFromOffset(offset);
		index = MAX(0, ret);
		index = MIN(index, maxIdx);

		if(ret == index)
			return index;
		else
			return -1;
	}

};

#endif
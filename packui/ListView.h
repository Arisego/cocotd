#ifndef __LISTVIEW_H__
#define __LISTVIEW_H__


#include "cocos-ext.h"
#include "DBSource.h"


USING_NS_CC_EXT;

//this listview is for variable top bottom use only.
class ListView : public CCTableView
{
public:

	void _updateContentSize()
	{

		this->setContentSize(((DBSource*) m_pDataSource)->getwholesize());
		//CCLOG("Content Size:%f,%f",m_pDataSource->getwholesize().width,m_pDataSource->getwholesize().height);
	}

	CCPoint _offsetFromIndex(unsigned int index)
	{
		return CCPoint(0,((DBSource*) m_pDataSource)->offsetfromidx(index));
	}

	unsigned int _indexFromOffset(CCPoint offset)
	{
		return ((DBSource*) m_pDataSource)->indexfromoffset(offset.y);
	}

	void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
	{
		if (!this->isVisible()) {
			return;
		}
		if (m_pTouches->count() == 1 && !this->isTouchMoved()) {
			unsigned int        index;
			CCTableViewCell   *cell;
			CCPoint           point;

			point = this->getContainer()->convertTouchToNodeSpace(pTouch);

			index = this->_indexFromOffset(point);
			cell  = this->_cellWithIndex(index);

			if (cell) {
				m_pTableViewDelegate->tableCellTouched(this, cell);
			}
		}
		CCScrollView::ccTouchEnded(pTouch, pEvent);
		CCLOG("curr-->%f,%f",this->getContentOffset().x,this->getContentOffset().y);
	}

	// µ÷ÕûÏÔÊ¾
	void scrollViewDidScroll(CCScrollView* view)
	{
		unsigned int uCountOfItems = m_pDataSource->numberOfCellsInTableView(this);
		if (0 == uCountOfItems)
		{
			return;
		}

		unsigned int startIdx = 0, endIdx = 0, idx = 0, maxIdx = 0;
		CCPoint offset = ccpMult(this->getContentOffset(), -1);
		maxIdx = MAX(uCountOfItems-1, 0);
		

		if (m_eVordering == kCCTableViewFillTopDown)
		{
			offset.y = offset.y + m_tViewSize.height/this->getContainer()->getScaleY();
		}
		startIdx = this->_indexFromOffset(offset);

		if (m_eVordering == kCCTableViewFillTopDown)
		{
			offset.y -= m_tViewSize.height/this->getContainer()->getScaleY();
		}
		else 
		{
			offset.y += m_tViewSize.height/this->getContainer()->getScaleY();
		}
		offset.x += m_tViewSize.width/this->getContainer()->getScaleX();

		endIdx   = this->_indexFromOffset(offset);   

		CCLOG(">startid:%d,endid:%d",startIdx,endIdx);
		CCLOG(">Offset in DidScroll:%f,%f.",offset.x,offset.y);

		if (m_pCellsUsed->count() > 0)
		{
			CCTableViewCell* cell = (CCTableViewCell*)m_pCellsUsed->objectAtIndex(0);

			idx = cell->getIdx();
			while(idx <startIdx)
			{
				this->_moveCellOutOfSight(cell);
				if (m_pCellsUsed->count() > 0)
				{
					cell = (CCTableViewCell*)m_pCellsUsed->objectAtIndex(0);
					idx = cell->getIdx();    
				}
				else
				{
					break;
				}
			}
		}
		if (m_pCellsUsed->count() > 0)
		{
			CCTableViewCell *cell = (CCTableViewCell*)m_pCellsUsed->lastObject();
			idx = cell->getIdx();

			while(idx <= maxIdx && idx > endIdx)
			{
				this->_moveCellOutOfSight(cell);
				if (m_pCellsUsed->count() > 0)
				{
					cell = (CCTableViewCell*)m_pCellsUsed->lastObject();
					idx = cell->getIdx();

				}
				else
				{
					break;
				}
			}
		}

		for (unsigned int i=startIdx; i <= endIdx; i++)
		{
			//if ([m_pIndices containsIndex:i])
			if (m_pIndices->find(i) != m_pIndices->end())
			{
				continue;
			}
			this->updateCellAtIndex(i);
		}
	}

	//virtual void retain(){
	//	CCLOG("s");
	//}

};

#endif
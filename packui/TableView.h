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

private:
	CCScale9Sprite* pSb;	//ScrollBar
	float mfS_factor;

public:
	TableView(){
		pSb = NULL;
	}


	void f_generate_scrollbar(string a_sScrolBase = "Images/scrollbar"){
		//Generate the scrolling bar.
		CCSize cs = getContentSize();
		if(cs.height > m_tViewSize.height)		//只有容器小于内容时才需要
		{
			float t_sbbh = m_tViewSize.height - 20;	//滚动条区间长度

			mfS_factor = t_sbbh/cs.height;	
			float t_sblh = m_tViewSize.height * mfS_factor;
			CCNode* t_par = this->getParent();

			string ts;
			ts = a_sScrolBase;
			ts.append("_line.png");
			CCSprite* c9s = CCSprite::create(ts.c_str()); 
			c9s->setTextureRect(CCRectMake(0,0,2,t_sbbh));
			c9s->setAnchorPoint(ccp(0,0));
			c9s->setPosition(ccp(m_tViewSize.width - 4 , 10));
			t_par->addChild(c9s);

			CC_SAFE_RELEASE_NULL(pSb);
			ts = a_sScrolBase;
			ts.append("_normal.png");
			pSb = CCScale9Sprite::create(ts.c_str());
			pSb->setContentSize(CCSize(6,t_sblh));
			//pSb->setTextureRect(CCRectMake(0,0,6,t_sblh));
			pSb->setAnchorPoint(CCPointZero);
			pSb->setPosition(ccp(m_tViewSize.width-6,t_sbbh + 10 - t_sblh));

			t_par->addChild(pSb);
			return;
		}

		mfS_factor = 0;
	}


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

	// 调整显示
	virtual void scrollViewDidScroll(CCScrollView* view)
	{
		CCPoint offset = ccpMult(this->getContentOffset(), -1);

		if(pSb) pSb->setPosition(ccp(m_tViewSize.width-6,offset.y * mfS_factor + 10));

		CCTableView::scrollViewDidScroll(view);
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

	/* [IN] view id |Bring the cell to view. */
	void f_idx_sel(int idx,int atype = 0){
		do 
		{
			const CCSize cellSize = m_pDataSource->cellSizeForTable(this);
			float off_t;
			CCPoint offset = ccpMult(this->getContentOffset(), -1);

			off_t = this->_offsetFromIndex(idx).y;
			if (m_eVordering == kCCTableViewFillTopDown)
			{
				offset.y = offset.y + m_tViewSize.height/this->getContainer()->getScaleY() - cellSize.height;
			}
			if(off_t<offset.y){
				this->setContentOffset(ccp(offset.x,-off_t));
				break;
			}


			if (m_eVordering == kCCTableViewFillTopDown)
			{
				offset.y -= m_tViewSize.height/this->getContainer()->getScaleY();
			}
			else 
			{
				offset.y += m_tViewSize.height/this->getContainer()->getScaleY();
			}
			if(off_t - cellSize.height>offset.y){
				this->setContentOffset(ccp(offset.x,-off_t));
				break;
			}
		} while (1);
		CCTableViewCell *cell  = this->cellAtIndex(idx);

		if (cell) {
			m_pTableViewDelegate->tableCellTouched(this, cell);
		}




	}
};

#endif
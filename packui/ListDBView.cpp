#include "ListDBView.h"

void ListDBView::scrolldis(float dis){
	pTableView->setContentOffset(ccp(0,pTableView->getContentOffset().y + dis));
}

void ListDBView::setoffset(CCPoint offset){
	pTableView->setContentOffset(ccp(offset.x,offset.y + height - pTableView->getContainer()->getContentSize().height));
}

bool ListDBView::init()
{
	bool bRet = false;
	do
	{
		//width = 400;
		//height = 300;

		CC_BREAK_IF( !CCLayer::init() );
		CCSize winSize = CCDirector::sharedDirector()->getVisibleSize();
		CC_BREAK_IF(!init_data());
		pTableView = new TableView();
		pTableView->autorelease();
		pTableView->setDataSource(this);
		pTableView->initWithViewSize(CCSizeMake(width, height));
		pTableView->setDirection(kCCScrollViewDirectionVertical);
		pTableView->setPosition(CCPointZero);
		pTableView->setDelegate(this);
		pTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
		pTableView->f_init();
		this->addChild(pTableView);
		pTableView->reloadData();
		//pTableView->setContentOffset(ccp(0,height - pTableView->getContainer()->getContentSize().height));
		setoffset(ccp(0,0));
		pTableView->setBounceable(false);
		EventCenter::sharedEventCenter()->setScroller(this);



		bRet = true;
	}while(0);

	return bRet;
}


void ListDBView::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	//CCLOG("cell touched at index: %i", cell->getIdx());
	cellselect(cell->getIdx());
	activate(cell);
}



CCSize ListDBView::cellSizeForTable(CCTableView *table)
{
	return CCSizeMake(cWidth, cHeight);
}

//[]Read Cell out from data(CCArray of CCLabelTTF)
// Error Raise here always result from a null data read in init.
CCTableViewCell* ListDBView::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	//CCString *pString = CCString::createWithFormat("%d", idx);

	CCTableViewCell *pCell = new CCTableViewCell();

	pCell->autorelease();
	ItemCell *pLabel = (ItemCell*) data->objectAtIndex(idx);
	pLabel->setPosition(CCPointZero);
	pLabel->setAnchorPoint(CCPointZero);
	pCell->setTag(pLabel->getTag());
	
	//pLabel->setTag(123);

	if(pLabel->getParent() != NULL)
		return (CCTableViewCell *) pLabel->getParent();
	else{
		//pLabel->retain();
		pCell->addChild(pLabel);
	}
		



	return pCell;
}



void ListDBView::scrollViewDidScroll(CCScrollView *view)
{
}

void ListDBView::scrollViewDidZoom(CCScrollView *view)
{
}
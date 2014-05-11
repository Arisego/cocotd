#include "RWLView.h"


USING_NS_CC;
USING_NS_CC_EXT;

void RWLView::scrolldis(float dis){
	pTableView->setContentOffset(ccp(0,pTableView->getContentOffset().y + dis));
}

void RWLView::setoffset(CCPoint offset){
	pTableView->setContentOffset(ccp(offset.x,offset.y + height - pTableView->getContainer()->getContentSize().height));
}

bool RWLView::init()
{
	bool bRet = false;
	do
	{
		//width = 400;
		//height = 300;

		CC_BREAK_IF( !CCLayer::init() );
		CCSize winSize = CCDirector::sharedDirector()->getVisibleSize();

		// <数据初始化
		init_data(width - 8);
		pTableView = new ListView();
		pTableView->setDataSource(this);
		pTableView->initWithViewSize(CCSizeMake(width-8, height));
		pTableView->setDirection(kCCScrollViewDirectionVertical);
		pTableView->setPosition(CCPointZero);
		pTableView->setDelegate(this);
		pTableView->setVerticalFillOrder(kCCTableViewFillTopDown);
		this->addChild(pTableView);
		pTableView->reloadData();
		//pTableView->setContentOffset(ccp(0,height - pTableView->getContainer()->getContentSize().height));
		setoffset(ccp(0,0));
		pTableView->setBounceable(false);
		EventCenter::sharedEventCenter()->setScroller(this);

		pTableView->f_generate_scrollbar();

		bRet = true;
	}while(0);

	return bRet;
}


//[EndPoint] Which Cell is clickced. Of no use for RWLView.
void RWLView::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
	CCLOG("cell touched at index: %i", cell->getIdx());
}


//[NOT USED]
CCSize RWLView::cellSizeForTable(CCTableView *table)
{
	return CCSizeMake(800, 1);
}

//[]Read Cell out from data(CCArray of CCLabelTTF)
CCTableViewCell* RWLView::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
	//CCString *pString = CCString::createWithFormat("%d", idx);

	CCTableViewCell *pCell = new CCTableViewCell();

	pCell->autorelease();
	CCLabelTTF *pLabel = (CCLabelTTF*) data->objectAtIndex(idx);
	pLabel->setPosition(CCPointZero);
	pLabel->setAnchorPoint(CCPointZero);
	pLabel->setTag(123);

	if(pLabel->getParent() != NULL)
		return (CCTableViewCell *) pLabel->getParent();
	else
		pCell->addChild(pLabel);



	return pCell;
}



void RWLView::scrollViewDidScroll(CCScrollView *view)
{
	CCPoint cs = pTableView->getContentOffset();
}

void RWLView::scrollViewDidZoom(CCScrollView *view)
{
}

RWLView::~RWLView()
{

	EventCenter::sharedEventCenter()->setScroller(NULL);
	pTableView->removeFromParent();

	CC_SAFE_RELEASE_NULL(pTableView);
	removeAllChildren();
}

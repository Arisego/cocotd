#include "TableView.h"

#include "packui/ListDBView.h"

void TableView::update(float fDelta)
{
	CCTableView::update(fDelta);

	//static float fds = 0;

	//[TVMes] 
	CCLOG("[TVMes]>Update being called...");
	if(lasttouch) {
		CCPoint point = this->getContainer()->convertTouchToNodeSpace(lasttouch);
		if (m_eVordering == kCCTableViewFillTopDown) {
			CCSize cellSize = m_pDataSource->cellSizeForTable(this);
			point.y -= cellSize.height;
		}
		//[TVMes] CCLOG("[TVMes]>Update|Calculate idx...");
		int index = f_offsettoidx(point);
		//[TVMes] CCLOG("[TVMes]>Update|idx:%d...",index);
		if(index>-1)
			((ListDBSource<>*) m_pDataSource)->cellhover(index);
		//fds = 0;
	}/*else{
		fds += fDelta;
		if(fds>1) 	
			((ListDBSource*) m_pDataSource)->cellhover(-1);
	}
	lasttouch = NULL;*/
}

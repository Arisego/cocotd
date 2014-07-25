#ifndef __R_WL_VIEW_H__
#define __R_WL_VIEW_H__

/*
	<胜败条件文本列表[ListView]
*/

#include "cocos2d.h"
#include "cocos-ext.h"

#include "packui/ListView.h"

#include "utils/Controller.h"
#include "EventCenter.h"
#include "RWLViewDBSource.h"

class RWLView : public cocos2d::CCLayer, public RWLViewDBSource, public cocos2d::extension::CCTableViewDelegate, public Scroller
{
private:
	float width,height;
	ListView* pTableView;


	int m_iTag;

public:
	~RWLView();

	void scroll_in(WPARAM wParam, LPARAM lParam){
		float zDelta = (short) HIWORD(wParam);    // wheel rotation
		scrolldis(-zDelta);
	}
	//[IN] 构造
	RWLView(float w,float h,int t){
		width = w;
		height = h;
		m_iTag = t;
		miSBType = m_iTag;
		init();
	}
	//[IN] 滚动相应的距离
	void scrolldis(float dis);
	//[IN] 设置偏移量，ccp(0,y);
	virtual void setoffset(CCPoint offset);

	virtual bool init();  

	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

	virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);

	//处理触摸事件
	virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
	//每一项的宽度和高度
	virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);
	//生成列表每一项的内容
	virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	////一共多少项
	//virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);

};

#endif
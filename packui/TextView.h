#ifndef __TABLEVIEWTESTSCENE_H__
#define __TABLEVIEWTESTSCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "DBSource.h"
#include "ListView.h"

#include "utils/Controller.h"
#include "EventCenter.h"

//TextView 作为ListView和DBsource的容器被使用,作为最外层的封装和数据源连接器。
//关于 pTableView->setContentOffset || 容器高度 - 内容高度 为顶端显示，亦即起点是基于容器左下角的 <<已封装 setoffset();
class TextView : public cocos2d::CCLayer, public DBSource, public cocos2d::extension::CCTableViewDelegate, public Scroller
{
private:
	float width,height;
	ListView* pTableView;
	int m_iTag;

public:
	~TextView();

	void scroll_in(WPARAM wParam, LPARAM lParam){
		//fwKeys = LOWORD(wParam);    // key flags
		float zDelta = (short) HIWORD(wParam);    // wheel rotation
		//int xPos = GET_X_LPARAM(lParam);		//Mouse_Wheel的坐标是基于屏幕左上的，必须使用mouse_move_in事件来进行控制分离。
		//int yPos = GET_Y_LPARAM(lParam);
		//CCLOG("Point,%d,%d",xPos,yPos);
		//if(CCRect::CCRectContainsPoint(m_rContent,ccp(xPos,CCDirector::sharedDirector()->getVisibleSize().height - yPos))){		//director.getheight = 600;modify it if dix change.
		//	CCLOG("a in.");
		//}
		scrolldis(-zDelta);
	}
	//[IN] 构造
	TextView(float w,float h,int t){
		width = w;
		height = h;
		m_iTag = t;

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

	//CREATE_FUNC(TextView);
};

#endif 
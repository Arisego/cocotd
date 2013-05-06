#ifndef __TABLEVIEWTESTSCENE_H__
#define __TABLEVIEWTESTSCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "DBSource.h"
#include "ListView.h"

#include "utils/Controller.h"
#include "EventCenter.h"

//TextView ��ΪListView��DBsource��������ʹ��,��Ϊ�����ķ�װ������Դ��������
//���� pTableView->setContentOffset || �����߶� - ���ݸ߶� Ϊ������ʾ���༴����ǻ����������½ǵ� <<�ѷ�װ setoffset();
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
		//int xPos = GET_X_LPARAM(lParam);		//Mouse_Wheel�������ǻ�����Ļ���ϵģ�����ʹ��mouse_move_in�¼������п��Ʒ��롣
		//int yPos = GET_Y_LPARAM(lParam);
		//CCLOG("Point,%d,%d",xPos,yPos);
		//if(CCRect::CCRectContainsPoint(m_rContent,ccp(xPos,CCDirector::sharedDirector()->getVisibleSize().height - yPos))){		//director.getheight = 600;modify it if dix change.
		//	CCLOG("a in.");
		//}
		scrolldis(-zDelta);
	}
	//[IN] ����
	TextView(float w,float h,int t){
		width = w;
		height = h;
		m_iTag = t;

		init();
	}
	//[IN] ������Ӧ�ľ���
	void scrolldis(float dis);
	//[IN] ����ƫ������ccp(0,y);
	virtual void setoffset(CCPoint offset);

	virtual bool init();  

	virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

	virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);

	//�������¼�
	virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
	//ÿһ��Ŀ�Ⱥ͸߶�
	virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);
	//�����б�ÿһ�������
	virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	////һ��������
	//virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);

	//CREATE_FUNC(TextView);
};

#endif 
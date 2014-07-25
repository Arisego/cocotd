#ifndef __R_WL_VIEW_H__
#define __R_WL_VIEW_H__

/*
	<ʤ�������ı��б�[ListView]
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
	//[IN] ����
	RWLView(float w,float h,int t){
		width = w;
		height = h;
		m_iTag = t;
		miSBType = m_iTag;
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

};

#endif
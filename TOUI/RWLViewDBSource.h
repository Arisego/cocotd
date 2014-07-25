#ifndef __RWL_VIEW_DBSOURCE_H__
#define __RWL_VIEW_DBSOURCE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DBUtil.h"
#include "StateCenter.h"
#include "Macros.h"
#include "LogicTree.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class RWLViewDBSource : public cocos2d::extension::CCTableViewDataSource
{

	float rw;
	int m_number;

public:
	~RWLViewDBSource();
	RWLViewDBSource();

	CCArray *data;
	vector<map<string,string>> vdata;
	vector<float> m_plength;
	float m_height;

	virtual void init_data(float width);

	virtual unsigned int numberOfCellsInTableView(CCTableView *table);

	virtual CCSize getwholesize();

	virtual  float offsetfromidx(int idx);
	virtual int indexfromoffset(float hei);

	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);

private:
	void ReadLgCell(std::string asHead, LogicCell* algc);
public:
	int miSBType;	/* <胜利条件或者失败条件 */
};

#endif
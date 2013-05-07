#ifndef __LIST_DBSOURCE_H__
#define __LIST_DBSOURCE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DBUtil.h"
#include "StateCenter.h"

#include "packui/ItemCell.h"
#include "packui/TableView.h"
#include "utils/Controller.h"
#include "EventCenter.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class ListDBSource : public cocos2d::extension::CCTableViewDataSource
{
protected:
	map<int,map<string,string>> m_missDbData;		//Data Detail Read From DB.
	CCDictionary* m_caStData;							//Data Counts Read From SC.
	CCArray *data;									//Array for DataSource
	
	int m_iNumber;
	float rw,cHeight,cWidth;

	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;

public:
	int m_ICType;									// 0 --> view of Item | 1 --> view of Skill | 2 --> view of Equip
	string m_sSql;									//Query String.

	~ListDBSource(){
		if(m_ICType == 1) {
			CC_SAFE_RELEASE_NULL(m_caStData);
		}
		m_miiViDb.clear();

		CCObject* tco;
		CCARRAY_FOREACH(data,tco){
			((Container*) tco)->onExit();
		}
		CC_SAFE_RELEASE_NULL(data);					//Do not Release m_caStData, they are maintance by StateCenter.
	}

	ListDBSource(){
		moto = -1;
		smoto= -1;
		data = NULL;
		
	}

	bool init_data(){
		bool ret = false;

		CC_SAFE_RELEASE_NULL(data);
		vdata.clear();

		DBUtil::initDB("save.db");
		vdata = DBUtil::getDataInfo(m_sSql,NULL);
		

		int m_number = vdata.size();
		
		//////////////////////////////////////////////////////////////////////////
		//
		DBUtil::closeDB(); 
		if(0 == m_number){
			CCLOG("ERROR: Reading a null result from database. Exit in debug.");
			return ret;//exit(0x5004);		//ERROR: Reading a null result from database. Exit in debug.
		}
		//
		m_iNumber = m_caStData->count();			//TODO:Firstly read it from SC please.
		data = CCArray::createWithCapacity(m_iNumber);
		for(int i = 0;i<m_number;i++){
			map<string,string> t_ssm = (map<string,string>) vdata.at(i);
			int item_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
			m_miiViDb[item_id] = i;							//方便查找？

			CCLOG(">Read for item id:%d.", item_id);

			string s = t_ssm.at("name");
			CCLOG(">Item Name:%s.",s.c_str());
			
			ItemCellData* ticd = (ItemCellData*) m_caStData->objectForKey(item_id);
			ticd->name		 = s;
			ticd->icon_mask	 = t_ssm.at("icon");

			ItemCell* tic = new ItemCell(cWidth, cHeight, m_ICType, ticd);
			tic->setTag(item_id);
			//tic->autorelease();
				//CCLabelTTF* tlt = CCLabelTTF::create(s.c_str(), "fonts/STHUPO.TTF", 24,CCSize(rw,0), kCCTextAlignmentLeft);
				//tlt->retain();

			data->addObject(tic);		
		}
		
		data->retain();
		ret = true;
		return ret;
		
	}

	virtual void RefreshSingleItem(int id, int value){
		
		int ti = m_miiViDb[id];		//Find the cell real id.
		ItemCell* tci = (ItemCell*) data->objectAtIndex(ti);
		//ItemCellData* ticd = (ItemCellData*) m_caStData->objectForKey(id);
		CCLOG(">Change the cid:%d,%d.", id, value);
		//ticd->sum = value;
		tci->f_setsum(value);
	}

	virtual void SetSelect(int id){
		int ti = m_miiViDb[id];		//Find the cell real id.
		//ItemCell* tci = (ItemCell*) data->objectAtIndex(ti);
		cellselect(ti);
	}

	string getdiscrip(int ti){
		int i = m_miiViDb[ti];
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		return t_ssm.at("discription");
	}

	string getval(string name,int ti){
		int i = m_miiViDb[ti];
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		return t_ssm.at(name);
	}

	unsigned int numberOfCellsInTableView(CCTableView *table){
		return m_iNumber;
	}

	int moto;		//不适用Static
	void cellhover(int idx){
		if(moto != idx){
			if(moto>-1) 
				((ItemCell*) data->objectAtIndex(moto))->onNormal();
			moto = idx;
			if(moto>-1)
				((ItemCell*) data->objectAtIndex(moto))->onHover();
		}
	}

	int smoto;
	void cellselect(int idx){
		if(smoto != idx){
			if(smoto>-1) 
				((ItemCell*) data->objectAtIndex(smoto))->setNormal();
			smoto = idx;
			((ItemCell*) data->objectAtIndex(smoto))->onSelect();
		}
	}

};

class ListDBView : public cocos2d::CCLayer, public ListDBSource, public cocos2d::extension::CCTableViewDelegate, public Scroller
{
private:
	float width,height;
	TableView* pTableView;
	//int m_iTag;				//tag is for group id ? < sqlstring 

protected:
	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	bool m_bIsEnabled;

	void activate(CCObject* aci){
		if (m_bIsEnabled)
		{
			if (m_pListener && m_pfnSelector)
			{
				(m_pListener->*m_pfnSelector)(aci);
			}
		}
	}

public:
	~ListDBView(){
		EventCenter::sharedEventCenter()->setScroller(NULL);
		pTableView->removeFromParent();
//		removeAllChildren();
//		data->removeAllObjects();
//		CC_SAFE_RELEASE_NULL(data);
	}

	void scroll_in(WPARAM wParam, LPARAM lParam){
		float zDelta = (short) HIWORD(wParam);    // wheel rotation
		scrolldis(-zDelta);
	}
	//[IN] 构造
	ListDBView(float w,float h, string s, CCDictionary* a_ca, CCObject* target, SEL_MenuHandler selector,int tid = 0){
		width = w;
		height = h;

		//m_iTag = t;
		m_sSql = s;

		cWidth = width;
		cHeight = 25;
		m_caStData = a_ca;

		m_pListener = target;
		m_pfnSelector = selector;
		m_bIsEnabled = true;

		m_ICType = tid;

		//init();
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
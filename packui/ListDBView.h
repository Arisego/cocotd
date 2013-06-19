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

template <class T = ItemCell> class ListDBSource : public cocos2d::extension::CCTableViewDataSource
{
protected:
	map<int,map<string,string>> m_missDbData;		//Data Detail Read From DB.
	CCDictionary* m_caStData;							//Data Counts Read From SC.
	CCArray *data;									//Array for DataSource
	
	int m_iNumber;
	float rw,cHeight,cWidth;

	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				//map item_id to db_idx
	map<int,int> m_miiViDi;				//map view_id to item_id
	map<int,int> m_miiViDs;				//map sour_id to view_id

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
			m_miiViDb[item_id] = i;							//方便查找

			CCLOG(">Read for item id:%d.", item_id);
		}
		
		if(m_number<1) ret = false;	//Something is wrong.
		else {
			ret = true;

			int item_db_idx;
			int t_ci = 0;
			ItemCellData* ticd = NULL;
			CCDictElement* ticde = NULL;
			CCDICT_FOREACH(m_caStData,ticde){
				ticd = (ItemCellData*) ticde->getObject();

				item_db_idx = m_miiViDb[ticd->type_id];
				map<string,string> t_ssm = (map<string,string>) vdata.at(item_db_idx);
				m_miiViDi[t_ci] = ticd->type_id;
				m_miiViDs[ticde->getIntKey()] = t_ci;

				string s = t_ssm.at("name");
				CCLOG(">Item Name:%s.",s.c_str());
					
				ticd->name		 = s;
				ticd->icon_mask	 = t_ssm.at("icon");

				T* tic = new T(cWidth, cHeight, m_ICType, ticd);
				tic->setTag(ticde->getIntKey());
				//tic->autorelease();
					//CCLabelTTF* tlt = CCLabelTTF::create(s.c_str(), "fonts/STHUPO.TTF", 24,CCSize(rw,0), kCCTextAlignmentLeft);
					//tlt->retain();
				++t_ci;
				data->addObject(tic);		
			}
		}
		data->retain();
		
		return ret;
		
	}

	/* [IN] view_id?src_id please. */
	virtual void RefreshSingleItem(int id, int value){
		//int ti = id;
		int ti = m_miiViDs[id];		//Find the cell real id.
		T* tci = (T*) data->objectAtIndex(ti);
		//ItemCellData* ticd = (ItemCellData*) m_caStData->objectForKey(id);
		CCLOG(">Change the cid:%d,%d.", id, value);
		//ticd->sum = value;
		tci->f_setsum(value);
	}

	/* [IN] view_id?src_id please */
	virtual void SetSelect(int id){
		//int ti = id;
		int ti = m_miiViDs[id];		//Find the cell real id.
		//T* tci = (T*) data->objectAtIndex(ti);
		cellselect(ti);
	}

	/* [IN] src_id */
	string getdiscrip(int ti){
		int i = m_miiViDs[ti];
		i = m_miiViDi[i];
		i  = m_miiViDb[i];
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		return t_ssm.at("discription");
	}

	string getval(string name,int ti){
		int i = m_miiViDs[ti];
		i = m_miiViDi[i];
		i  = m_miiViDb[i];
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		return t_ssm.at(name);
	}

	unsigned int numberOfCellsInTableView(CCTableView *table){
		return m_iNumber;
	}

	int moto;		//不适用Static
	void cellhover(int idx){
		if(moto != idx){
			if(moto>-1) {
				((T*) data->objectAtIndex(moto))->onNormal();
				//((T*) data->objectAtIndex(moto))->setNormal();
			}
			moto = idx;
			if(moto>-1)
				((T*) data->objectAtIndex(moto))->onHover();
		}
	}

	int smoto;
	void cellselect(int idx){
		if(smoto != idx){
			if(smoto>-1) {
				((T*) data->objectAtIndex(smoto))->onNormal();
				((T*) data->objectAtIndex(smoto))->setNormal();
			}
			smoto = idx;
			if(smoto>-1)
				((T*) data->objectAtIndex(smoto))->onSelect();
		}
	}

	void cellnormal(int idx){
		((T*) data->objectAtIndex(smoto))->setNormal();
		((T*) data->objectAtIndex(idx))->onNormal();
	}

};


/*
	ListDBView
	<用于列表显示的页面
	<目前[装备]物品列表的主键不再是item_id，在使用时注意区分，因为item_id究竟是不是在作为主键并不重要，关心item_id的类需要修改为自己去读取item_idB
	<ListDBView回调的CCTableViewCell里面有tag为1234的ItemCell，可以用于读取详细状态
	<ListDBView的sqlMask发生了更改，如果出现空列表参照ToPopup进行修改。
	<使用listdbviews
*/
template <class T = ItemCell> class ListDBView : public cocos2d::CCLayer, public ListDBSource<T>, public cocos2d::extension::CCTableViewDelegate, public Scroller
{
private:
	float width,height;
	TableView* pTableView;
	string ms_ScrollBase;
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

	ListDBView()
	{
		pTableView = NULL;
	}

	ListDBView( float w,float h, const char* s, CCDictionary* a_ca, CCObject* target, SEL_MenuHandler selector,int tid = 0,const char* sc ="Images/scrollbar" )
	{
		{
			width = w;
			height = h;

			//m_iTag = t;
			m_sSql = s;
			ms_ScrollBase = sc;

			cWidth = width;
			cHeight = 25;
			m_caStData = a_ca;

			m_pListener = target;
			m_pfnSelector = selector;
			m_bIsEnabled = true;

			m_ICType = tid;

			pTableView = NULL;
			//init();
		}
	}

	~ListDBView()
	{
		EventCenter::sharedEventCenter()->setScroller(NULL);
		if(pTableView) pTableView->removeFromParent();
	}


	void scrolldis(float dis){
		pTableView->setContentOffset(ccp(0,pTableView->getContentOffset().y + dis));
	}

	void setoffset(CCPoint offset){
		pTableView->setContentOffset(ccp(offset.x,offset.y + height - pTableView->getContainer()->getContentSize().height));
	}

	bool init()
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

			pTableView->f_generate_scrollbar(ms_ScrollBase);

			bRet = true;
		}while(0);

		return bRet;
	}


	void tableCellTouched(CCTableView* table, CCTableViewCell* cell)
	{
		//CCLOG("cell touched at index: %i", cell->getIdx());
		cellselect(cell->getIdx());
		activate(cell);
	}



	CCSize cellSizeForTable(CCTableView *table)
	{
		return CCSizeMake(cWidth, cHeight);
	}

	//[]Read Cell out from data(CCArray of CCLabelTTF)
	// Error Raise here always result from a null data read in init.
	CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx)
	{
		//CCString *pString = CCString::createWithFormat("%d", idx);

		CCTableViewCell *pCell = new CCTableViewCell();

		pCell->autorelease();
		T *pLabel = (T*) data->objectAtIndex(idx);
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



	void scrollViewDidScroll(CCScrollView *view)
	{
	}

	void scrollViewDidZoom(CCScrollView *view)
	{
	}

	void scroll_in(WPARAM wParam, LPARAM lParam){
		float zDelta = (short) HIWORD(wParam);    // wheel rotation
		scrolldis(-zDelta);
	}

	/* [IN] view_id please. */
	void f_swithc_id(int aid, int atype){
		aid = min(aid,m_iNumber-1);
		pTableView->f_idx_sel(aid,atype);
	}

	int f_get_viewid(int aid)
	{
		if(aid>=0)
			return min(m_miiViDs[aid],m_iNumber-1);
		else
			return aid;
	}

	//~ListDBView();
	//ListDBView();


	////[IN] 构造
	//// ListDBView(float w,float h, string s, CCDictionary* a_ca, CCObject* target, SEL_MenuHandler selector,int tid = 0);
	////[IN] 滚动相应的距离
	//void scrolldis(float dis);
	////[IN] 设置偏移量，ccp(0,y);
	//virtual void setoffset(CCPoint offset);

	//virtual bool init();  

	//virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

	//virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);

	////处理触摸事件
	//virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
	////每一项的宽度和高度
	//virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);
	////生成列表每一项的内容
	//virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	////一共多少项
	//virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);

};


#endif
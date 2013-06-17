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
			m_miiViDb[item_id] = i;							//�������

			CCLOG(">Read for item id:%d.", item_id);
		}
		
		if(m_number<1) ret = false;	//Something is wrong.
		else {
			ret = true;

			int item_id;
			ItemCellData* ticd = NULL;
			CCDictElement* ticde = NULL;
			CCDICT_FOREACH(m_caStData,ticde){
				ticd = (ItemCellData*) ticde->getObject();

				item_id = m_miiViDb[ticd->type_id];
				map<string,string> t_ssm = (map<string,string>) vdata.at(item_id);
				string s = t_ssm.at("name");
				CCLOG(">Item Name:%s.",s.c_str());
					
				ticd->name		 = s;
				ticd->icon_mask	 = t_ssm.at("icon");

				T* tic = new T(cWidth, cHeight, m_ICType, ticd);
				tic->setTag(ticde->getIntKey());
				//tic->autorelease();
					//CCLabelTTF* tlt = CCLabelTTF::create(s.c_str(), "fonts/STHUPO.TTF", 24,CCSize(rw,0), kCCTextAlignmentLeft);
					//tlt->retain();

				data->addObject(tic);		
			}
		}
		data->retain();
		
		return ret;
		
	}

	virtual void RefreshSingleItem(int id, int value){
		
		int ti = m_miiViDb[id];		//Find the cell real id.
		T* tci = (T*) data->objectAtIndex(ti);
		//ItemCellData* ticd = (ItemCellData*) m_caStData->objectForKey(id);
		CCLOG(">Change the cid:%d,%d.", id, value);
		//ticd->sum = value;
		tci->f_setsum(value);
	}

	virtual void SetSelect(int id){
		int ti = m_miiViDb[id];		//Find the cell real id.
		//T* tci = (T*) data->objectAtIndex(ti);
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

	int moto;		//������Static
	void cellhover(int idx){
		if(moto != idx){
			if(moto>-1) 
				((T*) data->objectAtIndex(moto))->onNormal();
			moto = idx;
			if(moto>-1)
				((T*) data->objectAtIndex(moto))->onHover();
		}
	}

	int smoto;
	void cellselect(int idx){
		if(smoto != idx){
			if(smoto>-1) 
				((T*) data->objectAtIndex(smoto))->setNormal();
			smoto = idx;
			((T*) data->objectAtIndex(smoto))->onSelect();
		}
	}

};


/*
	ListDBView
	<�����б���ʾ��ҳ��
	<Ŀǰ[װ��]��Ʒ�б������������item_id����ʹ��ʱע�����֣���Ϊitem_id�����ǲ�������Ϊ����������Ҫ������item_id������Ҫ�޸�Ϊ�Լ�ȥ��ȡitem_idB
	<ListDBView�ص���CCTableViewCell������tagΪ1234��ItemCell���������ڶ�ȡ��ϸ״̬
	<ListDBView��sqlMask�����˸��ģ�������ֿ��б����ToPopup�����޸ġ�
*/
template <class T = ItemCell> class ListDBView : public cocos2d::CCLayer, public ListDBSource<T>, public cocos2d::extension::CCTableViewDelegate, public Scroller
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

	ListDBView()
	{
		pTableView = NULL;
	}

	ListDBView( float w,float h, const char* s, CCDictionary* a_ca, CCObject* target, SEL_MenuHandler selector,int tid = 0 )
	{
		{
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

			pTableView->f_generate_scrollbar();

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

	//~ListDBView();
	//ListDBView();


	////[IN] ����
	//// ListDBView(float w,float h, string s, CCDictionary* a_ca, CCObject* target, SEL_MenuHandler selector,int tid = 0);
	////[IN] ������Ӧ�ľ���
	//void scrolldis(float dis);
	////[IN] ����ƫ������ccp(0,y);
	//virtual void setoffset(CCPoint offset);

	//virtual bool init();  

	//virtual void scrollViewDidScroll(cocos2d::extension::CCScrollView* view);

	//virtual void scrollViewDidZoom(cocos2d::extension::CCScrollView* view);

	////�������¼�
	//virtual void tableCellTouched(cocos2d::extension::CCTableView* table, cocos2d::extension::CCTableViewCell* cell);
	////ÿһ��Ŀ�Ⱥ͸߶�
	//virtual cocos2d::CCSize cellSizeForTable(cocos2d::extension::CCTableView *table);
	////�����б�ÿһ�������
	//virtual cocos2d::extension::CCTableViewCell* tableCellAtIndex(cocos2d::extension::CCTableView *table, unsigned int idx);
	////һ��������
	//virtual unsigned int numberOfCellsInTableView(cocos2d::extension::CCTableView *table);

};


#endif
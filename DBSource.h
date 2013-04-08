#ifndef __DBSOURCE_H__
#define __DBSOURCE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "DBUtil.h"
#include "StateCenter.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;

class DBSource : public cocos2d::extension::CCTableViewDataSource
{
	
	float rw;
	int m_number;

public:
	~DBSource(){
		data->release();
	}

	DBSource(){
		data = NULL;
	}

	CCArray *data;
	vector<map<string,string>> vdata;
	vector<float> m_plength;
	float m_height;

	void init_data(float width, int o_iid){
		char * errMsg = NULL;
		std::string sqlstr;
		vdata.clear();
		m_plength.clear();
		m_height = 0;
		CC_SAFE_RELEASE_NULL(data);

		rw = width;
		DBUtil::initDB("save.db"); 

		sqlstr = CCString::createWithFormat("select content,lockst from infor_table where infoid = %d ORDER BY lineid",o_iid)->getCString();
		
		vdata = DBUtil::getDataInfo(sqlstr,NULL);
		m_number = vdata.size();
		data = CCArray::createWithCapacity(m_number);
		int t_c = 0;

		for(int i = 0;i<m_number;i++){
			map<string,string> t_ssm = (map<string,string>) vdata.at(i);
			string t_slockst = t_ssm.at("lockst");
			CCLOG(">Lockst:%s.",t_slockst.c_str());
			if(t_slockst.size() == 1 || StateCenter::sharedStateCenter()->g_lock_query(t_slockst)){

				string s = t_ssm.at("content");
				CCLOG(">Content:%s.",s.c_str());
				CCLabelTTF* tlt = CCLabelTTF::create(s.c_str(), "fonts/STHUPO.TTF", 24,CCSize(rw,0), kCCTextAlignmentLeft);
				tlt->retain();

				m_plength.push_back(tlt->getContentSize().height);
				m_height += tlt->getContentSize().height;
				data->addObject(tlt);		
			}
		}
		m_number = data->count();
		data->retain();
		DBUtil::closeDB(); 
	}

	unsigned int numberOfCellsInTableView(CCTableView *table){
		return m_number;
	}

	CCSize getwholesize(){
		return CCSize(rw,m_height);
	}

	float offsetfromidx(int idx){
		float th = 0.0;
		for(int i=0;i<=idx;i++){
			th += m_plength[i];
		}

		return m_height - th;
	}

	int indexfromoffset(float hei){	//input the origin from tableview,do not modify it in tableview again;
		float dxh = m_height - hei;

		int idx = 0;
		int i=0;
		for(;i<m_number;i++){
			dxh -= m_plength[i];
			if(dxh<0) break;
		}

//		if(dxh<0)					//完整性检查，防止越界  xxx 会破坏tableview本身的保护机制
			return i;


	}

};

#endif
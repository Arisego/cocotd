#include "BufferList.h"
#include "DBUtil.h"

BufferList *BufferList::mSharedBufferList = NULL;


BufferList *BufferList::sharedBufferList()
{
	//if(GameManager::sharedLogicCenter()->ml->m_iMLState < 2){	// <对于不是战斗地图的情况不提供bf实例
	//	return NULL;
	//}
	if (mSharedBufferList == NULL)
	{
		mSharedBufferList = new BufferList();
		if (!mSharedBufferList || !mSharedBufferList->init())
		{
			CC_SAFE_DELETE(mSharedBufferList);
		}

	}

	return mSharedBufferList;

}


void BufferList::purgeSharedBufferList()
{
	CC_SAFE_DELETE(mSharedBufferList);

}

//////////////////////////////////////////////////////////////////////////
void BufferList::LoadFromDb()
{
	DBUtil::initDB("save.db"); 

	string sqlstr = "select * from buffer_list";

	{
		vector<map<string,string>> adata;

		adata = DBUtil::getDataInfo(sqlstr,NULL);
		int m_number = adata.size();

		for(int i = 0;i<m_number;i++){
			msiBufferScps.insert(make_pair(stoi(adata.at(i).at("id")), adata.at(i).at("scp")));
		}
	}

	DBUtil::closeDB(); 
}

bool BufferList::init()
{
	bool ret = false;
	do 
	{
		LoadFromDb();
		ret = true;
	} while (false);
	return ret;
}

const char* BufferList::getScp(int ai)
{
	return msiBufferScps.at(ai).c_str();
}


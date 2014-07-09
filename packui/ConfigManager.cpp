#include "ConfigManager.h"
#include "cocos2d.h"

#include "DBUtil.h"

ConfigManager* ConfigManager::mSharedConfigManager = NULL;

template <class T> 
std::string ConvertToString(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

ConfigManager::~ConfigManager(){
	
	cvs->release();
	strtable->release();
}

ConfigManager* ConfigManager::sharedConfigManager(){
	if(!mSharedConfigManager){
		mSharedConfigManager = new ConfigManager();
		mSharedConfigManager->init();
	}
	return mSharedConfigManager;
}

bool ConfigManager::init(){
	do 
	{
		DBUtil::initDB("save.db"); 
		cvs = new CCDictionary();
		strtable = new CCDictionary();
		LoadConfig();
		DBUtil::closeDB(); 
		return true;
	} while (0);
	return false;
}

void ConfigManager::LoadConfig(){
	string sqlstr="select * from config_save";
	{
		vector<map<string,string>> vdata;

		vdata = DBUtil::getDataInfo(sqlstr,NULL);
		int m_number = vdata.size();

		for(int i = 0;i<m_number;i++){


			ConfigStruct* cs = new ConfigStruct();
			cs->autorelease();
			cs->value = atof(((map<string,string>) vdata.at(i)).at("value").c_str());
			cs->dvalue = atof(((map<string,string>) vdata.at(i)).at("dvalue").c_str());


			cvs->setObject(cs,atof(((map<string,string>) vdata.at(i)).at("id").c_str()));

		}

		sqlstr = "select * from str_table";
	}
	{
		vector<map<string,string>> adata;

		adata = DBUtil::getDataInfo(sqlstr,NULL);
		int m_number = adata.size();

		for(int i = 0;i<m_number;i++){
			strtable->setObject(CCStringMake(((map<string,string>) adata.at(i)).at("content")),((map<string,string>) adata.at(i)).at("mask"));
		}
	}

}

void ConfigManager::GetConfigV(CMS flag, int &val){
	val = ((ConfigStruct*) cvs->objectForKey(flag))->value;
}

float ConfigManager::GetConfigF(CMS flag, float fact){
	return ((ConfigStruct*) cvs->objectForKey(flag))->value/fact;
}

void ConfigManager::GetConfigDV(CMS flag, int &val){
	val = ((ConfigStruct*) cvs->objectForKey(flag))->dvalue;
}

void ConfigManager::SetConfigV(int flag, int val){
	DBUtil::initDB("save.db"); 
	string sqlstr = "update config_save set value=" + ConvertToString(val) + " where id = " + ConvertToString(flag);
	((ConfigStruct*) cvs->objectForKey(flag))->value = val;
	DBUtil::updateData(sqlstr);
	DBUtil::closeDB(); 
}

string ConfigManager::GetConfigS(const char* msk){
	CCLog(">[ConfigManager] GetConfiS(%s)", msk);
	return ((CCString*) strtable->objectForKey(msk))->getCString();
}

void ConfigManager::purgeSharedConfigManager()
{
	CC_SAFE_DELETE(mSharedConfigManager);
	
}

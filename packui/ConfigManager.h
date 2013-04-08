#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include "cocos2d.h"
USING_NS_CC;

using namespace std;

typedef enum {
	CM_FULLSCREEN = 1,
	CM_VIDIEOVOLEM,
	CM_EFFECTVOLEM,
	CM_BGMVOLEM,
	CM_TEXTSPEED,
	CM_AUTOTIME
}CMS;

class ConfigStruct: public CCObject
{
public:
	string label;
	int value,dvalue;
};

class ConfigManager
{
	static ConfigManager* mSharedConfigManager;
	bool init();
	CCDictionary* cvs;
	CCDictionary* strtable;
public:
	static ConfigManager* sharedConfigManager();
	void LoadConfig();
	void GetConfigV(CMS flag, int &val);
	void GetConfigDV(CMS flag, int &val);
	float GetConfigF(CMS flag, float fact);
	void SetConfigV(int flag, int val);
	~ConfigManager();

	string GetConfigS(const char* msk);
};

#endif
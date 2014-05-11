#ifndef __WALK_MAP_H__
#define __WALK_MAP_H__

#include "TileMap.h"

#include "AppMacros.h"
#include "hud/ItemPicker.h"

using namespace std;

class WalkMap : public TileMap{
public:
	WalkMap(const char* a,const char* b);
	WalkMap(const char* a,const char* b, Script* asp);

	WalkMap();

	bool show_hud();
	virtual bool init();

	CCArray* mt_EffectList;
	int mt_iCount;
	void show_text(string s);			//Called by ML through show_text();
	void effectback(CCObject* pSender);
	void addbullet(Script* scp);
private:

};

#endif
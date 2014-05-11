#ifndef _CHARA_S_H__
#define _CHARA_S_H__

#include "battle/BChara.h"
#include "battle/BEquip.h"

class CharaS : CCObject
{
public:
	CCDictionary* m_caCharas;
	vector<int> m_viCharas;
	static CharaS* mSharedCharaS;

	vector<map<string,string>> vdata;
	//CCDictionary *m_caStaticData;			
	bool m_bNotifate;		// If the notifation will be showed, its always off, and will be turn off automaticly after one shown off.


	int m_iNumber;

	int m_iGold;

	static CharaS *sharedCharaS();
	static void purgeSharedCharaS();

	bool init();

	void load_file(Script* sp);
	void load_chara(Script* sp);
	void inform();			//in form Chara.

	void bback(CCObject*);

	Chara* getchara(int i);			//get a Chara with a stored integer flag. 0~5 maybe. Class outside of CharaS is not suppose to use chara_id.
	Chara* getdispchara();			//get wm Chara. maybe you want a default one?

	~CharaS();
	Chara* getIDChara( int i );

};

#endif
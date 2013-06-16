#ifndef _CHARA_S_H__
#define _CHARA_S_H__

#include "utils/Scriptor.h"
#include "packui/ItemCell.h"

struct Equip : CCObject
{
	int id;
	string effect;
	string name;
	string discript;
	int position;

	int lock,sum;
};

class Chara : public CCObject
{
public:
	int m_iCharaID;
	string m_sName, m_sPsz;

	int m_iElement;
	map<int,int> m_miiEquips;		//for equipments
	/* TO */
	vector<int> m_viiESum;
	vector<int> m_viiELock;

	map<string,int> m_iiAttrs;		//for attributes
	vector<int> m_viSkills;			//for skills,change this to map if group is needed.



	bool m_bIsDead;


	Chara(){
		m_bIsDead = false;
	}

	~Chara(){
		m_viSkills.clear();
	}

	int getvalue(string name){
		return m_iiAttrs[name];
	}

	void setvalue(string name, int val){
		m_iiAttrs[name] = val;
	}

	void addvalue(string name, int val){
		if(!name.compare("chp")) sethp(m_iiAttrs[name] + val);
		else if(!name.compare("cmp")) setmp(m_iiAttrs[name] + val);
		else setvalue(name,m_iiAttrs[name] + val);
	}

	int gethp(){			//gethp is calling to getvalue "chp"
		return getvalue("chp");
	}

	void sethp(int value){
		if(value > getvalue("hp"))
			value = getvalue("hp");
		else if(value < 0)
			value = 0;
		setvalue("chp",value);
	}
	int getmp(){			//getmp is calling to getvalue "cmp"
		return getvalue("cmp");
	}
	
	void setmp(int value){
		if(value > getvalue("mp"))
			value = getvalue("mp");
		else if(value < 0)
			value = 0;
		setvalue("cmp",value);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	map<string,int> mt_stDiffer;
	//set<string> mt_effecSet;

	void aplyDiffer(){
		for(map<string,int>::iterator it = mt_stDiffer.begin(); it != mt_stDiffer.end(); ++it){
			CCLOG(">Aply Differs:%s,%d",it->first.c_str(),it->second);
			m_iiAttrs[it->first] += it->second;
		}
	}

	void calDiffer( string item_effect_1, string item_effect_2 )
	{
		//string test = "atk 1 def 2 mag 3";
		mt_stDiffer.clear();

		stringstream teststream;
		teststream<<item_effect_1;
		string s;
		int k;
		do{
			s.clear();
			teststream>>s;
			if(s.length() < 1) break;
			teststream>>k;
			mt_stDiffer[s] = -k;
			CCLOG("Read out :%s.%d.:",s.c_str(),k);
		} while(1);

		teststream.clear();
		teststream<<item_effect_2;
		do{
			s.clear();
			teststream>>s;
			if(s.length() < 1) break;
			teststream>>k;
			mt_stDiffer[s] += k;
			CCLOG("Read out :%s.%d.:",s.c_str(),k);
		} while(1);

		for(map<string,int>::iterator it = mt_stDiffer.begin(); it != mt_stDiffer.end(); ++it){
			CCLOG(">Deffer cal:%s,%d",it->first.c_str(),it->second);
		}
		return;
	}
};

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
	
};

#endif
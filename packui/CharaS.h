#ifndef _CHARA_S_H__
#define _CHARA_S_H__

#include "utils/Scriptor.h"
#include "packui/ItemCell.h"

//static const char* tlevel[] = {"E","D","C","B","A","S"};
static const int siHunPo[] = {2,4,5,6,8,10};
static const int siHuiHun[] = {0,1,2,3,4,6};
static const int siXuDong[] = {1,2,3,4,5,6};
static const int siXuDongP[] = {0,0,1,1,2,2};	// <续动额外加成
static const int siGeDang[] = {0,1,2,3,5,7};

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
	//vector<int> m_viSkills;			//for skills,change this to map if group is needed.
	map<int,int> m_viSkills;

	bool m_bIsDead;


	Chara(){
		m_bIsDead = false;
		miLevel	  = 0;
		miLeadAdd = 0;
		m_ssLib	  = NULL;
	}

	~Chara(){
		m_viSkills.clear();
		CC_SAFE_RELEASE_NULL(m_ssLib);
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
		if(value > getvalue("hp")){
			value = getvalue("hp");
			m_bIsDead = false;
		}			
		else if(value < 0)
		{
			m_bIsDead = true;
			value = 0;
		}
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

	/* item_effect_1:原装备 || item_effect_2:目标装备 */
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

	//////////////////////////////////////////////////////////////////////////
	// <Skill

	/* <意志，请注意Refresh函数读取的是第几个值 */
	int miLevel;
	/* <用于刷新修正指数，在BF判定前调用这个接口 */
	void RefreshFix(){
		miLevel = getvalue("tama_3");		// 0 - 5 || E - S
	}

	/* <获得统帅的值 */
	int getLead(){
		return getvalue("tama_5");
	}

	/* <读取概率加成 */
	int getFixRate(){
		float fix_rate = gethp()/getvalue("hp");
		switch (miLevel)
		{
		case 4:	//A
			{
				if(fix_rate<0.2){
					return 4;
				}else if(fix_rate<0.4){
					return 2;
				}else{
					return 0;
				}
				break;
			}
		case 5:	//S
			{
				if(fix_rate<0.2){
					return 8;
				}else if(fix_rate<0.35){
					return 4;
				}else if(fix_rate<0.5){
					return 2;;
				}else{
					return 0;
				}
				break;
			}
		default:
			return 0;
			break;
		}
	}

	/* <如果不是需要修正的参数请不要调用这个接口！ */
	int getFixValue(string name){
		float fix_rate = gethp()/getvalue("hp");
		int ret;
		switch (miLevel)
		{
		case 0:	//E
			{
				
				if(fix_rate<0.5){
					ret = (getvalue(name) - 5);
				}else if(fix_rate<0.8){
					ret = (getvalue(name) - 3);
				}else{
					ret = getvalue(name);
				}
				break;
			}
		case 1:	//D
			{
				if(fix_rate<0.4){
					ret = (getvalue(name) - 3);
				}else if(fix_rate<0.7){
					ret = (getvalue(name) - 1);
				}else{
					ret = getvalue(name);
				}
				break;
			}
		case 2:	//C
			{
				if(fix_rate<0.5){
					ret = (getvalue(name) - 1);
				}else{
					ret = getvalue(name);
				}
				break;
			}
		case 3:	//B
			{
				if(fix_rate<0.3){
					ret = (getvalue(name) + 2);
				}else if(fix_rate<0.5){
					ret = (getvalue(name) + 1);
				}else{
					ret = getvalue(name);
				}
				break;
			}
		case 4:	//A
			{
				if(fix_rate<0.2){
					ret = (getvalue(name) + 4);
				}else if(fix_rate<0.4){
					ret = (getvalue(name) + 3);
				}else if(fix_rate<0.6){
					ret = (getvalue(name) + 2);
				}else{
					ret = getvalue(name);
				}
				break;
			}
		case 5:	//S
			{
				if(fix_rate<0.2){
					ret = (getvalue(name) + 6);
				}else if(fix_rate<0.35){
					ret = (getvalue(name) + 4);
				}else if(fix_rate<0.5){
					ret = (getvalue(name) + 3);;
				}else if(fix_rate<0.7){
					ret = (getvalue(name) + 2);;
				}else{
					ret = getvalue(name);
				}
				break;
			}
		default:
			ret = getvalue(name);
			break;
		}
		return ret+miLeadAdd;
	}

	/* <统帅加成 */
	map<int,int> mLRecord;
	int miLeadAdd;

	/* <清理，在BF开始时执行 */
	void ClearLead(){
		mLRecord.clear();
	}

	/* <单纯的添加不需要RefreshLead */
	void AddLead(int val){
		mLRecord[val] = mLRecord[val] + 1;
		if(val>miLeadAdd) miLeadAdd = val;
	}

	void RemoveLead(int val){
		mLRecord[val] = mLRecord[val] - 1;
	}

	/* <记得刷新 */
	void RefreshLead(){
		for(int i = 3; i>0; --i){
			miLeadAdd = mLRecord[i];
			if(miLeadAdd > 0){
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	/* <获得反击的优先权 */
	int GetRePrioer(){
		return getvalue("boss_class")*100 + getvalue("lv")+1;
	}

	/* <存储声音播放配置 */
	CCDictionary* m_ssLib;
	float m_sDelay;
	/* <播放特定的声音脚本 */
	void PlaySS(){
		if(!m_ssLib) return;

		int tiLevel = 14;
		Script* ts =(Script*) m_ssLib->objectForKey("name");
		if(!ts) return;
		CCDictionary* mca = ts->mapscpnodes;
		if(!ts) return;
		CCDictElement* tco;

		CCDICT_FOREACH(mca,tco){
			Script* mtca = (Script*) tco->getObject();

			if(tiLevel<mtca->getint("min")) continue;
			if(tiLevel>mtca->getint("max")) continue;

			int rate = 0;
			int b_r = CCRANDOM_0_1() * 100;
			CCArray* tcca = mtca->scriptnodes;
			
			for(int i = 0; i<tcca->count(); ++i)
			{
				Script* td = (Script*) tcca->objectAtIndex(i);
				rate += td->getint("rate");
				if(b_r<rate) {
					m_sDelay = td->getfloat("time");
					CCLog(">Ready for sound playing:%s,%f",td->getstring("value"),m_sDelay);
					break;
				}
			}

			break;
		}

	}

	//////////////////////////////////////////////////////////////////////////
	// <战斗轮换

	/* <魂魄最大值 */
	int getSHP(){
		return siHunPo[m_iiAttrs["tama_0"]];
	}

	void initValues(){ // <初始化
		setvalue("b_hunpo",siHunPo[m_iiAttrs["tama_0"]]);	// <魂魄
		setvalue("b_maxxudong",siXuDong[m_iiAttrs["tama_2"]]);
		setvalue("b_maxgedang",siGeDang[m_iiAttrs["tama_4"]]);
		roundUp();
		miRu = 0;
	}

	int miRu;
	void roundUp(){ // <回合切换时
		++miRu;
		// <回魂
		int hh = siHuiHun[m_iiAttrs["tama_1"]];
		if(hh == 0) hh = miRu%2;

		int th = m_iiAttrs["b_hunpo"];
		th += hh;
		if(th>siHunPo[m_iiAttrs["tama_0"]]) th = siHunPo[m_iiAttrs["tama_0"]];
		m_iiAttrs["b_hunpo"] = th;

		// <续动
		setvalue("b_xudong",m_iiAttrs["b_maxxudong"]);
	}

	void battleOver(){
		m_iiAttrs.erase("b_hunpo");
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
	Chara* getIDChara( int i );

};

#endif
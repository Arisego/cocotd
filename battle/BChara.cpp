#include "battle/BChara.h"
#include "SoundManager.h"
#include "DBUtil.h"
#include "BufferList.h"
#include "BBuffer.h"
#include "SingleTon/BattleFiled.h"
#include "BHalo.h"
#include "utils/Scriptor.h"
#include "GameManager.h"

static const char* sRateMask[] = {"mz_rate","hx_rate"};



Chara::Chara(){
	m_bIsDead = false;
	//miYizi	  = 0;
	miLeadAdd = 0;
	m_ssLib	  = NULL;
	miEXP	  = 0;

	mSkillList = new BSkillList();
	mcdBuffers = new CCDictionary();
	mcaHalos   = new CCArray();
	mcaFHalos  = new CCArray();

	msLastVoice.clear();

	miRateFix = 0;
	m_iiAdditions.clear();

	miSHitBClass = 0;
	mVodScp = NULL;

	miBeDamage = 0;
	miVodPrior = 0;
	mcTar = NULL;
	//m_sName = "FUCINGBUG";

	mEcOwner = nullptr;

	miRange = 1;
	mfLastHpRate = 1.0;
	mfLastHpJudge = 1.0;
	miSSCount = 0;
	miRateMax = 100;

	miSeType = 0;
}

Chara::~Chara(){
	m_viSkills.clear();	// MarKD
	CC_SAFE_DELETE(mSkillList);
	CC_SAFE_RELEASE_NULL(m_ssLib);
	CC_SAFE_RELEASE_NULL(mcdBuffers);
	mcaHalos->removeAllObjects();
	CC_SAFE_RELEASE_NULL(mcaHalos);
	RemoveRefHalos();
}

int Chara::getvalue(string name){
	int ret = m_iiAttrs[name];
	//CCLog(">[Chara] getvalue() | %s, %d", name.c_str(), ret);
	return ret;
}

void Chara::setvalue(string name, int val){
	m_iiAttrs[name] = val;
}

void Chara::addBatValue(string name, int val)
{
	int ai = getvalue("tc" + name);		// <生成对应的控制值 tcb_为额外的增添控制值
	int bi = getvalue("br" + name);		// <作用概率

	if(bi>0 && val<0){
		if((bi/100) < CCRANDOM_0_1())	// <概率过滤
			return;
	}

	if(ai>0 && val<0){					// <对值进行修正
		val = min(val+ai, 0);	// <值是负的
		CCLog(">[BChara] ValueFixed | name:%s, val:%d ", name.c_str(), val);
	}

	CCLog(">[BChara] addBatValue: %s,%d | Final.", name.c_str(), val);
	plusvalue(name, val);
}

void Chara::addvalue(string name, int val){
	if(name[0] == 'b'){
		if(name[1] == '_'){
			addBatValue(name,val);
			return;
		}
	}

	if(!name.compare("chp")) sethp(m_iiAttrs[name] + val);
	else if(!name.compare("cmp")) setmp(m_iiAttrs[name] + val);
	else setvalue(name,m_iiAttrs[name] + val);
}

void Chara::plusvalue(string name, int val)
{
	setvalue(name,m_iiAttrs[name] + val);
}

void Chara::implyVal(string name, int val){
	setvalue(name,m_iiAttrs[name] + val);
}

int Chara::gethp(){			//gethp is calling to getvalue "chp"
	return getvalue("chp");
}

void Chara::sethp(int value){
	//CCLog(">[Chara]sethp() | %d",value);
	if(value > getvalue("hp")){
		value = getvalue("hp");
		m_bIsDead = false;
		//CCLog(">[Chara]sethp() | Over Gagge.");
	}			
	else if(value <= 0)
	{
		m_bIsDead = true;
		value = 0;
		//CCLog(">[Chara]sethp() | Dead.");
	}
	setvalue("chp",value);
	Trig(TR_HP);
}

int Chara::getmp(){			//getmp is calling to getvalue "cmp"
	return getvalue("cmp");
}
	
void Chara::setmp(int value){
	if(value > getvalue("mp"))
		value = getvalue("mp");
	else if(value < 0)
		value = 0;
	setvalue("cmp",value);
}

void Chara::aplyDiffer(){
	for(map<string,int>::iterator it = mt_stDiffer.begin(); it != mt_stDiffer.end(); ++it){
		CCLOG(">Aply Differs:%s,%d",it->first.c_str(),it->second);
		m_iiAttrs[it->first] += it->second;
	}
}

/* <item_effect_1:原装备 || item_effect_2:目标装备 */
void Chara::calDiffer( string item_effect_1, string item_effect_2 )
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

//void Chara::RefreshFix(){
//	miYizi = getvalue("tama_3");		// 0 - 5 || E - S
//}
	
int Chara::getLead(){
	return getvalue("tama_5");
}

#define  miYizi getvalue("tama_3")
//////////////////////////////////////////////////////////////////////////
// <各种加成

int Chara::getFixRate(int tiType){
	int tiRate = 0;

	// < 读取意志部分的加成
	float fix_rate = gethp()/getvalue("hp");
	switch (miYizi)
	{
	case 4:	//A
		{
			if(fix_rate<0.2){
				tiRate =4;
			}else if(fix_rate<0.4){
				tiRate =2;
			}else{
				tiRate =0;
			}
			break;
		}
	case 5:	//S
		{
			if(fix_rate<0.2){
				tiRate =8;
			}else if(fix_rate<0.35){
				tiRate =4;
			}else if(fix_rate<0.5){
				tiRate =2;;
			}else{
				tiRate =0;
			}
			break;
		}
	default:
		tiRate =0;
		break;
	}
	
	// <读取Buffer造成的影响
	tiRate += miRateFix;
	tiRate += getvalue(sRateMask[tiType]);

	return tiRate;
}

	
int Chara::getFixValue(string name){
	float fix_rate = ((float) gethp())/getvalue("hp");
	int ret;

	if(name[0] == 'a' && name[1] == '_') return getvalue(name);	// < a_ 开头的不需要额外附加
	// <读取意志造成的影响
	ret = getvalue(name);
	CCLog(">[Chara] GetFixValue | Aruji: %s", m_sName.c_str());
	CCLog(">[Chara] GetFixValue | Moto: %s, %d", name.c_str(), ret);
	CCLog(">[Chara] GetFixValue | Yizi: %d, CHP/HP: %d/%d, RaV: %0f", miYizi, gethp(), getvalue("hp"), fix_rate);

	switch (miYizi)
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
	CCLog(">[Chara] GetFixValue | YiZhi: %s, %d", name.c_str(), ret);

	// <读取统帅造成的影响
	ret += miLeadAdd;

	// <读取Buffer造成的影响
	ret += m_iiAdditions[name];

	CCLog(">[Chara] GetFixValue | Final: %s, %d", name.c_str(), ret);

	return ret;
}

//////////////////////////////////////////////////////////////////////////
// Lead	||  <统率 | 不修改概率
void Chara::ClearLead(){
	mLRecord.clear();
}

/* <单纯的添加不需要RefreshLead */
void Chara::AddLead(int val){
	mLRecord[val] = mLRecord[val] + 1;
	if(val>miLeadAdd) miLeadAdd = val;
}

void Chara::RemoveLead(int val){
	mLRecord[val] = mLRecord[val] - 1;
}

/* <记得刷新 */
void Chara::RefreshLead(){
	for(int i = 3; i>0; --i){
		miLeadAdd = mLRecord[i];
		if(miLeadAdd > 0){
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
	
/* <获得反击的优先权 */
int Chara::GetRePrioer(){
	return getvalue("boss_class")*100 + getvalue("lv")+1;
}

//////////////////////////////////////////////////////////////////////////
/* <播放特定的声音脚本 | maxprior 给出优先级覆盖过滤 */
void Chara::PlaySS(const char* asName, int aiPrio){
	m_sDelay = 0;
	CCLog(">[Chara] PlaySS() | %s |", asName);
	if(!m_ssLib) return;

	int tiLevel = 14;		// <当前关卡值 [调试]

	miTmpVodPrior = aiPrio;

	Script* ts =(Script*) m_ssLib->objectForKey(asName);
	if(!ts) return;
	CCDictionary* mca = ts->mapscpnodes;
	if(!ts) return;
	CCDictElement* tco;

	
	CCDICT_FOREACH(mca,tco){
		Script* mtca = (Script*) tco->getObject();

		if(tiLevel<mtca->getint("min")) continue;
		if(tiLevel>mtca->getint("max")) continue;

		if(mtca->getint("maxprior") < miVodPrior) break;	// <前方HP过滤是唯一的，所以这里 continue 和 break 等同
		int timp = mtca->getint("maxprior");

		if(checkSScp(mtca)) break;

		int rate = 0;
		int b_r = CCRANDOM_0_1() * miRateMax;
		CCArray* tcca = mtca->scriptnodes;
			
		//b_r = 55; // <调试

		int tiflag;
		for(int i = 0; i<tcca->count(); ++i)
		{
			Script* td = (Script*) tcca->objectAtIndex(i);
			tiflag = 1 << i;
			//CCLog("><><>< %d,%d", i, tiflag);
			
			if(td->getint("additid") > 0) continue;
			if(miRateMax<100){
				if(tiflag & miHSMask) continue;
			}

			rate += td->getint("rate");		/* <概率控制 | 累加过滤 */

			if(timp>0 && td->getint("maxprior") < timp) continue;

			if(b_r<rate) {
				if(timp>0) miTmpVodPrior = td->getint("maxprior");
				PlaySSScript(td);
				miHSMask |= tiflag;	// <临时保存
				break;
			}
		}

		break;
	}

	miRateMax = 100;
	if(aiPrio == 0){
		PlayVodActor();
	}
}

bool Chara::checkSScp(Script* ascp)
{
	bool ret = false;
	do 
	{
		if(ascp->getint("addit") == 0) break;	// <必须指定自身为额外脚本才能进入这一步判定	

		stringstream ss, ssc;
		ss << ascp->getstring("addstring");
		int b_r = CCRANDOM_0_1() * 100;

		string ts;
		ss >> ts;
		bool tBLock = false;
		while (ts.size() > 0)
		{
			ssc.clear();
			ssc << ascp->getstring(ts.c_str());

			int tki;
			ssc >> ts;
			ssc >> tki;
			tBLock = false;

			switch (tki)
			{
			case 1:	// <Buffer状态检测 | 大于0
				{
					ssc >> tki;
					if(BufferState(tki) > 0) tBLock = true;
					break;
				}
			case 2:	// <会心
				{
					if(((EChesses*) mEcOwner)->miHitFlag) tBLock = true; 
					break;
				}
			case 3:	// <埃尔维斯-不动单位
				{
					if(!((Chara*) mcTar)->CanAct()) tBLock = true;
					if(BattleField::sharedBattleField()->miState > 0) tBLock = false;
					if(tBLock){
						((EChesses*) ((Chara*) mcTar)->mEcOwner)->miFianDamage += 5;
					}
					break;
				}
			case 4:	// <闪避敌人攻击
				{
					if(((EChesses*) mEcOwner)->miAvgFlag) tBLock = true;
					break;
				}
			case 5:	/* <埃尔维斯-条件1：受到的伤害小于5 | 条件2上次反击时触发过ssa68的情况 */
				{
					CCLog(">[Chara] Check|Prior| case 5");
					string	tsMark;
					int		tiSet;

					ssc >> tsMark;
					ssc >> tiSet;

					if(miBeDamage < 5 && m_iiAttrs[tsMark] == 1){
						m_iiAttrs[tsMark] = tiSet;
						tBLock = true;
					}
					break;
				}
			case 6:	/* <晓绝 | Select_S | 条件1： 未触发过 | 条件2： 回数在5~7之间 */
				{
					if(m_iiAttrs["se_"+ts]>0) break;
					int tmit = GameManager::sharedLogicCenter()->ml->m_iTurn;
					if(tmit<5) break;
					if(tmit>7) break;

					if(tmit != 7){
						if(b_r > 0.333) break;
					}

					tBLock = true;

					break;
				}
			default:
				break;
			}

			if(tBLock){
				ssc >> tki;
				if(tki > b_r){	// <概率值大于随机数
					ssc >> tki;

					b_r = CCRANDOM_0_1() * 100;
					int tri = 0;
					CCArray* tcca = ascp->scriptnodes;
					for(int i = 0; i<tcca->count(); ++i)
					{
						Script* td = (Script*) tcca->objectAtIndex(i);
						if(td->getint("rate") > 0) continue;
						if(td->getint("additid") != tki) continue;	// <ID匹配
						tri += td->getint("rate");

						if(tri > -b_r) continue; 

						if(td->getint("maxprior") > 0) miTmpVodPrior =td->getint("maxprior") ;
						PlaySSScript(td);
						ret = true;
						break;
					}		


				}
			}

			if(ret) break;
			ts.clear();
			ss >> ts;
		}




	} while (false);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// <战斗轮换

/* <魂魄最大值 */
int Chara::getSHP(){
	return getvalue("b_maxhunpo");
}

void Chara::initValues(){ // <战场环境开启后进行的初始化
	
	int t_iType = getvalue("attack");		// <[TODO]从单位中获得攻击属性,具体的来源需要根据设计进行变更，注意默认取得的是0
	//t_iType = 1;																// <[TestOnly] 使用上面获取的值


	vector<map<string,string>> vdata;
	vdata.clear();
	DBUtil::initDB("save.db");
	CCString* t_csSql = CCString::createWithFormat("select * from attr_attact where id = %d",t_iType);
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	int m_number = vdata.size();
	DBUtil::closeDB(); 

	map<string,string> t_ssm = (map<string,string>) vdata.at(0);	
	miRange = stoi(t_ssm.at("range"));

	initSkill();

	miRu = 0;
	if(!StateCenter::sharedStateCenter()->m_bIsLoad){ // <非读档状态才需要进行这些初始化
		setvalue("b_hunpo", siHunPo[m_iiAttrs["tama_0"]]);	// <魂魄
		setvalue("b_maxhunpo", siHunPo[m_iiAttrs["tama_0"]]);
		setvalue("b_maxxudong",siXuDong[m_iiAttrs["tama_2"]]);
		setvalue("b_maxgedang",siGeDang[m_iiAttrs["tama_4"]]);

		m_iiAttrs["b_actcout"] = 0;		// <单位行动过的次数
		roundUp();
	}
	
	
	miRu = 0;
}


void Chara::roundUp(){ // <回合切换时
	++miRu;

	Trig(TR_ROUND);

	// <回魂
	int hh = siHuiHun[m_iiAttrs["tama_1"]];
	if(hh == 0) hh = miRu%2;

	int th = m_iiAttrs["b_hunpo"];
	th += hh;
	if(th>getvalue("b_maxhunpo")) th = getvalue("b_maxhunpo");
	m_iiAttrs["b_hunpo"] = th;

	// <续动
	setvalue("b_xudong",m_iiAttrs["b_maxxudong"]);

	setvalue("b_gedang", m_iiAttrs["b_maxgedang"]);

	//////////////////////////////////////////////////////////////////////////
	// <Buffer检测

	// <颜色
	
	((EChesses*) mEcOwner)->setGrey(0);
}

void Chara::battleOver(){
	m_iiAttrs.erase("b_hunpo");
	m_iiAttrs.erase("b_maxxudong");
}

int Chara::getMoveDis()
{
	return 4;
}

static const int HPZDL[] = {15, 20, 25, 30, 35, 40};		// E - S | 15 - 40 || 魂魄 - 战斗力
int Chara::getZDL()
{
	return (getFixValue("atk") + getFixValue("def") + getFixValue("mag") + getFixValue("rst") + getFixValue("hit") + getFixValue("avg") + getFixValue("luk") + 2*getFixValue("mov")
			+ gethp()/2 + HPZDL[getvalue("tama_0")] + HPZDL[getvalue("tama_1")]+ HPZDL[getvalue("tama_2")]+ HPZDL[getvalue("tama_3")]+ HPZDL[getvalue("tama_4")]+ HPZDL[getvalue("tama_5")]
		);
}

//////////////////////////////////////////////////////////////////////////
// <加成

void Chara::AddFiRate(int ai)
{
	miRateFix += ai;
}

void Chara::RemoveMAttrs(map<string,int> &attrs)
{
	for(map<string,int>::iterator it = attrs.begin(); it != attrs.end(); ++it){
		m_iiAttrs[it->first.c_str()] -= it->second;
	}
}

void Chara::AddonMAttrs(map<string,int> &attrs)
{
	for(map<string,int>::iterator it = attrs.begin(); it != attrs.end(); ++it){
		//m_iiAttrs[it->first.c_str()] += it->second;
		addvalue(it->first.c_str(), it->second);
	}
}

void Chara::RemoveBuffer(int aid)
{
	CCLog(">[Chara] RemoveBuffer(%04d)", aid);
	mbDels.insert(aid);//mcdBuffers->removeObjectForKey(aid);
	CCLog(">[Chara] RemoveBuffer Over...");
}

void Chara::LoadBuffer(int aid, int c, CCObject* acTar)
{
	CCLog(">[Chara] LoadBuffer(%d)", aid);
	string tscp = BufferList::sharedBufferList()->getScp(aid);
	BBuffer* tbb = new BBuffer(tscp.c_str(), this);
	tbb->mcTarget = acTar;
	//if(this == acTar) exit(99);

	tbb->miId = aid;
	tbb->miCountDown = c;
	mcdBuffers->setObject(tbb, aid);
	tbb->autorelease();
}

void Chara::initSkill()
{
	for(int i = 8; i< 16; ++i){
		int tsi = m_viSkills[i];
		if(tsi){
			BSkill* tbs = mSkillList->getSkill(tsi);
			LoadBuffer(tbs->m_iUseCase);
		}
	}
	
}

void Chara::checkVals()
{
	CCLog(">[Chara] checkVals() | %d", getvalue("chp"));
	if(getvalue("b_maxhunpo") < 0) setvalue("b_maxhunpo",0);
	if(m_iiAttrs["b_hunpo"]>getvalue("b_maxhunpo")) m_iiAttrs["b_hunpo"] = getvalue("b_maxhunpo");
	if(getvalue("chp") > getvalue("hp")) setvalue("chp", getvalue("hp"));
	if(getvalue("chp") <= 0) {
		CCLog(">[Chara] checkVals() | Dead");
		setvalue("chp",0);
		m_bIsDead = true;
	}else{
		m_bIsDead = false;
	}
	if(getvalue("b_hunpo") < 0) setvalue("b_hunpo",0);

}

int Chara::BufferState(int abid)
{
	BBuffer* tbb = (BBuffer*) mcdBuffers->objectForKey(abid);
	if(!tbb) return -1;

	return tbb->getState();
}

void Chara::ClearTriggers()
{
	CCLog(">[BChara] ClearTriggers()");
	for(set<int>::iterator it = mbDels.begin(); it != mbDels.end(); ++it){
		mcdBuffers->removeObjectForKey(*it);
	}
	mbDels.clear();
}

bool Chara::checkXuDong()
{
	if(!CanAct()) return true;
	
	return getvalue("b_xudong") <= 0;
}

bool Chara::CanAct()
{
	if(m_bIsDead) return false;
	if(getvalue("bf_sleep")) return false;
	if(getvalue("bf_qidao")) return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// < 音效相关

void Chara::setHit(int ai)
{
	CCLog(">[Chara] setHit() | %d", ai);
	
	if(abs(ai) >= abs(miSHitBClass)){
		if(!miSHitBClass) miSHitBClass = ai;
		miSHitBClass = min(ai, miSHitBClass);
	}

}

void Chara::PlaySESkill(int asid)
{
	CCLog(">[Chara] PlaySESkill() | skill id:%d", asid);
	PlaySS(CCString::createWithFormat("skill_%d", asid)->getCString(), 3);
}

void Chara::PlayAttack()
{
	if(abs(miSHitBClass) < 2) PlaySS("attack",1);
	else PlaySS("attack_boss",1);
}

void Chara::PlayReAtk()	// 2
{
	int b_r = CCRANDOM_0_1() * 100;
	CCLog(">[Chara] PlayReAttack() | Random:%d", b_r);

	// <下一步骤会自动调用PlayAttack
	if(abs(miSHitBClass) < 2){
		if(b_r<50){			// <测试
			int tirc = ((EChessComp*) ((EChesses*) mEcOwner)->getComponent("controller"))->miReCount;
			switch (tirc)
			{
			case 1:	// <第一次反击
				{
					PlaySS("ren1",2);
					break;
				}
			case 0:	// <第二次反击
				{
					PlaySS("ren2",2);
					break;
				}
			default:
				break;
			}

		}
	}else{
		PlayReFilt("reboss");
	}
	//PlayReFilt("reboss");
}

void Chara::PlayReFilt(const char* asname)
{
	CCLog(">[Chara] PlayReFit | %s", asname);
	if(!m_ssLib) return;

	CCLog(">[Chara] PlayReFit | match begins.");
	Script* ts =(Script*) m_ssLib->objectForKey(asname);
	if(!ts) return;
	CCDictionary* mca = ts->mapscpnodes;
	if(!ts) return;
	CCDictElement* tco;

	int tiBeHurt = miBeDamage;
	miBeDamage = 0;

	CCDICT_FOREACH(mca,tco){
		Script* mtca = (Script*) tco->getObject();
		CCLog(">[Chara] PlayReFit | %d,%d,%d", tiBeHurt, mtca->getint("min"), mtca->getint("max"));
		if(mtca->getint("min") > tiBeHurt) continue;
		if(mtca->getint("max") < tiBeHurt) continue;

		PlaySS(mtca->getstring("name"), 2);
		break;
	}
}

void Chara::PlayToKill()
{
	if(mbToKill){
		int b_r = CCRANDOM_0_1() * 100;
		if(b_r < 55) // <55%播放终结语音
		{
			if(abs(miSHitBClass) < 2) PlaySS("tokill",3);
			else PlaySS("tokill_boss",3);		
		}
	}

}

void Chara::PlayHpStat()	// 4
{
	if(!m_ssLib) return;

	Script* ts =(Script*) m_ssLib->objectForKey("hpstatlist");
	if(!ts) return;
	CCDictionary* mca = ts->mapscpnodes;
	if(!ts) return;
	CCDictElement* tco;

	float tfHprate = 1.0 * gethp()/getvalue("hp");
	int ti = 0;

	mfLastHpRate = tfHprate;

	//tfHprate = 0.44;
	CCDICT_FOREACH(mca,tco){
		++ti;
		Script* mtca = (Script*) tco->getObject();
		float tfMax = mtca->getfloat("max");

		if(mtca->getfloat("min") > tfHprate) continue;
		if(tfMax < tfHprate) continue;

		if(tfMax >= mfLastHpJudge){
			//mfLastHpJudge = tfMax;
			break;
		}

		mfLastHpJudge = tfMax;
		mfLastHpRate = mtca->getfloat("max");

		miRateMax	=	100 - m_iiAttrs[CCString::createWithFormat("bse_hpstatt_%d", ti)->getCString()];
		msHSMask	=	CCString::createWithFormat("bse_hpstatti_%d", ti)->getCString();
		miHSMask	=	m_iiAttrs[msHSMask];

		if(miRateMax>0) PlaySS(mtca->getstring("name"), 4);

		break;
	}

	if(tfHprate>mfLastHpJudge) mfLastHpJudge = tfHprate;
}

void Chara::PlayXuDong()
{
	CCLog(">[Chara] PlayXuDong() | %d", miSHitBClass);
	if(getvalue("b_xudong") == 0) return;

	if(miSHitBClass > 2){
		PlaySS("ctn_nbl");
	}else if(miSHitBClass > 0){
		PlaySS("ctn_nnl");
	}else if(miSHitBClass < -2){
		PlaySS("ctn_nbd");
	}else if(miSHitBClass < 0){
		PlaySS("ctn_nnd");
	}else{
		PlaySS("ctn_na");
	}
}

void Chara::PlaySSScript(Script* ascp)
{
	CCLog(">[Chara] PlaySSScript() | %d >> %d", miTmpVodPrior, miVodPrior);
	if(miTmpVodPrior >= miVodPrior){
		++miSSCount;
		CCLog(">[Chara] PlaySSScript() | Replace");
		mVodScp = ascp;
		miVodPrior = miTmpVodPrior;
	}
}

void Chara::initWithSsm(map<string,string> &a_ssm)
{

	m_sName		 =	a_ssm.at("name");			
	m_sPsz		 =	a_ssm.at("psz");
	m_iElement	 =	stoi(a_ssm.at("element"));

	m_sSpx		 =	a_ssm.at("spx");
	msBHead		 =  a_ssm.at("bhead");
	CCLog(">[Chara] initwithSSm() | msBHead:%s - Name:%s", msBHead.c_str(), m_sName.c_str());

}

void Chara::LoadHalo(Script* asp)
{
	BHalo* tHa = new BHalo(asp, mEcOwner);
	//tHa->m_ecOwner = mEcOwner;
	mcaHalos->addObject(tHa);
	tHa->autorelease();
}

void Chara::CheckHalo(CCObject* aec, CCPoint acpmo, CCPoint acpaf)
{
	CCObject* to;
	CCARRAY_FOREACH(mcaHalos,to){
		((BHalo*) to)->checkEC(aec, acpmo, acpaf);
	}
}

void Chara::RemoHalo()
{
	CCObject* to;
	CCARRAY_FOREACH(mcaHalos,to){
		((BHalo*) to)->removeAllMov();
	}
}

void Chara::RemoveRefHalos()
{
	CCObject* to;
	CCARRAY_FOREACH(mcaFHalos,to){
		((BHalo*) to)->removeClean(mEcOwner);
	}
	CC_SAFE_RELEASE_NULL(mcaFHalos);
}

void Chara::RefreshVodAttrs(const char* ast)
{
	RemoveMAttrs(miVodAttrs);
	miVodAttrs.clear();

	stringstream teststream;
	teststream<<ast;
	string s;
	int k;
	do{
		s.clear();
		teststream>>s;
		if(s.length() < 1) break;
		teststream>>k;
		miVodAttrs[s] += k;
		CCLog(">[Chara] RefreshVodAttrs:[%s||%d]",s.c_str(),k);
	} while(1);

	AddonMAttrs(miVodAttrs);
}

void Chara::TurnOver()
{
	CCLog(">[Chara] TurnOver()");
	RemoveMAttrs(miVodAttrs);
	miVodAttrs.clear();
}

void Chara::PlayVodActor()
{
	int tivp = miVodPrior;	/* <防止内部使用 */
	miVodPrior = 0;
	
	CCLog(">[Chara] PlayVodActor()");
	if(!mVodScp) return;

	//if(miVodPrior>0) GameManager::sharedLogicCenter()->ml->DelaySound(mVodScp, this->mEcOwner);
	//miVodPrior = 0;

	//////////////////////////////////////////////////////////////////////////
	// /* <脚本第一次过滤 */

	int tif = mVodScp->getint("filt");
	msLastVoice.clear();	// <直接清除保有
	Script* ttd = mVodScp;

	CCLog(">[Chara] PlayVodActor() | Filter:%d", tif);
	switch(tif)
	{
	case 0:		/* <普通 */
		{
			break;
		}
	case 1:		/* <HP分隔 | 造成的伤害 */
		{
			CCArray* ttca = mVodScp->scriptnodes;

			for(int j=0; j<ttca->count(); ++j){
				ttd = (Script*) ttca->objectAtIndex(j);
				//CCLog("TEEE[%d,%d,%d]",ttd->getint("min"), ttd->getint("max"), miToDamage);
				if(ttd->getint("min") < miToDamage && miToDamage < ttd->getint("max")){
					break;
				}
			}
			break;
		}
	case -1:
		{
			CCArray* ttca = mVodScp->scriptnodes;
			for(int j=0; j<ttca->count(); ++j){
				ttd = (Script*) ttca->objectAtIndex(j);
				//CCLog("TEEE[%d,%d,%d]",ttd->getint("min"), ttd->getint("max"), miToDamage);
				if(ttd->getint("min") < miBeDamage && miBeDamage < ttd->getint("max")){
					break;
				}
			}
			break;
		}
	case 2:		/* <Buffer状态 */
		{
			int tbid = mVodScp->getint("bid");
			int tbsta = BufferState(tbid);

			CCArray* ttca = mVodScp->scriptnodes;
			for(int j=0; j<ttca->count(); ++j){
				ttd = (Script*) ttca->objectAtIndex(j);

				if(tbsta <= ttd->getint("bstat")){
					break;
				}
			}

			break;
		}
	case 3:	/* <跳转 */
		{
			mcTar = NULL;
			mVodScp = NULL;
			miSSCount = 0;
			msHSMask.clear();

			PlaySS(ttd->getstring("jump"));
			return;
			break;
		}
	default:
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	// /* <脚本标记过滤 */
	string tsMark = ttd->getstring("mark");
	if(tsMark.size() > 0){
		int tiMarkState = m_iiAttrs[tsMark];

		CCArray* ttca = ttd->scriptnodes;
		for(int j=0; j<ttca->count(); ++j){
			ttd = (Script*) ttca->objectAtIndex(j);
			CCLog(">[Chara] PlayVod() | Mark:%s << %d", tsMark.c_str(), j);
			if(ttd->getint(tsMark.c_str()) ==  tiMarkState){
				if(ttd->getint("setmark")>0){
					m_iiAttrs[tsMark] = ttd->getint("setmark");
				}
				break;
			} 
		}

	}

	//////////////////////////////////////////////////////////////////////////
	// /* <脚本读取到执行脚本 */
	m_sDelay = ttd->getfloat("time");
	msLastVoice = ttd->getstring("value");

	if(ttd->getint("tpdelay") == 0){
		CCLog(">Ready for sound playing:%s,%f", msLastVoice.c_str(), m_sDelay);
		SoundManager::sharedSoundManager()->PlayEffect(msLastVoice.c_str());
	}else{
		GameManager::sharedLogicCenter()->ml->DelaySound(ttd, this->mEcOwner);
	}


	RefreshVodAttrs(ttd->getstring("actvals"));
	if(mcTar) ((EChesses*) ((Chara*) mcTar)->mEcOwner)->miFianDamage += ttd->getint("dam");	// <附加额外的伤害

	CCLog(">[Chara] PlayVodAttr() | Check:%d, %d, %d", miSSCount, tivp, miHSMask);
	if(miSSCount == 1 && tivp == 4 && miHSMask > 0){	// <输出的是HPStat音效
		CCLog(">[Chara] PlayVodAttr() | %s, %d", msHSMask.c_str(), miHSMask);
		 m_iiAttrs[msHSMask] = miHSMask;
		 mfLastHpJudge = mfLastHpRate;	/* <只有发生区间降落才会通过last hp judge的检测，故而采用tfMax和HpRate是一个效果 */
	}

	mcTar = NULL;
	mVodScp = NULL;
	miSSCount = 0;
	msHSMask.clear();
}

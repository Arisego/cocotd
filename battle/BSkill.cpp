#include "battle/BSkill.h"
#include "DBUtil.h"



//////////////////////////////////////////////////////////////////////////
//	<BSkill

BSkill::BSkill( map<string,string> &a_ssm )
{
	miDamage = stoi(a_ssm.at("dam"));			//PrimaryKey:ItemID
	msAddition = a_ssm.at("additional");
	msSideEffect = a_ssm.at("side_effect");
	msName = a_ssm.at("name");

	m_iUseCase = stoi(a_ssm.at("usecase"));			/* <标记技能的敌我识别数据 */
	m_iLink = stoi(a_ssm.at("link"));				/* <标记是否允许该格上存在单位 */
	miSFlag = stoi(a_ssm.at("phase"));				/* <技能的使用时间段旗标 */
	miType = stoi(a_ssm.at("type"));				/* <技能的主动被动类型 */

	miPhase = stoi(a_ssm.at("phase"));

	miTarget = stoi(a_ssm.at("target"));
	msDeffect = a_ssm.at("deffect");
	msScript = a_ssm.at("script");
}




//////////////////////////////////////////////////////////////////////////
//


void BSkillList::clear()
{
	m_viSkills.clear();
}

void BSkillList::insert( int aia, int aib )
{
	m_viSkills.insert(make_pair(aia,aib));
}

int BSkillList::size()
{
	return m_viSkills.size();
}

void BSkillList::saveXml( TiXmlElement *LockElement )
{
	for(map<int,int>::iterator it = m_viSkills.begin(); it != m_viSkills.end(); ++it){
		TiXmlElement *LockElement1 = new TiXmlElement("lock");
		LockElement->LinkEndChild(LockElement1);
		LockElement1->SetAttribute("lock",it->first);
		LockElement1->SetAttribute("id",it->second);
	}
}

void BSkillList::inform()
{
	vdata.clear();
	if(m_viSkills.size() == 0){
		CCLog("> [BS] Chara with no skill.");
		return;
	}
	CC_SAFE_RELEASE_NULL(mcd_Skills);
	mcd_Skills = new CCDictionary();

	//CCDictionary* m_cid = new CCDictionary();
	string t_sMask;
	for(map<int,int>::iterator it = m_viSkills.begin(); it != m_viSkills.end(); ++it)
	{
		int t_id = it->second;
		t_sMask +=  CCString::createWithFormat("%d,",t_id)->getCString();
	}
	t_sMask.erase(t_sMask.length()-1);
	CCString* t_csSql = CCString::createWithFormat("select * from skill_list where itemid IN (%s)",t_sMask.c_str());

	DBUtil::initDB("save.db");
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	int m_number = vdata.size();
	DBUtil::closeDB(); 

	m_miiViDb.clear();
	for(int i = 0;i<m_number;i++){
		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		int item_id = stoi(t_ssm.at("itemid"));			//PrimaryKey:ItemID
		m_miiViDb[item_id] = i;							//方便查找
		
		BSkill* tbs = new BSkill(t_ssm);
		mcd_Skills->setObject(tbs,item_id);
		tbs->autorelease();

		CCLOG(">Read for item id:%d.", item_id);
	}
	CCLog(">[BS] Skill Load over: %d skills loaded.", mcd_Skills->count());
}

BSkillList::BSkillList()
{
	mcd_Skills = NULL;
}

BSkillList::~BSkillList()
{
	CC_SAFE_RELEASE_NULL(mcd_Skills);
}

BSkill* BSkillList::getSkill( int id )
{
	if(id<=0) return NULL;
	BSkill* ret = (BSkill*) mcd_Skills->objectForKey(id);
	if(!ret) {
		CCLog(">[BSkill] Get Id [%d] From SingleTon. If you do not try this manualy, it's a script error.", id);
		ret = mSharedBSkillList->getSkill(id);
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// Init

BSkillList *BSkillList::mSharedBSkillList = NULL;


bool BSkillList::init()
{
	insert(9001,9001);
	insert(9002,9002);
	inform();
	return true;
}


BSkillList *BSkillList::sharedBSkillList()
{
	if (mSharedBSkillList == NULL)
	{
		mSharedBSkillList = new BSkillList();
		if (!mSharedBSkillList || !mSharedBSkillList->init())
		{
			CC_SAFE_DELETE(mSharedBSkillList);
		}

	}

	return mSharedBSkillList;

}


void BSkillList::purgeSharedBSkillList()
{
	CC_SAFE_DELETE(mSharedBSkillList);

}
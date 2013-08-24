#include "CharaS.h"
#include "DBUtil.h"
#include "utils/EffectControler.h"
#include "sublayer/InfoTab.h"

CharaS *CharaS::mSharedCharaS = NULL;

CharaS *CharaS::sharedCharaS()
{
	if (mSharedCharaS == NULL)
	{
		mSharedCharaS = new CharaS();
		if (!mSharedCharaS || !mSharedCharaS->init())
		{
			CC_SAFE_DELETE(mSharedCharaS);
		}
	}
	return mSharedCharaS;
}

void CharaS::purgeSharedCharaS()
{
	CC_SAFE_DELETE(mSharedCharaS);
}

bool CharaS::init(){
	do 
	{
		EffectControler::sharedEffectControler();
		m_caCharas = NULL;
		m_bNotifate = false;
		//m_caStaticData = NULL;
		return true;
	} while (0);
	
	return false;
}

void CharaS::load_file(Script* ts){
	CC_SAFE_RELEASE_NULL(m_caCharas);				//TODO:所有的icd都没有被释放，autorelease机制是否在CCObject上运行呢？
	m_caCharas = new CCDictionary();
	m_iNumber = 0;
	m_viCharas.clear();
	m_iGold = ts->getint("gold");
	load_chara(ts);

	CCLOG(">Load in charas:%d",m_viCharas.size());

}

void CharaS::load_chara(Script* ts){
	if(!m_caCharas) m_caCharas = new CCDictionary();

	for(int i = 0; i<ts->m_snum; ++i){
		Script* t = (Script*) ts->scriptnodes->objectAtIndex(i);

		CCLOG(">Read for chara:%d",t->getint("cid"));

		int i_id = t->getint("cid");
		Chara* ticd = new Chara();
		ticd->m_iCharaID = i_id;

		ticd->autorelease();
		m_caCharas->setObject(ticd,i_id);
		m_viCharas.push_back(i_id);
		ticd->m_viiELock.assign(5,0);
		ticd->m_viiESum.assign(5,0);

		for(int j = 0;j<t->m_snum;++j){
			Script* t_cp = (Script*) t->scriptnodes->objectAtIndex(j);
			int t_it = t_cp->getint("type");

			switch(t_it){

			case(0):
				{
					for(int k = 0; k<t_cp->m_snum;++k){
						Script* t_kcp = (Script*) t_cp->scriptnodes->objectAtIndex(k);
						ticd->m_iiAttrs.insert(pair<string,int>(t_kcp->getstring("name"),t_kcp->getint("value")));
					}
					break;
				}
			case(1):
				{
					int t_ii;
					for(int k = 0; k<t_cp->m_snum;++k){
						Script* t_kcp = (Script*) t_cp->scriptnodes->objectAtIndex(k);
						t_ii = t_kcp->getint("name");
						ticd->m_miiEquips.insert(pair<int,int>(t_ii,t_kcp->getint("value")));
						ticd->m_viiELock[t_ii] = t_kcp->getint("lock");
						ticd->m_viiESum[t_ii] = t_kcp->getint("sum");
					}
					break;
				}
			case(2):
				{
					for(int k = 0; k<t_cp->m_snum;++k){
						Script* t_kcp = (Script*) t_cp->scriptnodes->objectAtIndex(k);
						ticd->m_viSkills.insert(make_pair(t_kcp->getint("lock"),t_kcp->getint("id")));						
					}
					break;
				}

			}

		}


	}

		inform();

}

void CharaS::inform(){
	string t_sMask;
	CCDictElement* pElement = NULL;
	if(m_caCharas->count() == 0) return;
	CCDICT_FOREACH(m_caCharas,pElement)
	{
		t_sMask +=  CCString::createWithFormat("%d,",pElement->getIntKey())->getCString();
	}
	t_sMask.erase(t_sMask.length()-1);

	CCString* t_csSql = CCString::createWithFormat("select * from chara_list where id IN (%s)",t_sMask.c_str());

	DBUtil::initDB("save.db");
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	m_iNumber = m_caCharas->count();			

	int m_number = vdata.size();
	//CC_SAFE_RELEASE_NULL(m_caStaticData);
	//data = CCArray::createWithCapacity(m_iNumber);
	string out_put;
	//////////////////////////////////////////////////////////////////////////
	Scriptor* tsp = new Scriptor();

	for(int i = 0;i<m_number;i++){
		tsp->re_init();

		map<string,string> t_ssm = (map<string,string>) vdata.at(i);
		int item_id = stoi(t_ssm.at("id"));			//PrimaryKey:ID
		
		CCLOG(">Read DB for item id:%d.", item_id);

		Chara* t_ca = (Chara*) m_caCharas->objectForKey(item_id);
		if(t_ca->m_sName.length() > 0)	continue;				//No permission to rewrite, ti's add not update, do not mud them together. And here we manage the output to new chara only.
		t_ca->m_sName		 =	 t_ssm.at("name");
		t_ca->m_sPsz		 =	 t_ssm.at("psz");
		t_ca->m_iElement	 =	 stoi(t_ssm.at("element"));
		tsp->parse_string(t_ssm.at("sound"));
		t_ca->m_ssLib		 =	 tsp->mapscps;
		tsp->mapscps->retain();

		out_put += "[";
		out_put += t_ssm.at("name").c_str();
		out_put += "]";
			
	}
	DBUtil::closeDB(); 

	if(m_bNotifate){
		InfoTab::sharedInfoTab()->showbmfl("pre_people",this,menu_selector(CharaS::bback),out_put);
		m_bNotifate = false;
	}
}

void CharaS::bback( CCObject* )
{
	CCLOG("<......");
}

Chara* CharaS::getchara( int i )
{
	if(m_viCharas.size()<1) return NULL;							//TODO: TestOnly. You cant give a game without player, but test gives none.
	return ((Chara*) m_caCharas->objectForKey(m_viCharas[i]));		//It may return null point. But we do not generate a new one here. Its unexpected.
}

Chara* CharaS::getdispchara()
{
	return getchara(0);												//Change default CharaS through CharaS, do not control it separately.
}

Chara* CharaS::getIDChara( int i ){
	if(m_viCharas.size()<1) return NULL;							//TODO: TestOnly. You cant give a game without player, but test gives none.
	return ((Chara*) m_caCharas->objectForKey(i));					//It may return null point. But we do not generate a new one here. Its unexpected.
}

CharaS::~CharaS(){
	if(m_caCharas){
		m_caCharas->removeAllObjects();	
	}
	CC_SAFE_RELEASE_NULL(m_caCharas);
}



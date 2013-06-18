#include "StateCenter.h"
#include "GameManager.h"

#include "packui/ItemCell.h"


//////////////////////////////////////////////////////////////////////////
// Static Member Init
StateCenter *StateCenter::mSharedStateCenter = NULL;
map<string,int> StateCenter::g_lock = map<string,int>();

StateCenter *StateCenter::sharedStateCenter()
{
	if (mSharedStateCenter == NULL)
	{
		mSharedStateCenter = new StateCenter();
		if (!mSharedStateCenter || !mSharedStateCenter->init())
		{
			CC_SAFE_DELETE(mSharedStateCenter);
		}
	}
	return mSharedStateCenter;
}

void StateCenter::purgeSharedStateCenter()
{
	CC_SAFE_DELETE(mSharedStateCenter);
}

bool StateCenter::init(){
	do 
	{
		m_cdItemList = new CCDictionary();			//Avoid Null Refrence.
		g_charas = CharaS::sharedCharaS();
		t_ldb_cid = NULL;

		g_sp = NULL;
		m_scTags = NULL;
		m_bIsLoad = false;
		return true;
	} while (0);
	return false;
}

void StateCenter::clear(){
	g_lock.clear();
}

//////////////////////////////////////////////////////////////////////////
//	Functions for g-lock
void StateCenter::g_lock_plus(const char* name, int value){	//plus and minus
	g_lock[name] += value;
}

void StateCenter::g_lock_set(const char* name, int value){	//plus and minus
	g_lock[name] = value;
}

int StateCenter::g_lock_query(string name){
	return g_lock.count(name);
}

void StateCenter::g_lock_change(Script* ts){
	//[Error]CCLOG(">G-Lock.Change.");
	int num = ts->m_snum;
	CCArray* ss = ts->scriptnodes;
	for(int i =0;i<num;i++){
		Script* tmp = (Script*) ss->objectAtIndex(i);
		if(!tmp->getint("add")) g_lock_plus(tmp->getstring("name"), tmp->getint("value"));
		else g_lock_set(tmp->getstring("name"), tmp->getint("value"));
	}

}

void StateCenter::g_lock_check(Script* ts) {
	int jmp = -1;
	int num = ts->m_snum;
	CCArray* ss = ts->scriptnodes;
	for(int i=0;i<num;i++){
		Script* tmp = (Script*) ss->objectAtIndex(i);
		if(b_lock_check(tmp)) return;
	}
	GameManager::sharedLogicCenter()->e_jump(-1);
}

bool StateCenter::b_lock_check(Script* ts){
	int num = ts->m_snum;
	CCArray* ss = ts->scriptnodes;
	for(int i=0;i<num;i++){
		Script* tmp =(Script*) ss->objectAtIndex(i);
		if(!m_lock_check(tmp)) return false;
	}
	GameManager::sharedLogicCenter()->e_jump(ts->getint("jump"));
	return true;
}

bool StateCenter::m_lock_check(Script* ts){
	do 
	{
		const char* type = ts->getstring("type");
		//[Error]CCLOG("Lock check:you meant-%d,but i have-%d,method-%s,valuename:%s",ts->getint("value"),g_lock[ts->getstring("name")],type,ts->getstring("name"));
		if(strcmp(type,"st") == 0) {
			CC_BREAK_IF((ts->getint("value") > g_lock[ts->getstring("name")]));
		}else if(strcmp(type,"et") == 0){
			CC_BREAK_IF((ts->getint("value") == g_lock[ts->getstring("name")]));
		}else if(strcmp(type,"gt") == 0){
			CC_BREAK_IF((ts->getint("value") < g_lock[ts->getstring("name")]));
		}
		return false;
	} while (0);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// XML读写
bool StateCenter::f_save_file(const char* psz){
	do 
	{
		GameManager::sharedLogicCenter()->PrepareSave();

		TiXmlDocument *myDocument = new TiXmlDocument();

		TiXmlDeclaration Declaration( "1.0","UTF-8", "yes" );  
		myDocument->InsertEndChild( Declaration );   

		TiXmlElement *RootElement = new TiXmlElement("Save");
		myDocument->LinkEndChild(RootElement);


		//////////////////////////////////////////////////////////////////////////
		TiXmlElement *PackElement = new TiXmlElement("pack");
		RootElement->LinkEndChild(PackElement);
		PackElement->SetAttribute("type", "0");

		TiXmlElement *PngElement = new TiXmlElement("png");
		PackElement->LinkEndChild(PngElement);
		PngElement->SetAttribute("type",m_iState);
		PngElement->SetAttribute("content",m_sName.c_str());
		PngElement->SetAttribute("jump",m_iJump);
		PngElement->SetAttribute("olds",m_oldstate);
		PngElement->SetAttribute("bgi",m_sBgi.c_str());
		//[Error]CCLOG(">>Save to file.%s",m_sBgi.c_str());
		PngElement->SetAttribute("tjump",m_iTJump);

		for(map<string,int>::iterator it = TagMap.begin(); it != TagMap.end(); ++it){
			if(it->second == 0) continue;
			TiXmlElement *LockElement = new TiXmlElement("lock");
			PngElement->LinkEndChild(LockElement);
			LockElement->SetAttribute("name",it->first.c_str());
			LockElement->SetAttribute("tag",it->second);
			LockElement->SetAttribute("content",PathMap[it->first].c_str());

			//[Error]CCLOG(">save for png by tag:%d",it->second);
			CCSprite* t_ccs = (CCSprite*) GameManager::sharedLogicCenter()->te->getChildByTag(it->second);
			LockElement->SetAttribute("x",((CCString*) CCString::createWithFormat("f%f",t_ccs->getPositionX()))->getCString());
			LockElement->SetAttribute("y",((CCString*) CCString::createWithFormat("f%f",t_ccs->getPositionY()))->getCString());
			LockElement->SetAttribute("angel",((CCString*) CCString::createWithFormat("f%f",t_ccs->getRotation()))->getCString());
		}
		
		//////////////////////////////////////////////////////////////////////////
		PackElement = new TiXmlElement("pack");
		RootElement->LinkEndChild(PackElement);
		PackElement->SetAttribute("type",3);

		CCDictElement* tCe;
		CCDICT_FOREACH(m_cdItemList,tCe){
			CCDictionary* t_caI = (CCDictionary*) tCe->getObject();
			PngElement = new TiXmlElement("png");
			PackElement->LinkEndChild(PngElement);
			PngElement->SetAttribute("group",tCe->getIntKey());

			CCDictElement* pElement = NULL;
			CCDICT_FOREACH(t_caI,pElement)
			{
				ItemCellData* t_icad = (ItemCellData*) pElement->getObject();

				TiXmlElement *LockElement = new TiXmlElement("lock");
				PngElement->LinkEndChild(LockElement);

				LockElement->SetAttribute("item",t_icad->type_id);
				LockElement->SetAttribute("sum",t_icad->sum);
				LockElement->SetAttribute("lock",t_icad->lock);
				CCLOG(">Dic Each...");
			}
		}

		//////////////////////////////////////////////////////////////////////////
		PackElement = new TiXmlElement("pack");
		RootElement->LinkEndChild(PackElement);
		PackElement->SetAttribute("type",4);
		PackElement->SetAttribute("gold",g_charas->m_iGold);

		CCDictElement* pElement = NULL;
		CCDictionary* tcd = g_charas->m_caCharas;
		if(tcd){
			CCDICT_FOREACH(tcd,pElement)
			{
				Chara* t_icad = (Chara*) pElement->getObject();

				PngElement = new TiXmlElement("png");
				PackElement->LinkEndChild(PngElement);

				PngElement->SetAttribute("cid",t_icad->m_iCharaID);

				{
					TiXmlElement *LockElement = new TiXmlElement("lock");
					PngElement->LinkEndChild(LockElement);
					LockElement->SetAttribute("type",0);
					for(map<string,int>::iterator it = t_icad->m_iiAttrs.begin(); it != t_icad->m_iiAttrs.end(); ++it){
						TiXmlElement *LockElement1 = new TiXmlElement("lock");
						LockElement->LinkEndChild(LockElement1);
						LockElement1->SetAttribute("name",it->first.c_str());
						LockElement1->SetAttribute("value",it->second);
					}
				}

				{
					TiXmlElement *LockElement = new TiXmlElement("lock");
					PngElement->LinkEndChild(LockElement);
					LockElement->SetAttribute("type",1);
					int ti;
					for(map<int,int>::iterator it = t_icad->m_miiEquips.begin(); it != t_icad->m_miiEquips.end(); ++it){
						TiXmlElement *LockElement1 = new TiXmlElement("lock");
						LockElement->LinkEndChild(LockElement1);
						ti	= it->first;
						LockElement1->SetAttribute("name",ti);
						LockElement1->SetAttribute("value",it->second);
						LockElement1->SetAttribute("lock",t_icad->m_viiELock[ti]);
						LockElement1->SetAttribute("sum",t_icad->m_viiESum[ti]);
					}
				}

				{
					TiXmlElement *LockElement = new TiXmlElement("lock");
					PngElement->LinkEndChild(LockElement);
					LockElement->SetAttribute("type",2);
					for(vector<int>::iterator it = t_icad->m_viSkills.begin(); it != t_icad->m_viSkills.end(); ++it){
						TiXmlElement *LockElement1 = new TiXmlElement("lock");
						LockElement->LinkEndChild(LockElement1);
						LockElement1->SetAttribute("id",*it);
					}
				}


				PngElement->SetAttribute("chp",t_icad->getvalue("chp"));
				PngElement->SetAttribute("cmp",t_icad->getvalue("cmp"));
				CCLOG(">Dic Each...");
			}
		}

		
		//////////////////////////////////////////////////////////////////////////
		PackElement = new TiXmlElement("pack");
		RootElement->LinkEndChild(PackElement);
		PackElement->SetAttribute("type", "1");

		map<string,int>::iterator it;
		for (it = g_lock.begin(); it != g_lock.end(); ++it)
		{
			TiXmlElement *LockElement = new TiXmlElement("lock");
			//LockElement = new TiXmlElement("lock");
			PackElement->LinkEndChild(LockElement);
			LockElement->SetAttribute("name",it->first.c_str());
			LockElement->SetAttribute("value",it->second);
		}
		//////////////////////////////////////////////////////////////////////////
		if(il){												// type = 2|TileMap
			PackElement = new TiXmlElement("pack");
			RootElement->LinkEndChild(PackElement);
			PackElement->SetAttribute("type", "2");
			PackElement->SetAttribute("map",m_tpsz.c_str());
			PackElement->SetAttribute("script",m_tscr.c_str());
			ccColor4F color = GameManager::sharedLogicCenter()->ml->tm->getRenderColor();
			PackElement->SetAttribute("r",((CCString*) CCString::createWithFormat("f%f",color.r))->getCString());
			PackElement->SetAttribute("g",((CCString*) CCString::createWithFormat("f%f",color.g))->getCString());
			PackElement->SetAttribute("b",((CCString*) CCString::createWithFormat("f%f",color.b))->getCString());
			PackElement->SetAttribute("a",((CCString*) CCString::createWithFormat("f%f",color.a))->getCString());
			PackElement->SetAttribute("control",GameManager::sharedLogicCenter()->ml->tm->m_sNaCon.c_str());
			PackElement->SetAttribute("camera",GameManager::sharedLogicCenter()->ml->tm->m_sNaCam.c_str());
			PackElement->SetAttribute("sscript",GameManager::sharedLogicCenter()->ml->tm->m_sNaScp.c_str());
			PackElement->SetAttribute("iscript",GameManager::sharedLogicCenter()->ml->tm->m_iNaScp);
			//fa	--x
			//fb	--x
			//camara
			//control

			CCArray* indexs = il->allKeys();
			for(int i = 0; i<indexs->count(); ++i){
				CCString* index = (CCString*) indexs->objectAtIndex(i);
				Entiles* ten = (Entiles*) il->objectForKey(index->getCString());

				PngElement = new TiXmlElement("png");			//IN;
				PackElement->LinkEndChild(PngElement);

				PngElement->SetAttribute("name",ten->name.c_str());
				PngElement->SetAttribute("group",ten->group.c_str());
				PngElement->SetAttribute("psz",ten->psz.c_str());
				PngElement->SetAttribute("direct",ten->direc);
				PngElement->SetAttribute("stand",ten->stand);
				PngElement->SetAttribute("target",ten->m_sTarget.c_str());
				PngElement->SetAttribute("state",ten->state);
				PngElement->SetAttribute("gmid",ten->group_mask);
				PngElement->SetAttribute("giid",ten->group_id);

				b2Body* b2b = ten->m_body;
				PngElement->SetAttribute("x",((CCString*) CCString::createWithFormat("f%f",b2b->GetPosition().x))->getCString());
				PngElement->SetAttribute("y",((CCString*) CCString::createWithFormat("f%f",b2b->GetPosition().y))->getCString());
				PngElement->SetAttribute("angel",((CCString*) CCString::createWithFormat("f%f",b2b->GetAngle()))->getCString());

				for (b2Fixture* f = b2b->GetFixtureList(); f; f = f->GetNext()) 
				{
					b2Shape::Type shapeType = f->GetType();
					if ( shapeType == b2Shape::e_circle )
					{
						b2CircleShape* circleShape = (b2CircleShape*)f->GetShape();
						TiXmlElement *LockElement = new TiXmlElement("lock");
						PngElement->LinkEndChild(LockElement);
						LockElement->SetAttribute("radius",((CCString*) CCString::createWithFormat("f%f",circleShape->m_radius))->getCString());
						LockElement->SetAttribute("sensor",f->IsSensor());
						LockElement->SetAttribute("type",shapeType);
						Entiles* tmp = (Entiles*) f->GetUserData();
						if(tmp)
							LockElement->SetAttribute("name",tmp->name.c_str());
						
					}
					else if ( shapeType == b2Shape::e_polygon )
					{
						b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
						//[Error]CCLOG("E+POLYGON.<Not using.");
					}
				}
				
				/*CCDictionary* en_list = il->objectForKey(index->getCString());
				CCArray* en_indexs = en_list->allKeys();
				for(int j=0; j<en_indexs->count(); ++j){
					CCString* en_name = (CCString*) en_indexs->objectAtIndex(j);
					Entiles* ten = en_list->objectForKey(en_name->getCString());

					


				}*/

			}


		}
		//////////////////////////////////////////////////////////////////////////


		CCString* fullPath = CCString::createWithFormat("save/%s.xml",psz);
		myDocument->SaveFile(fullPath->getCString());//保存到文件
		fullPath = CCString::createWithFormat("save/%s.jpg",psz);
		
		GameManager::sharedLogicCenter()->snapshot->saveToFile(fullPath->getCString(),false);
		myDocument->Clear();
		delete myDocument;

		return true;
	} while (0);
	return false;
}

bool StateCenter::f_load_file(const char* psz){
	do												//TODO:分离，让重生的gamescene自己来取。
	{		//Move it to Load over.
		CC_SAFE_DELETE(g_sp);
		g_sp = NULL;

		g_sp = new Scriptor();		
		CCString* fullPath = CCString::createWithFormat("save/%s.xml",psz);
		if(! g_sp->parse_file(fullPath->getCString())) break;
		//CCDirector::sharedDirector()->getRunningScene()->pauseSchedulerAndActions();
		CC_SAFE_RELEASE_NULL(m_scTags);
		m_bIsLoad = true;

		CCArray* scrps = g_sp->initcs;
		for(int i = 0; i<scrps->count(); ++i){
			Script* t = (Script*) scrps->objectAtIndex(i);
			switch(t->getint("type")){
			case(0):					//脚本文件状态
				{
					t =(Script*) t->scriptnodes->objectAtIndex(0);
					m_iState	=	t->getint("type");
					m_iJump		=	t->getint("jump");
					m_sName		=	t->getstring("content");
					m_oldstate	=	t->getint("olds");
					m_sBgi		=	t->getstring("bgi");
					m_iTJump	=	t->getint("tjump");
					GameManager::sharedGameManager()->runSceneWithId(GameManager::SCENE_PLAY);
					m_scTags	=	t;
					m_scTags->retain();
					return true;
				}
			}
		}

	} while (0);
	return false;
}

bool StateCenter::g_load_file(){
	if(!g_sp) return false;
	
	CCArray* scrps = g_sp->initcs;
	for(int i = 0; i<scrps->count(); ++i){
		Script* t = (Script*) scrps->objectAtIndex(i);
		switch(t->getint("type")){
		case(0):
			{
				break;
			}
		case(1):
			{
				f_load_lock(t);
				break;
			}
		case(2):
			{
				f_load_map(t);
				break;
			}
		case(3):
			{
				f_load_item(t);
				break;
			}
		case(4):
			{
				g_charas->load_file(t);
				break;
			}
		default:
			{
				CCLOG("<Error in LoadingSCP.Invalid Type:%d...",t->getint("type"));
				exit(0x5002);
			}
		}
	}

	CC_SAFE_DELETE(g_sp);
	g_sp = NULL;
	return true;
}

void StateCenter::f_load_map(Script* ts){
	GameManager::sharedLogicCenter()->ml->LoadMap(ts->getstring("map"),ts->getstring("script"),ts);
}

void StateCenter::f_load_lock(Script* ts){
	g_lock.clear();
	for(int i = 0; i<ts->m_snum; ++i){
		Script* t = (Script*) ts->scriptnodes->objectAtIndex(i);
		g_lock[t->getstring("name")] = t->getint("value");
	}
}

void StateCenter::f_get_state(){
	GameScene* gs = GameManager::sharedLogicCenter();
	il = NULL;
	//////////////////////////////////////////////////////////////////////////
	m_oldstate	=	gs->las_state;
	m_iState	=	gs->e_State;
	m_iJump		=	gs->e_curscript;
	m_sName		=	gs->scpfile;
	m_iTJump	=	gs->t_curscript;
	//////////////////////////////////////////////////////////////////////////
	//[Error]CCLOG(">Get State()");
	m_sBgi.clear();
	if(gs->BgImg){
		m_sBgi = gs->m_sBgi;
		//[Error]CCLOG(">>Value:%s",m_sBgi.c_str());
	}
	//////////////////////////////////////////////////////////////////////////
	TagMap.clear();
	PathMap.clear();
	if(gs->te->TagMap.size()>0){
		TagMap = gs->te->TagMap;
		PathMap = gs->te->PathMap;
	}
	//////////////////////////////////////////////////////////////////////////
	m_tpsz.clear();
	m_tscr.clear();
	CC_SAFE_RELEASE_NULL(il);
	if(gs->ml->wm){
		m_tpsz = gs->ml->wm->map_path;
		m_tscr = gs->ml->wm->scr_path;
		il	=	gs->ml->wm->m_itemlist;
	}
}

void StateCenter::f_load_item( Script* ts )
{
	CC_SAFE_RELEASE_NULL(m_cdItemList);				//TODO:所有的icd都没有被释放，autorelease机制是否在CCObject上运行呢？
	m_cdItemList = new CCDictionary();
	int t_gid;
	for(int i = 0; i<ts->m_snum; ++i){
		Script* t = (Script*) ts->scriptnodes->objectAtIndex(i);
		CCDictionary* t_caGItem = new CCDictionary();
		t_gid = t->getint("group");


		for(int j = 0; j<t->m_snum; ++j){
			Script* tm = (Script*) t->scriptnodes->objectAtIndex(j);
			CCLOG(">Read for item:%d",tm->getint("item"));

			int i_id = tm->getint("item");
			ItemCellData* ticd = new ItemCellData(i_id, tm->getint("sum"), tm->getint("lock"));	
			ticd->autorelease();
			t_caGItem->setObject(ticd,j + i /* *10000 */);			//[TO_CHANGE] <不再将item_id作为索引，作分离处理。

		}


		t_caGItem->autorelease();
		m_cdItemList->setObject(t_caGItem,t_gid);
	}
}

CCDictionary* StateCenter::f_get_itemlist(int i)			//Group|| 1 ItemCanUsse || 4 Equips
{
	CCDictionary* ret = (CCDictionary*) m_cdItemList->objectForKey(i);
	if(!ret){
		ret = new CCDictionary();
		m_cdItemList->setObject(ret,i);
		ret->autorelease();
	}
	return ret;
}

void StateCenter::f_add_item( Script* ts, bool bSilent)
{

	if(!m_cdItemList) m_cdItemList = new CCDictionary();			//防止列表为空

	t_ldb_cid = new CCDictionary();						//LDB用缓存列表
	
	CCDictionary* t_iicdCir = new CCDictionary();


	for(int i = 0; i<ts->m_snum; ++i){								//gruop
		
		Script* t = (Script*) ts->scriptnodes->objectAtIndex(i);
		int t_groupid = t->getint("group");

		CCDictionary* t_caGItem = (CCDictionary*) m_cdItemList->objectForKey(t_groupid);
		if(!t_caGItem) {
			t_caGItem = new CCDictionary();
			m_cdItemList->setObject(t_caGItem,t_groupid);
			t_caGItem->autorelease();
		}

		
		for(int j = 0; j<t->m_snum; ++j){							//item
			Script* tm = (Script*) t->scriptnodes->objectAtIndex(j);
			CCLOG(">Read for item:%d",tm->getint("item"));

			int i_id = tm->getint("item");

			
			//ItemCellData* ticd = (ItemCellData*) t_caGItem->objectForKey(i_id);
			ItemCellData* t_icd = new ItemCellData(i_id, tm->getint("sum"), tm->getint("lock"));
			t_ldb_cid->setObject(t_icd,i_id);			//LDB.
			t_icd->autorelease();

			m_sShowMask +=  CCString::createWithFormat("%d,",i_id)->getCString();			//保证item_id的唯一吧，双主键在这个地方确实有点麻烦
			t_iicdCir->setObject(t_icd,i_id);

		}
		
		int t_max = 0;
		CCDictElement* cde = NULL;
		vector<ItemCellData*> t_vi;

		if(t_groupid != 4){																//[TO_CHANGE]group = 4 <特别的不进行叠加
			CCDICT_FOREACH(t_caGItem,cde){
				t_max = max(t_max,cde->getIntKey());

				ItemCellData* t_icd = (ItemCellData*) cde->getObject();
				ItemCellData* ticd = (ItemCellData*) t_iicdCir->objectForKey(t_icd->type_id);

				if(ticd){												//已有物体
					t_icd->sum = t_icd->sum + ticd->sum;		//lock is of no use for adding item
					t_iicdCir->removeObjectForKey(t_icd->type_id);
				}

			}
		}


		CCDICT_FOREACH(t_iicdCir,cde){
			++t_max;
			t_caGItem->setObject(cde->getObject(),t_max);
		}
		t_iicdCir->removeAllObjects();
	}
	//////////////////////////////////////////////////////////////////////////
	CC_SAFE_RELEASE_NULL(t_iicdCir);

	if(bSilent) return;

	//t_ldb_cid->retain();
	m_sShowMask.erase(m_sShowMask.length()-1);

}


void StateCenter::f_add_item_show()
{
	CCString* t_csSql = CCString::createWithFormat("select itemid,name,icon from item_list where itemid IN (%s) union select itemid,name,icon from equip_list where itemid IN (%s)",m_sShowMask.c_str(),m_sShowMask.c_str());
	InfoTab::sharedInfoTab()->showldb("item_added",this,menu_selector(StateCenter::bback),t_csSql->getCString(),t_ldb_cid);
}


void StateCenter::bback(CCObject* c){
	CC_SAFE_RELEASE_NULL(t_ldb_cid);
	CCLOG("maybe it will boneeded?");
}

StateCenter::~StateCenter()
{
	CC_SAFE_RELEASE_NULL(t_ldb_cid);
	CC_SAFE_DELETE(g_sp);
	CC_SAFE_RELEASE_NULL(m_scTags);
	if(m_cdItemList){
		m_cdItemList->removeAllObjects();
		m_cdItemList->release();
	}
}

//void StateCenter::f_insert_item(int aid,int agroup, ItemCellData* aicd )
//{
//	if(!m_cdItemList) m_cdItemList = new CCDictionary();			//防止列表为空
//
//	CCDictionary* t_caGItem = (CCDictionary*) m_cdItemList->objectForKey(agroup);
//	if(!t_caGItem) {
//		t_caGItem = new CCDictionary();
//		m_cdItemList->setObject(t_caGItem,agroup);
//		t_caGItem->autorelease();
//	}
//
//	//Group == 4 || <装备切换	
//	t_caGItem->setObject(aicd,aid);
//
//}

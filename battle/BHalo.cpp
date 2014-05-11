#include "BHalo.h"
#include "SingleTon\BattleFiled.h"

class HaloStat{
public:
	map<string, int> vals;

};

BHalo::BHalo(Script* asp, CCObject* aec)
{
	mScp = asp;
	m_ecOwner = aec;

	miState = -1;
	motoNum = -100;
	mHaloOS = new HaloStat();

	miType = asp->getint("halotype");
	miOwType = asp->getint("owntype");

	mbTarProtect = false;
	mbOwnProtect = false;
	init();
}

BHalo::~BHalo()
{
	CC_SAFE_DELETE(mHaloOS);
	
	f_clean();
	m_caEObs.clear();
}

void BHalo::removEC(CCObject* aeTar)
{
	m_caEObs.erase(aeTar);
	CC_SAFE_DELETE(mHalosTates[aeTar]);
	mHalosTates.erase(aeTar);
}

void BHalo::checkEC(CCObject* aeTar, CCPoint acpmo, CCPoint acpaf)
{
	EChesses* tsrc = (EChesses*) m_ecOwner;
	EChesses* te = (EChesses*) aeTar;
	
	bool tmo = miDist < abs(acpmo.x - tsrc->pos.x) + abs(acpmo.y - tsrc->pos.y);
	bool taf = miDist < abs(acpaf.x - tsrc->pos.x) + abs(acpaf.y - tsrc->pos.y);

	if(tmo == taf) return;

	if(taf){
		HaloStat* ths = new HaloStat();
		mHalosTates.insert(make_pair(te, ths));
		m_caEObs.insert(te);
		te->m_pChara->mcaFHalos->addObject(this);
	}else{
		removEC(aeTar);
	}

	RefreshNums();
	RefreshState();
	RefreshOwner();
}

void BHalo::RefreshAttrs(const char* ast)
{
	miAttrs.clear();

	stringstream teststream;
	teststream<<ast;
	string s;
	int k;
	do{
		s.clear();
		teststream>>s;
		if(s.length() < 1) break;
		teststream>>k;
		miAttrs[s] += k;
		CCLog(">[BBuffer] Read attr :[%s||%d]",s.c_str(),k);
	} while(1);
}

void BHalo::RefreshState()
{
	if(mbTarProtect) return;
	Chara* tcTar;
	Script* stsp; 

	switch(miType){
	case 0:		// <默认扩散型 | 只有初始化时需要刷新
		{
			if(miState != -1) return;
			stsp = (Script*)mScp->scriptnodes->objectAtIndex(0);

			RefreshAttrs(stsp->getstring("vals"));

			for(map<CCObject*, HaloStat*>::iterator it = mHalosTates.begin(); it != mHalosTates.end(); ++it){
				HaloStat* ths = it->second;
				tcTar = ((EChesses*) it->first)->m_pChara;
				tcTar->RemoveMAttrs(it->second->vals);

				tcTar->AddonMAttrs(miAttrs);
				it->second->vals = miAttrs;
				tcTar->checkVals();
			}
			break;
		}
	}
	mbTarProtect = true;

}

void BHalo::RefreshOwner()
{
	if(mbOwnProtect) return;
	Script* stsp;

	switch (miOwType)
	{
	case 1:		// <无人值守型
		{
			CCLog(">[BHalo] RefreshOwner() | type == 1 |");
			if(miNums>0){
				miAttrs.clear();
			}else{
				stsp = (Script*)mScp->scriptnodes->objectAtIndex(1);
				RefreshAttrs(stsp->getstring("vals"));
				((EChesses*) m_ecOwner)->ShowTestF(mScp->getstring("text"), mScp->getint("color"));
			}

			Chara* tcSrc = ((EChesses*) m_ecOwner)->m_pChara;
			tcSrc->RemoveMAttrs(mHaloOS->vals);
			tcSrc->AddonMAttrs(miAttrs);
			break;
		}
	}
	mbOwnProtect = true;
}

bool BHalo::init()
{
	bool ret = false;
	do 
	{
		miDist = mScp->getint("distance");
		EChesses* tsrc = (EChesses*) m_ecOwner;
		EChesses* te;

		CCLog(">[BHalo] init() | dis:%d", miDist);
		CCLog(">[BHalo] Owner: %0.0f,%0.0f", tsrc->pos.x, tsrc->pos.y);

		map<pair<int,int>,EChesses*>::iterator it;
		for(it = BattleField::sharedBattleField()->mMapC.begin(); it != BattleField::sharedBattleField()->mMapC.end(); ++it){
			te = it->second;		
			CCLog(">[BHalo] init() | tar:%s | %0.0f,%0.0f", te->name.c_str(), te->pos.x, te->pos.y);
			
			if(te == tsrc)	continue;															// <和自己无关
			if(miDist < abs(te->pos.x - tsrc->pos.x) + abs(te->pos.y - tsrc->pos.y)) continue;	// <不在范围内就过滤
			if(te->group_id != tsrc->group_id) continue;										// <仅友军

			HaloStat* ths = new HaloStat();
			mHalosTates.insert(make_pair(te, ths));
			m_caEObs.insert(te);
			te->m_pChara->mcaFHalos->addObject(this);
		}
		
		mbTarProtect = false;
		
		miState = -1;
		miNums = m_caEObs.size();
		mbOwnProtect = (miNums == motoNum);

		CCLog(">[BHalo] Halo Nums Get:%d", miNums);

		RefreshState();
		RefreshOwner();
		RefreshNums();

		ret = true;
	} while (false);
	
	return ret;
}

void BHalo::RefreshNums()
{
	miNums = m_caEObs.size();
	if(miNums==0) miState = 0;
	else miState = 1;

	switch (miType)
	{
	case 0:
		mbTarProtect = true;
		break;
	default:
		break;
	}

	switch (miOwType)
	{
	case 1:
		if(motoNum>0 && miNums == 0) mbOwnProtect = false;
		if(motoNum == 0 && miNums>0) mbOwnProtect = false;
		break;
	default:
		break;
	}

	motoNum = miNums;
}

void BHalo::removeAllMov()
{
	f_clean();
	init();
}

void BHalo::removeClean(CCObject* aeTar){
	m_caEObs.erase(aeTar);
	CC_SAFE_DELETE(mHalosTates[aeTar]);
	mHalosTates.erase(aeTar);

	RefreshNums();
}

void BHalo::f_clean()
{
	Chara* tcTar;
	for(map<CCObject*, HaloStat*>::iterator it = mHalosTates.begin(); it != mHalosTates.end(); ++it){
		HaloStat* ths = it->second;
		tcTar = ((EChesses*) it->first)->m_pChara;

		tcTar->RemoveMAttrs(it->second->vals);
		tcTar->checkVals();
		CC_SAFE_DELETE(ths);
	}

	m_caEObs.clear();
	mHalosTates.clear();
}

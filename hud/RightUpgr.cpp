#include "RightUpgr.h"
#include "GameManager.h"
#include "Component\MapLayerComp.h"
#include "TOUI\HMenu.h"
#include "TOUI\RupCell.h"

#define SHOW_NUM(id, cname)																				\
	mvLabels[id]->setString(CCString::createWithFormat("%d",tChara->getvalue(cname))->getCString());	\
	ti = mUpvals[cname];																				\
	CCLog(">[RightUpgr] Show_Num %s,%d", cname, ti);											\
	if(ti){																								\
		RupCell* tRc = new RupCell(ti);																	\
		tRc->setPosition(mvLabels[id]->getPosition());													\
		addChild(tRc,1);																				\
		tRc->autorelease();																				\
	};									

#define SHOW_STR(id, cname)								\
	mvLabels[id]->setString(cname)

// 10 + 9
static const char* tlevel[] = {"E","D","C","B","A","S"};

void RightUpgr::f_init()
{
	miState = 0;

	CCSprite *tcsBg = CCSprite::create("Images/Rugr.png");
	tcsBg->setAnchorPoint(CCPointZero);
	tcsBg->setPosition(CCPointZero);
	addChild(tcsBg,-1);

	mvLabels.clear();
	CCLabelBMFont* labelAtlas;
	//////////////////////////////////////////////////////////////////////////

	labelAtlas = CCLabelBMFont::create("++++", FNT_CHN);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setScale(1.3);
	labelAtlas->setPosition(ccp(35,498));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);
	mvLabels.push_back(labelAtlas);						// < 0 -| 名字

	labelAtlas = CCLabelBMFont::create("++++", FNT_CHN);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setScale(1.3);
	labelAtlas->setPosition(ccp(66,530));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);
	mvLabels.push_back(labelAtlas);						// < 1 -| 等级

	labelAtlas = CCLabelBMFont::create("++++", FNT_CHN);
	labelAtlas->setAnchorPoint(CCPointZero);
	labelAtlas->setScale(1.3);
	labelAtlas->setPosition(ccp(230,498));
	labelAtlas->setColor(COLOUR_NORMAL);
	this->addChild(labelAtlas,1);
	mvLabels.push_back(labelAtlas);						// < 2 -| 职业

	float tfX = 90;
	float tfY = 410;

	for(int i = 0; i<5; ++i){
		labelAtlas = CCLabelBMFont::create("++++", FNT_CHN);
		labelAtlas->setAnchorPoint(CCPointZero);
		labelAtlas->setScale(1.3);
		labelAtlas->setPosition(ccp(tfX,tfY));
		labelAtlas->setColor(COLOUR_NORMAL);
		this->addChild(labelAtlas,1);
		mvLabels.push_back(labelAtlas);						// < 第一列 3 -7
		tfY -= 32;
	}
	
	tfX = 240;
	tfY = 410;
	for(int i = 0; i<5; ++i){
		labelAtlas = CCLabelBMFont::create("++++", FNT_CHN);
		labelAtlas->setAnchorPoint(CCPointZero);
		labelAtlas->setScale(1.3);
		labelAtlas->setPosition(ccp(tfX,tfY));
		labelAtlas->setColor(COLOUR_NORMAL);
		this->addChild(labelAtlas,1);
		mvLabels.push_back(labelAtlas);						// < 第二列 8 - 12
		tfY -= 32;
	}

	tfX = 279;
	tfY = 224;
	for(int i = 0; i<6; ++i){
		labelAtlas = CCLabelBMFont::create("S", FNT_CHN);
		labelAtlas->setAnchorPoint(CCPointZero);
		labelAtlas->setScale(1.3);
		labelAtlas->setPosition(ccp(tfX,tfY));
		labelAtlas->setColor(COLOUR_NORMAL);
		this->addChild(labelAtlas,1);
		mvLabels.push_back(labelAtlas);						// < 第二行 13 - 18
		tfY -= 34;
	}

}

// 
void RightUpgr::showUpgr(EChesses* aeTar)
{
	meTar = aeTar;
	Chara* tChara = aeTar->m_pChara;

	CCLog(">[RightUpgr] showUpgr() | %s",tChara->m_sName.c_str());
	//////////////////////////////////////////////////////////////////////////
	// <生成数值对

	vdata.clear();
	DBUtil::initDB("save.db");
	CCString* t_csSql = CCString::createWithFormat("select * from lvnums where cid = %d",tChara->m_iCharaID);
	CCLog("%s",t_csSql->getCString());
	vdata = DBUtil::getDataInfo(t_csSql->getCString(),NULL);
	int t_number = vdata.size();
	DBUtil::closeDB(); 

	if(t_number > 0){
		stringstream ss;
		string tsAddition;

		map<string,string> t_ssm = (map<string,string>) vdata.at(0);
		tsAddition = t_ssm.at("vpair");
		float tfRateLv = stof(t_ssm.at("str"));

		CCLog(">[RightUpgr] Pairs:%s", tsAddition.c_str());

		float tf_v;
		string ts_n;
		int ti_v;

		mUpvals.clear();
		ss << tsAddition;

		ss >> ts_n;
		while (ts_n.size()>0)
		{
			ss >> tf_v;
			tf_v *= tfRateLv;
					
			ti_v = floor(tf_v);
			tf_v -= ti_v;
			if(CCRANDOM_0_1() < tf_v) ++ti_v;

			//CCLog(">[RightUpgr] P-V:%s-%d", ts_n.c_str(), ti_v);
			mUpvals.insert(make_pair(ts_n, ti_v));
			tChara->plusvalue(ts_n, ti_v);

			ts_n.clear();
			ss >> ts_n;
		}

		//for(map<string,int>::iterator it = mUpvals.begin(); it != mUpvals.end(); ++it){
		//	CCLog("fuck:%s,%d", it->first.c_str(), it->second);
		//}

		
	}
	//////////////////////////////////////////////////////////////////////////
	// <显示

	mvLabels[0]->setString(CCString::createWithFormat("+%s+",tChara->m_sName.c_str())->getCString());
	mvLabels[1]->setString(CCString::createWithFormat("%i",tChara->getvalue("lv"))->getCString());
	mvLabels[2]->setString("XXXX");
	int ti;

	SHOW_NUM(3,"atk");
	SHOW_NUM(4,"def");
	SHOW_NUM(5,"mag");
	SHOW_NUM(6,"rst");
	SHOW_NUM(7,"hit");

	SHOW_NUM(8,"avg");
	SHOW_NUM(9,"spd");
	SHOW_NUM(10,"luk");
	SHOW_NUM(11,"mov");
	SHOW_NUM(12,"sou");

	SHOW_STR(13,tlevel[tChara->getvalue("tama_0")]);
	SHOW_STR(14,tlevel[tChara->getvalue("tama_1")]);
	SHOW_STR(15,tlevel[tChara->getvalue("tama_2")]);
	SHOW_STR(16,tlevel[tChara->getvalue("tama_3")]);
	SHOW_STR(17,tlevel[tChara->getvalue("tama_4")]);
	SHOW_STR(18,tlevel[tChara->getvalue("tama_5")]);

}

void RightUpgr::showOver()
{
	GameManager::sharedLogicCenter()->ml->m_Hmenu->dissmissRightUpg();
	((MapLayerComp*) GameManager::sharedLogicCenter()->ml->getComponent("controller"))->ActRelease();
}

void RightUpgr::showHub()
{
	showOver();			// <目前无进一步逻辑
}

void RightUpgr::byTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	CCLog(">[RightUpgr] TouchEnd | No Logic bended.");
	showHub();
}


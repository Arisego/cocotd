#include "EffectControler.h"
#include "packui/ConfigManager.h"
#include "StateCenter.h"
#include "packui/CharaS.h"


#include "DBUtil.h"

EffectControler* EffectControler::mSharedEffectControler = NULL;

EffectControler *EffectControler::sharedEffectControler()
{
	if (mSharedEffectControler == NULL)
	{
		mSharedEffectControler = new EffectControler();
		if (!mSharedEffectControler || !mSharedEffectControler->init())
		{
			CC_SAFE_DELETE(mSharedEffectControler);
		}

	}

	return mSharedEffectControler;

}

void EffectControler::purgeSharedEffectControler()
{
	CC_SAFE_DELETE(mSharedEffectControler);
}

bool EffectControler::init(){
	do 
	{
		sp = new Scriptor();
		m_iState = -1;
		m_caLocks = NULL;
		m_Esp = NULL;
		m_bSilent = false;
		m_tl = NULL;

		return true;
	} while (0);
	return false;
}

void EffectControler::md_use_skill( InterfaceEffect* iv, int a_skill_id, CCObject* src)
{
	//////////////////////////////////////////////////////////////////////////
	vdata.clear();
	m_src = NULL;

	DBUtil::initDB("save.db");
	vdata = DBUtil::getDataInfo(CCString::createWithFormat("select * from skill_list where itemid = %d",a_skill_id)->getCString(),NULL);
	DBUtil::closeDB(); 

	if(vdata.size()<1) exit(0x5003);
	//////////////////////////////////////////////////////////////////////////
	m_src = (Chara*) src;

	map<string,string> t_ssm = (map<string,string>) vdata.at(0);
	//int item_id	 =	 stoi(t_ssm.at("itemid"));			
	//int usecase	 =	 stoi(t_ssm.at("usecase"));
	//if(! (usecase&1)) exit(0x5003);
	//CC_SAFE_DELETE(sp);
	sp->re_init();
	sp->parse_string(t_ssm.at("side_effect"));
	m_caLocks = sp->m_caScript;
	if(m_caLocks) m_caLocks->retain();
	

	m_iTarget	=	 stoi(t_ssm.at("target"));	
	m_iState	=	 1;
	m_sEffect	=	 t_ssm.at("deffect");
	miView		=	 iv;

	sp->re_init();
	sp->parse_string(m_sEffect);
	CCLOG("Prepar ready for md_act_skill.");
	m_Esp = sp->m_caScript;
	m_Esp->retain();

	iv->get_target();
	
}

bool EffectControler::DerLock(){
	bool ret = true;
	if(!m_caLocks){
		CCLOG(">Lock array is NUll.");
		return true;
	}
	for(int i = 0; i<m_caLocks->count(); ++i){
		Script* t_scp = (Script*) m_caLocks->objectAtIndex(i);
		switch(t_scp->getint("type")){
		case(1):		//	1为属性大于判定 Int only   -- check lock state????
			{
				ret = ret&(m_src->getvalue(t_scp->getstring("name")) > t_scp->getint("value"));
				break;
			}
		}
	}

	return ret;
}

void EffectControler::md_act_skill( CCObject* src, CCArray* tl )			//ldb mini 未关闭的情况下将会不停的尝试调用effect
{
	if(!DerLock()) return;			
	m_iState = 2;		//做标记用，如果最后都不使用的话就去除。
	
	m_tl = tl;

	m_iCurS = -1;
	m_iSumS = m_Esp->count();
	f_sp_gonext();
}

void EffectControler::f_sp_act()
{
	DerSP((Script*) m_Esp->objectAtIndex(m_iCurS));
}

void EffectControler::f_sp_gonext()
{
	++m_iCurS;
	if(m_iCurS < m_iSumS){
		f_sp_act();
	}else{
		
		f_effect_over();
	}
}

void EffectControler::f_effect_over()			//Dont call it manually inside efc, call it from Implements for better action control.
{
	m_iState = 0;
	if(miView) miView->effect_over();	
	if(m_bSilent) f_clear();
	//sp->re_init(); //CC_SAFE_DELETE(sp);	//DONE:注意Scriptor多次使用过程中内存空间的释放 -> Scriptor已有reinit机制
}

void EffectControler::DerSP( Script* asp )
{
	for(int i = 0; i<asp->m_snum; ++i){
		Script* t_sp = (Script*) asp->scriptnodes->objectAtIndex(i);
		switch(t_sp->type){
		case(sChange):		/* <改变数值：对象为已经登记的源或/和目标 */
			{
				DerChange(t_sp);
				break;
			}
		case(sShowText):	
			{
				miView->show_text(t_sp->getstring("content"));
				break;
			}
		case(sItem):	/* <给予物品 */
			{
				
				StateCenter::sharedStateCenter()->f_add_item(t_sp,m_bSilent);
				//m_bSilent = false;
				if(miView) miView->refresh_view();
				else f_sp_gonext();

				if(!m_bSilent) StateCenter::sharedStateCenter()->f_add_item_show();
				break;
			}
		case(sAction):
			{
				CharaS::sharedCharaS()->m_bNotifate = !m_bSilent;
				CharaS::sharedCharaS()->load_chara(t_sp);
				//m_bSilent = false;
				if(miView) miView->refresh_view();
				else f_sp_gonext();
				break;
			}
		case(sMapAction):
			{
				if(miView) miView->handlesp(t_sp);
				else f_sp_gonext();
				break;
			}

		}
	}
	
}

void EffectControler::DerChange(Script* asp){
	switch(asp->getint("type")){
	case(1):
		{
			if(m_src){
				m_src->addvalue(asp->getstring("name"),asp->getint("value"));
			}
			break;
		}
	case(2):
		{
			CCObject* pSC;
			CCARRAY_FOREACH(m_tl,pSC){
				((Chara*) pSC)->addvalue(asp->getstring("name"),asp->getint("value"));
			};
			break;
		}

	}
}

void EffectControler::f_clear()
{
	CC_SAFE_RELEASE_NULL(m_caLocks);
	CC_SAFE_RELEASE_NULL(m_Esp);
	m_bSilent = false;			//一次性
}

void EffectControler::md_use_item( InterfaceEffect* iv, int a_item_id )
{
	//////////////////////////////////////////////////////////////////////////
	vdata.clear();
	m_src = NULL;

	DBUtil::initDB("save.db");
	vdata = DBUtil::getDataInfo(CCString::createWithFormat("select * from item_list where itemid = %d",a_item_id)->getCString(),NULL);
	DBUtil::closeDB(); 

	if(vdata.size()<1) exit(0x5003);
	//////////////////////////////////////////////////////////////////////////

	map<string,string> t_ssm = (map<string,string>) vdata.at(0);
	//int item_id	 =	 stoi(t_ssm.at("itemid"));			
	//int usecase	 =	 stoi(t_ssm.at("usecase"));
	//if(! (usecase&1)) exit(0x5003);
	//CC_SAFE_DELETE(sp);
	sp->re_init();
	sp->parse_string(t_ssm.at("side_effect"));
	m_caLocks = sp->m_caScript;
	if(m_caLocks) m_caLocks->retain();


	m_iTarget	=	 stoi(t_ssm.at("target"));	
	m_iState	=	 1;
	m_sEffect	=	 t_ssm.at("effect");
	miView		=	 iv;

	sp->re_init();
	sp->parse_string(m_sEffect);
	CCLOG("Prepar ready for md_act_item.");
	m_Esp = sp->m_caScript;
	if(m_Esp) m_Esp->retain();
	else return;
	//if(!m_Esp) return;

	if(iv) iv->get_target();
	else {
		m_bSilent = true;			//NULL iv means this item use is called by system itself
		md_act_item(NULL);
	}
}

void EffectControler::md_act_item( CCArray* tl )
{
	if(!DerLock()) return;						//TODO: 如果需要对物品使用进行禁用，可以将这个判断移动到md_use_item上。
	m_iState = 2;		//做标记用，如果最后都不使用的话就去除。

	m_tl = tl;

	m_iCurS = -1;
	m_iSumS = m_Esp->count();
	f_sp_gonext();
}

EffectControler::~EffectControler()
{
	//sp->re_init();
	f_clear();
	CC_SAFE_DELETE(sp);
}

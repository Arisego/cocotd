#ifndef __EFFECT_CONTROLER_H__
#define __EFFECT_CONTROLER_H__

#include "cocos2d.h"
#include "utils/Scriptor.h"
#include "packui/InterfaceEffect.h"
#include "packui/CharaS.h"

USING_NS_CC;
using namespace std;

/*
	EffectControler
	<负责技能和物品的特效播放管理
	<注意，这个单例是负责对实现了特效播放接口的对象进行管理的，并不实际负责特效播放。
	<目前同一时间只能播放一种特效，并不是所有的地图效果都是特效，本单例主要用于技能特效的播放。
	<为了保证同时播放多个特效，考虑不作为单例运行。
	<EffectOver是作为特效结束的，但是并没有重置标志位。
	<[TODO]通过包装回调来控制播放是否结束。
	<[TODO]interface_添加input_query来保证并行和用户输入之间的冲突
	<[TODO]interface_为了保证可暂停性，所有的特效保存在统一的节点下

*/
class EffectControler
{
public:

	static EffectControler* sharedEffectControler();
	static void purgeSharedEffectControler();

	void md_use_skill(InterfaceEffect* iv, int a_skill_id, CCObject* src);
	void md_act_skill(CCObject* src, CCArray* tl);
	void md_use_item(InterfaceEffect* iv, int a_item_id);
	void md_act_item(CCArray* tl);

	void f_clear();

	string m_sEffect;
	CCArray* m_caLocks;
	int m_iTarget;

	bool m_bSilent;

	~EffectControler();
protected:
	Chara* m_src;
	CCArray* m_tl;

	Scriptor* sp;
	InterfaceEffect* miView;

	int m_iState;

	vector<map<string,string>> vdata;
	//map<int,string> m_isCache;		//??:技能缓存

	static EffectControler* mSharedEffectControler;
	bool init();

	CCArray* m_Esp;
	int m_iCurS, m_iSumS;
	void f_sp_act();
	
	void DerSP(Script* asp);

	void DerChange(Script* asp);
	

public:
	string getESp();
	bool DerLock();		// <对技能锁进行检查

	void f_sp_gonext();
	void f_effect_over();
};

#endif
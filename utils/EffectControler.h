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
	<�����ܺ���Ʒ����Ч���Ź���
	<ע�⣬��������Ǹ����ʵ������Ч���ŽӿڵĶ�����й���ģ�����ʵ�ʸ�����Ч���š�
	<Ŀǰͬһʱ��ֻ�ܲ���һ����Ч�����������еĵ�ͼЧ��������Ч����������Ҫ���ڼ�����Ч�Ĳ��š�
	<Ϊ�˱�֤ͬʱ���Ŷ����Ч�����ǲ���Ϊ�������С�
	<EffectOver����Ϊ��Ч�����ģ����ǲ�û�����ñ�־λ��
	<[TODO]ͨ����װ�ص������Ʋ����Ƿ������
	<[TODO]interface_���input_query����֤���к��û�����֮��ĳ�ͻ
	<[TODO]interface_Ϊ�˱�֤����ͣ�ԣ����е���Ч������ͳһ�Ľڵ���

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
	//map<int,string> m_isCache;		//??:���ܻ���

	static EffectControler* mSharedEffectControler;
	bool init();

	CCArray* m_Esp;
	int m_iCurS, m_iSumS;
	void f_sp_act();
	
	void DerSP(Script* asp);

	void DerChange(Script* asp);
	

public:
	string getESp();
	bool DerLock();		// <�Լ��������м��

	void f_sp_gonext();
	void f_effect_over();
};

#endif
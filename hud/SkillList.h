#ifndef __SKILL_LIST_H__
#define __SKILL_LIST_H__

#include "byeven/BYCocos.h"
#include "packui/Container.h"
#include "packui/CharaS.h"

//////////////////////////////////////////////////////////////////////////
// Skill Meta
class SkillListMeta : public Container
{
public:
	SkillListMeta();
	void setSKLabel(const char* aname);		// <���ñ�ǩ������
	void setValue(int ai);					// <���ñ�������ֵ��ע�⵱ǰ�����ظ�����

private:
	CCLabelTTF* mLT_Name;

};

//////////////////////////////////////////////////////////////////////////
//

class SkillList : public BYLayerDescendant // ����BattleMenu���й���
{
public:
	bool f_init();

	void setactivator(CCObject* target, SEL_MenuHandler selector);
	void SKM_Back(CCObject* to);

	bool setChara(Chara* ac);		// <������ʾ��ɫ��

	const char* getval(const char* aname,int ait);
private:
	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate();

	Chara* m_pChara;
	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				// <����ID - vdata id

	vector<SkillListMeta*> m_viSkmL; 
	void RefreshSm();					// <ˢ����ʾ
};

#endif
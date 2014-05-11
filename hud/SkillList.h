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
	virtual void onSelect() override;

};

//////////////////////////////////////////////////////////////////////////
//

class SkillList : public BYLayerDescendant // ����BattleMenu���й���
{
public:
	bool f_init();

	void setactivator(CCObject* target, SEL_MenuHandler selector);
	void SKM_Back(CCObject* to);

	bool setChara(Chara* ac, int aiFlag = 0x01);		// <������ʾ��ɫ��

	BSkill* getval(int ait);		// <���ָ���ļ���
	int miFlag;						// <��� 0x0001 ��ͨ | 0b00010 ����Ԥ��[Judge()]
private:

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate();

	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				// <����ID - vdata id

	vector<SkillListMeta*> m_viSkmL; 
	void RefreshSm();					// <ˢ����ʾ
	set<int>	m_siSkillEn;			// <���ܿ�ʹ���б�

public:
	Chara* m_pChara;
	////////////////////////////////////////////////////////////////////////////
	// <���ܽ���
	bool setctns(Chara* ac, const char* sMask);			// <���ݴ���ļ����б����˵�
	void randomact();									// <AI���ѡ��һ�����в���
	void randomactfc();									// <ʵ�ʲ�����
};

#endif
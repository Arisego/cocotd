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
	void setSKLabel(const char* aname);		// <设置标签的文字
	void setValue(int ai);					// <设置背景动画值，注意当前不可重复调用

private:
	CCLabelTTF* mLT_Name;
	virtual void onSelect() override;

};

//////////////////////////////////////////////////////////////////////////
//

class SkillList : public BYLayerDescendant // 参照BattleMenu进行构建
{
public:
	bool f_init();

	void setactivator(CCObject* target, SEL_MenuHandler selector);
	void SKM_Back(CCObject* to);

	bool setChara(Chara* ac, int aiFlag = 0x01);		// <设置显示角色。

	BSkill* getval(int ait);		// <获得指定的技能
	int miFlag;						// <旗标 0x0001 普通 | 0b00010 攻击预判[Judge()]
private:

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate();

	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				// <技能ID - vdata id

	vector<SkillListMeta*> m_viSkmL; 
	void RefreshSm();					// <刷新显示
	set<int>	m_siSkillEn;			// <技能可使用列表

public:
	Chara* m_pChara;
	////////////////////////////////////////////////////////////////////////////
	// <技能接续
	bool setctns(Chara* ac, const char* sMask);			// <根据传入的技能列表弹出菜单
	void randomact();									// <AI随机选择一个进行操作
	void randomactfc();									// <实际操作体
};

#endif
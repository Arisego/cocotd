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

};

//////////////////////////////////////////////////////////////////////////
//

class SkillList : public BYLayerDescendant // 参照BattleMenu进行构建
{
public:
	bool f_init();

	void setactivator(CCObject* target, SEL_MenuHandler selector);
	void SKM_Back(CCObject* to);

	bool setChara(Chara* ac);		// <设置显示角色。

	const char* getval(const char* aname,int ait);
private:
	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate();

	Chara* m_pChara;
	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				// <技能ID - vdata id

	vector<SkillListMeta*> m_viSkmL; 
	void RefreshSm();					// <刷新显示
};

#endif
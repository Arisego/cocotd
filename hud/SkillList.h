#ifndef __SKILL_LIST_H__
#define __SKILL_LIST_H__

#include "byeven/BYCocos.h"
#include "packui/Container.h"

//////////////////////////////////////////////////////////////////////////
// Skill Meta
class SkillListMeta : public Container
{
public:
	SkillListMeta();
	void setSKLabel(const char* aname);
	void setValue(int ai);

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
private:
	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate();
};

#endif
#ifndef __TO_SKILL_V_A_H__
#define __TO_SKILL_V_A_H__

#include "Container.h"

struct SkillMeta:CCObject
{
	int id;
	string name;
	string discription;

};

class TOSkill :public Container
{
public:
	TOSkill();

private:
	CCLabelTTF* mLT_Name;
	SkillMeta* m_smData;
};

class TOTama : public Container
{
	;
};

class TOVoice : public Container
{
	;
};

class TOASkill : public Container
{
	;
};

#endif
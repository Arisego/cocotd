#ifndef __B_SKILL_H__
#define __B_SKILL_H__

/*
*	< 单个的技能及其管理类
*	< 将技能的使用统一起来
*/
#include "cocos2d.h"
#include "tinyxml\tinyxml.h"

using namespace std;
USING_NS_CC;

class BSkill : public CCObject
{
public:
	BSkill(map<string,string> &a_ssm);

	int miDamage;		/* <伤害 || 仅做预估用 */
	int miNext;			/* <下一个等级的编号||可能会在多等级技能中用到 */
	int miRange;		/* <???? */
	int miSFlag;		/* <技能的阶段标记 | bit flag */
	int miType;			/* <主动/被动 */
	int miTarget;

	int miPhase;		/* <主动技能的可触发使用阶段 */

	string msAddition;	/* <范围信息 */
	string msSideEffect;/* <技能的使用限制条件 */
	string msName;
	string msDeffect;	/* <列表UI用技能脚本 */
	string msScript;	/* <脚本 */

	int m_iUseCase;		
	int m_iLink;	
};

class BSkillList
{
public:
	BSkillList();
	~BSkillList();

	void clear();
	void insert(int aia, int aib);
	int size();
	void saveXml(TiXmlElement *LockElement);

	void inform();

	std::map<int,int> m_viSkills;
	BSkill* getSkill(int id);
private:
	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				// <技能ID - vdata id
	CCDictionary* mcd_Skills;

	static BSkillList* mSharedBSkillList;
public:
	static BSkillList *sharedBSkillList();
	static void purgeSharedBSkillList();
	bool init();
};

#endif
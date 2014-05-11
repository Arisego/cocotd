#ifndef __SKILL_JUDGE_H__
#define __SKILL_JUDGE_H__

#include "cocos2d.h"
#include "Component\AIComponent.h"
#include "Component\EChessComp.h"

USING_NS_CC;

class SkillJudge
{
private:
	Scriptor* sp;

protected:
	static SkillJudge* mSharedSkillJudge;

	bool init();
	//vector<map<string,string>> vdata;
	//map<int,int> m_miiViDb;				/* <技能ID - vdata id */
	Chara* mpchara;

	int miJudSkillRes;					/* <得出的结果 */
	set<int> msi_SkillSet;				/* <技能的集合 || 保存为m_miiViDd的id */

	void NoSkillFit();
	void FilterSkills();				/* <筛选出可使用的技能 */
	int ValueSkill(int aid);			/* <评估技能的价值 */
	void FindSkill();					/* <寻找估值最高的技能 */
public:
	~SkillJudge();

	static SkillJudge *sharedSkillJudge();
	static void purgeSharedSkillJudge();

	void JudgeSkill(AIComponent* tai);	/* <下次攻击技能的入口函数 */
	bool CheckSkill(AIComponent* tai);	/* <检查下次攻击技能是否可用并释放 */

	bool CheckNorM(CCObject* tai, CCObject* target = NULL, bool abact = false);	/* <检查是否可以进行普通攻击 || 实际入常 EChess | EChess */

	bool mbJudging;						/* <判定保护 */
	


	
};

#endif
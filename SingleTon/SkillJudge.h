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
	//map<int,int> m_miiViDb;				/* <����ID - vdata id */
	Chara* mpchara;

	int miJudSkillRes;					/* <�ó��Ľ�� */
	set<int> msi_SkillSet;				/* <���ܵļ��� || ����Ϊm_miiViDd��id */

	void NoSkillFit();
	void FilterSkills();				/* <ɸѡ����ʹ�õļ��� */
	int ValueSkill(int aid);			/* <�������ܵļ�ֵ */
	void FindSkill();					/* <Ѱ�ҹ�ֵ��ߵļ��� */
public:
	~SkillJudge();

	static SkillJudge *sharedSkillJudge();
	static void purgeSharedSkillJudge();

	void JudgeSkill(AIComponent* tai);	/* <�´ι������ܵ���ں��� */
	bool CheckSkill(AIComponent* tai);	/* <����´ι��������Ƿ���ò��ͷ� */

	bool CheckNorM(CCObject* tai, CCObject* target = NULL, bool abact = false);	/* <����Ƿ���Խ�����ͨ���� || ʵ���볣 EChess | EChess */

	bool mbJudging;						/* <�ж����� */
	


	
};

#endif
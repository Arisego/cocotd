#ifndef __SITU_JUDGE_H__
#define __SITU_JUDGE_H__

#include "cocos2d.h"
#include "utils/EChess.h"

USING_NS_CC;

class SituJudge
{
private:
	static SituJudge *mSharedSituJudge;
	EChesses* meThink;

	map<pair<int,int>,float> mVals;	/* <评估的数值对||A */
	set<pair<int,int>>	   m_Locks; /* <禁止站立的集合 */

	bool init();

	void DpsVals(EChesses* aSrc);								/* <扩散该单位属性 */
	int JudgeDamageSc(EChesses* aSrc, EChesses* aTar);			/* <计算攻击输出评分 */
	int JudgeHelpSc(EChesses* aSrc, EChesses* aTar);			/* <计算辅助输出评分 */
	void DpsValues(int av, int ax, int ay, int arange);			/* <实际进行值的填充 */
	void FilterVals();											/* <根据地形进行否决和加成 */
public:
	SituJudge();
	~SituJudge();

	static SituJudge *sharedSituJudge();
	static void purgeSharedSituJudge();

	void JudgeMove(EChesses* tar);	/* <判定入口 || 单位选择最优地形  */

};

#endif
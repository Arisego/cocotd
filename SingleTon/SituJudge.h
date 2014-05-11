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

	map<pair<int,int>,float> mVals;	/* <��������ֵ��||A */
	set<pair<int,int>>	   m_Locks; /* <��ֹվ���ļ��� */

	bool init();

	void DpsVals(EChesses* aSrc);								/* <��ɢ�õ�λ���� */
	int JudgeDamageSc(EChesses* aSrc, EChesses* aTar);			/* <���㹥��������� */
	int JudgeHelpSc(EChesses* aSrc, EChesses* aTar);			/* <���㸨��������� */
	void DpsValues(int av, int ax, int ay, int arange);			/* <ʵ�ʽ���ֵ����� */
	void FilterVals();											/* <���ݵ��ν��з���ͼӳ� */
public:
	SituJudge();
	~SituJudge();

	static SituJudge *sharedSituJudge();
	static void purgeSharedSituJudge();

	void JudgeMove(EChesses* tar);	/* <�ж���� || ��λѡ�����ŵ���  */

};

#endif
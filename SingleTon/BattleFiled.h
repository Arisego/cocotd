#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"
#include "utils\EChess.h"
#include "utils\Scriptor.h"

/*
	<ս��������
	-<����Ǳ���ΪOVERʱ�ᵼ��ML����BM��ע����������ʾ��
*/
class BattleField
{
public:
	static BattleField *sharedBattleField();
	static void purgeSharedBattleField();

	~BattleField();
	void Clean();

	bool IsOver();
	//////////////////////////////////////////////////////////////////////////
	// <ս������
	map<pair<int,int>,EChesses*> mMapC;	// <ս��λ�õǼ�
	void SetChess(EChesses* ae,int ax, int ay);			// <�������� | ��ʼ����
	void InitChessRefreh();				// <ˢ������ | ��ʼ����
	void ChessMoved(EChesses* ae, CCPoint astart, CCPoint aend); // <�����ƶ�������ͳ˧��ֵ
	void Judge();						// <�ж�

	bool HasChess(int ax, int ay);		// <����һ�����Ƿ��������

	//////////////////////////////////////////////////////////////////////////
	// <ս������
	void SetUp(EChesses* aSrc, CCArray* aTar,Script* sp = NULL);		// <ս�����ýӿ�
	void AMOver();														// <һ�ι�����������ս�����

	void SetSrc(EChesses* aSrc);
	void SetTars(CCArray* aTar);

	void PlayEffectSp(const char* asname, CCPoint end);					// <����һ��Sprite�����ƶ���ָ���ĵ㡣

	//////////////////////////////////////////////////////////////////////////
	// <�ٴι���
	EChesses* BackChess1;
	EChesses* BackChess2;
	EChesses* BackChess3;

	void CheckBackCh();		// <��EChessTar�ж�ȡ���Է�����������λ
	bool TestBackCh(EChesses* atar);		// <����Ƿ���Է��������з���

	bool LogicContinue();
	bool NormalAttackC();	// <��ͨ�����ļ��
	int	 miState;			// <ս���׶α�־λ 0 - �ȴ����ɷ����� || 1 - �Է�������б���ѯ�� || 2 - ��������η��� 

	//////////////////////////////////////////////////////////////////////////
	// <��̬�˺�Ԥ��
	void setBattle(bool ab);
	bool mbIsInBattle;		// <�Ƿ���Ҫ����ս��Ԥ��

	void PreJudge(EChesses* atar);

	//////////////////////////////////////////////////////////////////////////
	// <��ͼ�ű�
	void ClearBF();				// <�������ɨս��
	void InitBfSp(const char* pSz);

	Scriptor* mSpLinker;	// <������
	void ShowChess(EChesses* atar);

protected:
	virtual bool init();
	void CheckOver();								// <����Ƿ������������

	void DepRemoveLead( int centx, int centy, int range, int val );
	void DepLead( int centx, int centy, int range, int val );

	void SetLead(EChesses* te);
	int DerLead( int val,int cx, int cy,int dx, int dy );	// <����val��ֵ���ж��õ���ͳ˧�ӳ�
	void DerLead( int val,int cx, int cy );					// <������ɢ
	void UnDerLead( int val,int cx, int cy );				// <�Ƴ������Ӱ��
	
	//////////////////////////////////////////////////////////////////////////
	// <ս����ʾ
	int miSFlag;	// <���ս������״̬ || 0 - Srcδδ���� | 1 -Src�Ѿ����� �ȴ����ܵĿ�ʼ���ж� | 2 - ս��������һ���ж� <<< �Ժ���ж����Ժ�mistate���
	
public:
	bool mbIsMagic; // <�Ƿ���ħ������
	void RefreshStats();	// <����������ʾ
	EChesses* meTarget;		// <��ǰ��������ʾ���ұߵ�λ

	void ActionFac();		// <������ʵ��ִ��
	bool CalRate(EChesses* tar_o,int hit_rate_base);
	//void CheckBlood();		// <��Ѫ�����м��

private:
	EChesses* meOrig;		// <�����Ҫ��ָ��
	EChesses* meSrc;		// <�����ķ�����	| ������������Chara�ڽ���
	CCArray* meTar;			// <����
	Script*	 mspVals;		// <����ս���Ķ�Ϊ�������

	CCDictionary* mSpEvList;// <�¼��б�

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
	void setMeTar(CCArray* atar); // <meTar�ķ�װ�ӿ�
};

#endif
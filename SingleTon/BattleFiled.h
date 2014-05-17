#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"
#include "utils\EChess.h"
#include "utils\Scriptor.h"
#include "TOUI\LogicTree.h"

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
	map<pair<int,int>,EChesses*> mMapC;							// <ս��λ�õǼ�
	void SetChess(EChesses* ae,int ax, int ay);					// <�������� | ��ʼ������ʱ����
	void InitBattleFiled();										// <BattleFiled�ڲ���ʼ�� | ��ֵ
	void InitChessRefreh();										// <ˢ������ | ��ʼ�����ʱ����
	void ChessMoved(EChesses* ae, CCPoint astart, CCPoint aend); // <�����ƶ�������ͳ˧��ֵ
	void Judge(Script* ascp = NULL);		// <�ж� || ��� | �˺� | ����

	bool HasChess(int ax, int ay);								// <����һ�����Ƿ��������

	//////////////////////////////////////////////////////////////////////////
	// <ս������
	void SetUp(EChesses* aSrc, CCArray* aTar,Script* sp = NULL);		// <ս�����ýӿ�
	void AMOver();														// <һ�ι�����������ս�����

	void SetSrc(EChesses* aSrc);
	void SetTars(CCArray* aTar);

	void PlayEffectSp(CCObject* echesscom, const char* asname);								// <����һ��Sprite�����ƶ���ָ���ĵ㡣
	void PlayEffectSpFr(CCObject* tmpOwner, Script* sp);									// <����һ��Sprite(Frame)

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

	EChesses* meOrig;		// <�����Ҫ��ָ��
public: 
	EChesses* meSrc;		// <�����ķ�����	| ������������Chara�ڽ���
private:
	CCArray* meTar;			// <����
	Script*	 mspVals;		// <����ս���Ķ�Ϊ�������

	CCDictionary* mSpEvList;// <�¼��б�

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
	void setMeTar(CCArray* atar); // <meTar�ķ�װ�ӿ�
public:
	//////////////////////////////////////////////////////////////////////////
	// <���ܽ���
	bool fGetCont( const char* sname);

	//////////////////////////////////////////////////////////////////////////
	// <һ�ֽ�������
	map<pair<int,int>,EChesses*>::iterator m_EIt;		// <�����������ݵĵ�����
	void InitCheck();									// <��ʼ�����
	bool ActChack();									// <������������Ҫʱ��ġ�
	void UnitDead();									// <�е�λ�����ˣ����д���

	void ActOver();										// <һ�ֽ����Ľ���֪ͨ�����

	//////////////////////////////////////////////////////////////////////////
	// <�������
	bool ECIsSrc(EChesses* aTar);
	bool ExpCheck();									// <��龭��ֵ
	void ExpShow();							// <��ʾ����ֵ

	//////////////////////////////////////////////////////////////////////////
	//

	void InitBF();							// <�ű���س�ʼ������ | ��BF����ű��ٿط�Χ֮��

	// <ʤ������
	LogicCell* mlp_Win;
	LogicCell* mlp_Lose;
private:
	Script* Search_script(const char* asScp);
	void	Ex_script(Script* aScp, const char* asScp);

	set<string> mss_Win, mss_Lose;			// <��ֹmap�޶˵ı䳤
	map<string,LogicCell*> msb_Win, msb_Lose;		// <ʤ������

	void ReadLogicPointScpW(LogicCell* atar, Script* ascp);
	void ReadLogicPointScpL(LogicCell* atar, Script* ascp);

	void onEvent(const char* asEventName);	// <�¼�����ĺ��Ľӿ�

public:
	void onDeadEvent(EChesses* tar);		// <��λ��������ж������� | ��BF�н���Ǽ�
	
	//////////////////////////////////////////////////////////////////////////
	// <�浵����

	void setSaveState();					// <���ô浵״̬ | ʹ��WM�ܹ���������
	void setLoadState();					// <���ö���״̬ | �ڶ���ǰ����������
	void unsetSaveState();					// <����浵״̬ | ��ֹ����Ϸѭ�����Ӱ��
	void Load_Script(Script* ascp);			// <ս�������ӿ� | ����MapLayer���� | ��ע�⡿ս���д󲿷�������ǽ�ֹ�浵��
	void Load_Over();						// <������ɽӿ� | StateCenter������ɺ���ã���ֹ����wm��bm�ĳ�ͻ

	bool mbNeedSave;						// <�Ƿ���Ҫ�浵 | ����ʼ����setSaveState���޸� | ͬʱ�е������ı�־

	int miActCount;							// <���ֹ����еļ�����

public:	
	void ClearTrigFlag();					// <����������
	bool mbTrigged;							// <�����Ƿ񱻴���

	void Trigged();										// <����������ʱ����
	int TestPrior(CCObject* acSrc = NULL, CCObject* acTar = NULL);	// <���������λ���ٶ����ȼ� | Chara
	int DerLocks(CCObject* acSrc, CCObject* acTar, const char* asLScp);			// <��鼼���Ƿ�ɴ��� | ��
	bool PUSkillFilt(CCObject* acSrc, CCObject* acTar, int aiFlag, const char* asmask = "");				// <��src��tar�޶�flag�ļ����б����ɺ͵��� | ����|����|

	//void CleanAllAct();		// <�����������еĽű���ɵ�Ӱ�� | �ⲻ��һ������������
	Script* mCachedJudge;	// <ÿ���ж�����ͣʱ�Ļ���

	void ResumeJudge();		// <����б�ͣ����Judge��ָ��Ǵ��ж�
	void JudgeEnti(Script* asp);		// <Judge�ж���ʵ��

	//std::string msCacheSk;
	//void PunpOut();				// <�����϶�󵯳��˵��õĺ���
	void ActSkill(CCObject* src, Script* asp, CCObject* atar = NULL);					// <ĳ����λ�������ͷ�
	CCObject* mCachedChess;			// <�����������

	int getDisST();					// <��þ��� Src - Tar | ʹ��Tar�б�ĵ�һ����λΪ��׼

	void PlaySpxEC(CCObject* atar, Script* asp);		// <��ĳ�������ϲ���ָ����SPX����

	CCObject* meConS;			// <�������Ŀ�����

	bool mbNeLock;				// <�Ƿ���Ҫ����MLC��������

	map<int, CCPoint> mCachedSPp;	// <���漼���ж��е����õ�
	map<int, int>	mCachedSPpScs;	// <AI�ã���Ӧ���ܵ�����

	CCObject* meCtnTarget;			// <������Ŀ�� | ʵ������ EChess || CCArray��
private:
	bool CheckSkill(CCObject* acSrc, CCObject* acTar, int sid);	
	CCObject* meLastRe;				// <���һ��������λ
	int CalDamage(Script* asp);		// <�˺�����

public:
	void PlaySpxPo(CCPoint apos, Script* asp);	// <��ָ����λ�ò��Ŷ���

	int miSeType;		// <������Ч����ת

	bool mbCurMagic;	// <��ǰ�Ľű��Ƿ���ħ��
	void PlaySe(CCObject* aeChara);	// <������Ч | Chara

	int	miLastSkillID;			// <��һ�������ļ���ID

	void ClearReASkill();		// <����������Чʱ�������
};


#endif
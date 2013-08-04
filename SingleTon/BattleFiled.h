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

	void Clean();

	bool IsOver();
	//////////////////////////////////////////////////////////////////////////
	// <ս������
	map<pair<int,int>,EChesses*> mMapC;	// <ս��λ�õǼ�
	void SetChess(EChesses* ae,int ax, int ay);			// <�������� | ��ʼ����
	void InitChessRefreh();				// <ˢ������ | ��ʼ����
	void ChessMoved(EChesses* ae, CCPoint astart, CCPoint aend); // <�����ƶ�������ͳ˧��ֵ


	//////////////////////////////////////////////////////////////////////////
	// <ս������
	void SetUp(EChesses* aSrc, CCArray* aTar,Script* sp = NULL);		// <ս�����ýӿ�
	void AMOver();									// <һ�ι�����������ս�����

protected:
	virtual bool init();
	void CheckOver();								// <����Ƿ������������
	void DerLead(int val,int cx, int cy);
	void UnDerLead(int val,int cx, int cy);

	void DepLead(int centx, int centy, int range, int val);
	void DepRemoveLead( int centx, int centy, int range, int val );
private:
	Entiles* meSrc;		// <�����ķ�����	| ������������Chara�ڽ���
	CCArray* meTar;		// <����
	Script*	 mspVals;

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
};

#endif
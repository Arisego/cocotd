#ifndef __BATTLE_FIELD_H__
#define __BATTLE_FIELD_H__

#include "cocos2d.h"
#include "utils\Entiles.h"
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

	void SetUp(Entiles* aSrc, CCArray* aTar,Script* sp = NULL);		// <ս�����ýӿ�
	void AMOver();									// <һ�ι�����������ս�����

protected:
	virtual bool init();
	void CheckOver();								// <����Ƿ������������

private:
	Entiles* meSrc;		// <�����ķ�����	| ������������Chara�ڽ���
	CCArray* meTar;		// <����
	Script*	 mspVals;

	static BattleField *mSharedBattleField;
	bool mbIsOver;
	//int miEnemy;
};

#endif
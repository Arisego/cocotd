#ifndef __E_ROUND_H__
#define __E_ROUND_H__

#include "cocos2d.h"
#include "utils\Scriptor.h"
USING_NS_CC;


/*
	< EGroup | ������Ϣ������ - EChess
		< �����ѯ������������ϸ��Ϣ
		< ������ֵ�ĸ���
		< �����˺������Լ���
		< ���������ݣ�ֻΪͳһ���㷽��
*/

class EGroup
{
public:
	EGroup();
	~EGroup();

	static EGroup *sharedEGroup();
	static void purgeSharedEGroup();

	void	 FactDamage(CCObject* aSrc, CCObject* aTar, int adamv, int atype, int mtype = 0);		// <�˺����������ɺ���н�һ����������
	bool	 CheckGrp(CCObject* aTar);																// <����Ƿ�Ϊ��ҿ������� | ʵ����� EChess* | ������ʼ�����ɽű���� | == �Ƿ���Ҫ����ֵ���� == �Ƿ�AI
	void	 ExpHunp(CCObject* aSrc, int aiV);														// <���Ļ�������ľ���
	void	 ExpDead(CCObject* aSrc);																// <��λ����׷�Ӿ��� || ע�������˺���Դ���������
	bool	 IsEnemy(CCObject* aSrc, CCObject* aTar);	// <Src�Ƿ����Tar|ʵ����� EChess*		

private:
	static EGroup* mSharedEGroup;
	virtual bool init();

	CCObject* meSrc;
	CCObject* meTar;
public:
	bool	 ValChange(Script* asp);																	// <�����ֵ
	bool	 AtkPlay(Script* asp, CCObject* aesrc = NULL, CCObject* aetar = NULL);						// <���Ź���|�����|����
	bool	 AtkSkill(Script* asp, CCObject* aesrc = NULL, CCObject* aetar = NULL);						// <���ż���|�����|����

	int		 getRound();																				// <��õ�ǰ�غ���
	int		 getTurn();

	bool	 ManuAlrange(Script* asp, CCObject* aSrc, CCObject* aTar = NULL);												// <ͨ�õļ򻯲�������
	
	int		miDame;	// <�����˺���ֵ | 
	bool	MakDeath(Script* asp);	// <ǿ����������

	CCObject* meLast;		// <���õ�λ���������Լ��Ļ��ƣ�ʹ���������ϣ����Կ�����ʱʹ�á�
};

#endif
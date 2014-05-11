#ifndef __B_BUFFER_H__
#define __B_BUFFER_H__

/*
	< BBuffer | ����buffer����Ч
*/

#define TR_OPEN		0	/* <Buffer����ʱ���� */
#define TR_ROUND	1	/* <�غ��л���ʼʱ���� */
#define ATK_FIREST	2	/* <��һ�ι��� �� ��ͨ���� ʱ������������ */
#define TR_DAMAGE	3	/* <�Ա�ĵ�λ����˺�(Judge) */
#define TR_ATKED	4	/* <������������ǰ */
#define TR_HP		5	/* <HP�����䶯 | ��������ר�� */
#define TR_ATKOVER	6	/* <EchessComp һ�鶯��������� */
#define TR_MISS		7	/* <��������ʱ */
#define TR_COUTDOWN	8	/* <�������㷢��|����BUFFER�ڲ�ʹ�� */
#define TR_BEHURT	9	/* <�������˺��ж� | ���� */
#define TR_BEGIN	10	/* <��ʼһ��ս��ʱ���� ��SetBat�� */

#define TR_GEDANG	11	/* <���ж�ǰ */
#define TR_BEDAMA	12	/* <�˺���ʾǰ |��ʾ ��λ| ǿ������ */
#define TR_SRDAMA	13	/* <�˺��ж�ǰ |Egroup-FactDamage  |���� ��λ| ���� */
//#define TR_BSDAMA	14  /* <�˺��ж�ǰ |Entile-ShowDamage  |���� ��λ */
#define TR_DEATH	15  /* <��λ��ɱ�� |Entile-ShowDamage  |���� ��λ */

#include "cocos2d.h"
#include "BChara.h"
#include <map>
#include "utils\Scriptor.h"
USING_NS_CC;

class BBuffer : public CCObject
{
public:
	Chara* mpChara;
	~BBuffer();

	BBuffer(const char* scp, Chara* apc);

	void Trig(int ai);
private:
	Scriptor* mscp;

	std::map<int, Script*> misStates;
	std::map<string, int> miAttrs;

	std::map<int, Script*> miscTrigger;

	void RegTrigger(int ai);			// <ע�ᴥ����
	std::map<int, CCObject*>	mTrigerCache;	// <����������

	void RefreshAttrs(const char* ast);	// <������ֵ�Ա�

	int miState;
	
public:
	int miCountDown;					// <���ڼ����ۼ�

	int miId;							// <BufferID
	int getState();						// <���״ֵ̬

	CCObject* mcTarget;					// <�����Ŀ��
	
};

#endif
#ifndef __B_SKILL_H__
#define __B_SKILL_H__

/*
*	< �����ļ��ܼ��������
*	< �����ܵ�ʹ��ͳһ����
*/
#include "cocos2d.h"
#include "tinyxml\tinyxml.h"

using namespace std;
USING_NS_CC;

class BSkill : public CCObject
{
public:
	BSkill(map<string,string> &a_ssm);

	int miDamage;		/* <�˺� || ����Ԥ���� */
	int miNext;			/* <��һ���ȼ��ı��||���ܻ��ڶ�ȼ��������õ� */
	int miRange;		/* <???? */
	int miSFlag;		/* <���ܵĽ׶α�� | bit flag */
	int miType;			/* <����/���� */
	int miTarget;

	int miPhase;		/* <�������ܵĿɴ���ʹ�ý׶� */

	string msAddition;	/* <��Χ��Ϣ */
	string msSideEffect;/* <���ܵ�ʹ���������� */
	string msName;
	string msDeffect;	/* <�б�UI�ü��ܽű� */
	string msScript;	/* <�ű� */

	int m_iUseCase;		
	int m_iLink;	
};

class BSkillList
{
public:
	BSkillList();
	~BSkillList();

	void clear();
	void insert(int aia, int aib);
	int size();
	void saveXml(TiXmlElement *LockElement);

	void inform();

	std::map<int,int> m_viSkills;
	BSkill* getSkill(int id);
private:
	vector<map<string,string>> vdata;
	map<int,int> m_miiViDb;				// <����ID - vdata id
	CCDictionary* mcd_Skills;

	static BSkillList* mSharedBSkillList;
public:
	static BSkillList *sharedBSkillList();
	static void purgeSharedBSkillList();
	bool init();
};

#endif
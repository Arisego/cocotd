#ifndef __B_HALO_H__
#define __B_HALO_H__

#include "cocos2d.h"
#include "utils\Scriptor.h"
USING_NS_CC;

class HaloStat;

class BHalo : public CCObject
{
public:
	BHalo(Script* asp, CCObject* aec);
	~BHalo();

	CCObject*	m_ecOwner;						// <�⻷�ĳ�����
private:
	set<CCObject*>	m_caEObs;					// <�ܵ��⻷Ӱ��ĵ�λ
	int			miState;						// <״̬��־ | 0 - �޵�λ��׽ | 1 - ��׽����λ


	bool		init();							// <��ʼ��
	void		RefreshState();					// <����״̬
	void		RefreshOwner();					// <ˢ�³�����״̬

	Script*		mScp;							// <�⻷�ű�
	int			miDist;							// <�⻷�ķ�Χ
	map<CCObject*, HaloStat*> mHalosTates;		// <�⻷״̬����

	int			miNums;							// <��Ӱ��ĵ�λ�� | ����������
	std::map<string, int>	  miAttrs;			// <��ʱ����װ��

public:
	void		checkEC(CCObject* aeTar, CCPoint acpmo, CCPoint acpaf);		// <��鵥λ�Ƿ��ڹ⻷��
	void		removEC(CCObject* aeTar);		// <�Ƴ������λ�Ĺ⻷��Ч | ���Ƴ�����ӵ�ģʽ

protected:
	void		RefreshAttrs(const char* ast);	// <ˢ����ʱֵ
	void		RefreshNums();					// <������State
	
	HaloStat*	mHaloOS;						// <�����ߵ�״̬����

	int			miType;							// <�⻷����ɢ����
	int			miOwType;						// <�⻷�ĳ����ߵ�����

	bool		mbTarProtect, mbOwnProtect;		// <�������
	int			motoNum;						// <����

	void		f_clean();						// <ȫ�������ʵ��
public:
	void		removeAllMov();					// <���ƶ��������
	void		removeClean(CCObject* aeTar);	// <�����������
};


#endif
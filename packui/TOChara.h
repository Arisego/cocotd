#ifndef __TO_CHARA__
#define __TO_CHARA__

#include "cocos2d.h"
#include "packui/CharaS.h"
#include "packui/TOEquips.h"
#include "utils/Controller.h"
#include "packui/TOPopup.h"

USING_NS_CC;

/*
	TOChara
	<TO_���Ľ�ɫ��Ϣ����
	<�������̽ӿ� ijkl zx wsad

*/

class TOChara : public CCLayer,public CharaTab
{
public:
	TOChara(int a_iCharaID,CCObject* target, SEL_MenuHandler selector);
	~TOChara();

	/* <�������� */
	TOPopup* m_Topop;
	/* <װ�������,�������ӿ� */
	void EquipClick(CCObject* pSender);
	/* Change Equip view */
	bool RefreshEquip(int anewi);
	/* Pop up */
	void EquipPop();

protected:
	/* <����ֵ�ֵ� string|laberbm */
	CCDictionary* m_cdBmNum;
	/* <װ����ť */
	vector<TOEquips*> m_vEBtns;	
	/* <���Ժ�װ�� */
	CCSprite* mSp_av;
	/* <���� */
	CCSprite* mSp_sa;			

	/* <ˢ����ʾ */
	void RefreshView(int Page);
	/* <ˢ������ */
	void RefreshData(int Id);


	/* <��ʾ���Ժ�װ��ҳ pageid:1 */
	void ShowAV();
	/* <װ���б� */
	void ShowEquips();
	/* <��ʼ����ֵ��ʾ pageid:1 */
	void InitBmNums();


	/* <��ʾ����ҳ */
	void ShowSa();

	void w_press();
	void s_press();

private:
	//////////////////////////////////////////////////////////////////////////
	//	<ͨ�õĻص��ṹ

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate(CCObject* pSender);
	//////////////////////////////////////////////////////////////////////////
	/* <��ɫ��� */
	int m_iCharaID;
	/* <ҳ��־ 1 Ϊ���� 2 Ϊ���� */
	int m_iPage;
	/* <��ɫ��������CharaS */
	int m_CharaNums;
	int m_iCurrentEuip;

	/* <��ɫ������Դ */
	Chara* g_chara;
	//////////////////////////////////////////////////////////////////////////
	// page id 1
	/* name */
	CCLabelTTF* clt;
	/* stand */
	CCSprite*	mSpStand;
	/* Button area */
	BYLayerDescendant* eq_mb;
	/* Equip List<Db */
	CCDictionary* m_cdEquips;
};


#endif
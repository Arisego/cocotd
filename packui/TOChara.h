#ifndef __TO_CHARA__
#define __TO_CHARA__

#include "cocos2d.h"
#include "packui/CharaS.h"

USING_NS_CC;

/*
	TOChara
	<TO_���Ľ�ɫ��Ϣ����
	<�������̽ӿ� ijkl zx wsad

*/

class TOChara : public CCLayer
{
public:
	TOChara(int a_iCharaID,CCObject* target, SEL_MenuHandler selector);
	~TOChara();

protected:
	CCDictionary* m_cdBmNum;
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
	/* <��ʼ����ֵ��ʾ pageid:1 */
	void InitBmNums();

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

	/* <��ɫ������Դ */
	Chara* g_chara;
	//////////////////////////////////////////////////////////////////////////
	// page id 1
	/* name */
	CCLabelTTF* clt;
	/* stand */
	CCSprite*	mSpStand;
};


#endif
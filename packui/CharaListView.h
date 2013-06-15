#ifndef __CHARA_LIST_VIEW_H__
#define __CHARA_LIST_VIEW_H__

#include "packui/CharaState.h"
#include "packui/CharaS.h"

/*
	CharaListView
	<������ʾ��ɫ�б�
	<Prepare_xxx��ת����������
	<Ŀǰ��ӵ�е�״̬
		1<Mini״̬ --	-1
			<Ϊ���ܺ���Ʒʹ���ṩ������ѡ��,ͨ��activate���з���
		2<�б�״̬ --	1��2
			չʾCharaState���б�
			1�����б�
			2������չ��

*/
class CharaListView : public BYLayerDescendant
{
public:
	virtual bool init();
	int m_iNums, m_iCurs, m_iBegin;
	CCArray* charalist;
	
	/* ��ʶ��ǰCharaListView��״̬
			-1	--	<Mini״̬
			1	--	<���б�״̬
			2	--	<�б��еĵ���CharaStateչ��
	*/
	int m_iCLVState;

	void setbegin(int a_iBe);
	CharaListView();
	~CharaListView();

protected:
	CharaS* g_charas;
	//vector<Container*> lrbtns;
	Container* t_cbLeft;
	Container* t_cbRight;

private:
	/* <����CharaListView��������ֵ��λ�ã���ť */
	void RefreshList();

	/*	<����״̬ת���ĺ��� */
	void CharasCBack(CCObject* pSender);
	/* Left Right <�л���ť���� */
	void lrBack(CCObject* pSender);
	/* LR��ť��ʾ״̬ˢ�� */
	void RefreshLR();
	/* ׼������1��2��״̬�л�<�����Ŷ��� */
	void PrepareSingle();	
	/* 1��2�л���ɣ�����CharaState��<�Ŵ���� */
	void PrepareBack(CCObject* pSender);
	/* <[TO_USE]˳�����ı�ĵ����ûص� */
	void PopupBack(CCObject* pSender);

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;

	CharaState* m_tcsTarget;
	
public:
	void RefreshListValue();
	void showtargettext(string s, int type = 0);
	void activate(CCObject*);	//CallBack
	void setactivator(CCObject* target, SEL_MenuHandler selector);

	/*
		[state:-1]		m_iItemCount<��Ʒʹ�õ�ʣ������
		[state_flag]	m_iMiniType<�Ƿ���С�б�ģʽ�ı�־λ
	*/
	int m_iItemCount,m_iMiniType;			
	CCLabelBMFont* m_ic;

	/* [state:-1]<����������Ʒ�����Ľӿ� */
	void seticc(int i);
};

#endif
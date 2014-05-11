#ifndef __INFO_TAB_H__
#define __INFO_TAB_H__

#include "byeven/BYCocos.h"
#include "packui/Container.h"

using namespace std;

class InfoTab : public BYLayerAncestor
{
protected:

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate(CCObject* pSender);
	int m_iType;		//select,info,...
	
private:
	static InfoTab* m_sharedInfoTab;
	float m_fWidth,m_fHeight;
	BYLayerDescendant *mb;

	bool init();

public:
	~InfoTab();

	bool m_bIsEnabled;

	static InfoTab *sharedInfoTab();
	static void purgeSharedInfoTab();

	virtual void showinfo(string s);
	/* <ת����Ҫ���ڲ��¼� */
	virtual void selectrediret(CCObject* pSender);

	virtual void eatout(CCObject* pSender);
	virtual void showselect(string s, vector<string> sellis, CCObject* target, SEL_MenuHandler selector);
	virtual bool byTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void showsteper(string s, CCObject* target, SEL_MenuHandler selector,int sum = 0);
	virtual void showldb(string s, CCObject* target, SEL_MenuHandler selector, string sql, CCDictionary* a_icd, int type = 0);

	/* [TO]<��ɫ�����Լ������ */
	virtual void showcharasav(int a_iId,CCObject* target, SEL_MenuHandler selector);

	void stepchanged(CCObject *sender, CCControlEvent controlEvent);;
	
	void Add_Button(const char* name,int tag);
	bool handletouch(CCTouch* pt);
	void showbmfl(string s,CCObject* target, SEL_MenuHandler selector,string fls);
	bool m_bIsSimple;			//���tab��һ����tab����ô������Ҫ�����κ�ֵ������ֱ��dismis
	cocos2d::CCLabelBMFont* m_bmlable;
	int m_icount;
};

#endif
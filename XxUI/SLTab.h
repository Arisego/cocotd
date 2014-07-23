#ifndef __SL_TAB_H__
#define __SL_TAB_H__

#include "packui\Tab.h"
#include "json/json.h"

class SLCell : public Container
{
private:
	int miSlState;

	CCLabelBMFont*	mLbTIme;
	CCSprite*		mSpCellBg;
	virtual void update(float fDelta) override;

public:
	SLCell();
	void	initWithJSon(Json::Value);
	void	ChangeState(int ai);
};



/* <���� */


class SLTab : public Tab{
private:
	std::vector<SLCell*> m_vSLs;
	ConfigManager* cm;
	float m_fSHeight,m_fSWidth;
	int m_iCurTab;
	set<int> iSet;
	Container* mTabName;

	BYLayerDescendant* mb;
	
public:
	SLTab();

	CCObject* m_oLockSave;

	void selqueryback(CCObject* pSender);	/* <����ѯ�ʻص� */
	void savetofile();						/* <�����ļ� */
	void buttonback(CCObject* sender);		/* <��ȡ�ĵ��Ļص� */
	void Add_Button(int tag);				/* <��Ӷ�ȡ��ť */

	void ShowTab(int itab);					/* <�趨��ʾ */
	void CleanStates();

	~SLTab();
};
#endif



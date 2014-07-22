#ifndef __SL_TAB_H__
#define __SL_TAB_H__

#include "packui\Tab.h"

class SLCell : public Container
{
private:
	int miSlState;

public:
	SLCell();
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



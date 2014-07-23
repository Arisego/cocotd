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



/* <档案 */


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

	void selqueryback(CCObject* pSender);	/* <覆盖询问回调 */
	void savetofile();						/* <存入文件 */
	void buttonback(CCObject* sender);		/* <读取文档的回调 */
	void Add_Button(int tag);				/* <添加读取按钮 */

	void ShowTab(int itab);					/* <设定显示 */
	void CleanStates();

	~SLTab();
};
#endif



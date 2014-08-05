#ifndef __BM_VLIST_H__
#define __BM_VLIST_H__

/* <��ս��ģʽ��ʾ�����Ա� */

#include "byeven/BYCocos.h"
#include "battle/BChara.h"
#include "packui/TOEquips.h"

class BmVList
	: public BYLayerDescendant
{
protected:
	/* <���Ա�Ļ��� */
	CCSprite* mSp_av;	
	/* <����ֵ�ֵ� string|laberbm */
	CCDictionary* m_cdBmNum;
	/* <װ����ť */
	vector<TOEquips*> m_vEBtns;	
	//////////////////////////////////////////////////////////////////////////
	// page id 1
	/* name */
	CCLabelTTF* clt;
	/* Button area */
	BYLayerDescendant* eq_mb;
	/* Equip List<Db */
	CCDictionary* m_cdEquips;
	bool m_bPopup;
public:
	~BmVList();
	virtual bool init();
	void	setChara(Chara* ac);
	
	Chara* g_chara;	/* <��ɫ������Դ */

protected:
	void	ShowContents();
	void	InitBmNums();
};

#endif
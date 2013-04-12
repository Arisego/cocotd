#ifndef __ITEM_PICKER_H__
#define __ITEM_PICKER_H__

#include "packui/Container.h"
#include "packui/ItemCell.h"
#include "byeven/BYCocos.h"
#include "utils/Controller.h"
//ItemPacker. LeftDown Side of WalkMap for selecting
class ItemPicker : public BYLayerDescendant, public Selector
{
public:
	virtual bool init();
	void show_item();
	~ItemPicker();

private:
	int m_iCItemID;			//Real id. for last;

	void ItemSelect(CCObject*);
	bool refresh_itemlist();

	vector<map<string,string>> vdata;
	Container* m_bcItem;			//Item View.
	//BYLayerDescendant* m_bldContent;
	CCDictionary* m_cdItemList;		//Item List.

	int m_iCurItem, m_iSum;
	vector<ItemCellData*> m_viItemList;		//For View2Data Link.

	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	void activate(CCObject* pSender);
public:
	void setactivator(CCObject* target, SEL_MenuHandler selector);
	bool m_bIsEnabled;
	void onShow();
	void onDismiss();

	virtual void q_press();
	virtual void e_press();
	virtual void c_press();

};


#endif
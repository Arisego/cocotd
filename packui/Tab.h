/*
|	Tab容器籿
|
|
*/
#ifndef __TAB_H__
#define __TAB_H__

#include "packui/Container.h"
#include "packui/MouseController.h"
#include "cocos-ext.h"
#include "packui/ConfigManager.h"
#include "CCEGLView.h"
#include "utils/ALSingle.h"
#include "StateCenter.h"
#include "sublayer/InfoTab.h"
#include "packui/TextView.h"
#include "packui/ListDBView.h"
#include "packui/CharaListView.h"
#include "utils/EffectControler.h"
#include "packui/InterfaceEffect.h"
#include "Macros.h"

USING_NS_CC_EXT;
using namespace std;

#define SL_TAB_FACTOR 0.1875

class Tab : public BYLayerDescendant
{
public:
	virtual void CleanStates() = 0;
	virtual void ShowTab(int itab) = 0;

	Tab(){
		autorelease();
	}

	virtual void onExit(){
		//this->unscheduleUpdate();
	}

	~Tab(){
		CCLOG(">Tab DesC.");
	}
};

/* <配置 */
class ConfigTab : public Tab{
private:
	//std::vector<Container*> vcv;
	float m_flwidth,m_frwidth;
	std::map<int,int> valMo,ValCh;
	ConfigManager* cm;
	Container* tabc;

public:

	void menuCallback(CCObject* sender)
	{
		int itag = ((Container *) sender)->getTag();
		//TODO: 跳转到指定的Tab
	}

	ConfigTab(){
	//	this;
		cm = ConfigManager::sharedConfigManager();


		tabc = new Container();
		tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));
		tabc->setAnchorPoint(ccp(0,0));
		tabc->setPosition(ccp(0,500));

		addChild(tabc,1,0);
		tabc->onSelect();

		ShowTab();
	}

	void CleanStates(){
		removeAllChildren();
		//removeFromParent();
		//CC_SAFE_RELEASE_NULL(tabc);
	}

	void SystemC_Tab(){
		m_flwidth = 130;
		m_frwidth = 250;
		float s_fheight = 450;

		string s;
		int v;

		//cm->GetConfigV(CM_FULLSCREEN,v);
		//s = cm->GetConfigS("full_screen");

		//CCLabelBMFont* labelAtlas = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		//mControlSwith *switchControl = new mControlSwith();
		//switchControl->initWithMaskSprite(
		//	CCSprite::create("extensions/switch-mask.png"),
		//	CCSprite::create("extensions/switch-on.png"),
		//	CCSprite::create("extensions/switch-off.png"),
		//	CCSprite::create("extensions/switch-thumb.png"),
		//	CCLabelTTF::create("On", "Arial-BoldMT", 16),
		//	CCLabelTTF::create("Off", "Arial-BoldMT", 16)
		//	);
		////switchControl->setAnchorPoint(ccp(0,0));
		//switchControl->setPosition(ccp(m_frwidth, s_fheight));

		////labelAtlas->setAnchorPoint(ccp(0,0));
		//labelAtlas->setPosition(ccp(m_flwidth,s_fheight));
		//switchControl->setOn(v);
		//valMo[CM_FULLSCREEN] = v;
		//switchControl->autorelease();
		//addChild(labelAtlas);
		//addChild(switchControl);
		//switchControl->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigTab::valueChanged), CCControlEventValueChanged);

		//////////////////////////////////////////////////////////////////////////
		s_fheight -= 50;

		s = cm->GetConfigS("audio_set");
		CCLabelBMFont* label_audio_set = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		label_audio_set->setPosition(ccp(m_flwidth,s_fheight));
		addChild(label_audio_set);

		s_fheight -= 50;
		cm->GetConfigV(CM_BGMVOLEM,v);
		mPotentionMeter *potentiometer = mPotentionMeter::create("extensions/potentiometerTrack.png"
			,"extensions/potentiometerProgress.png"
			,"extensions/potentiometerButton.png");
		potentiometer->setPosition(ccp (m_frwidth, s_fheight));
		potentiometer->setTag(CM_BGMVOLEM);
		potentiometer->setValue(v/100.0);
		addChild(potentiometer);
		potentiometer->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigTab::metervalueChanged), CCControlEventValueChanged);

		cm->GetConfigV(CM_EFFECTVOLEM,v);
		mPotentionMeter* pm2 = mPotentionMeter::create("extensions/potentiometerTrack.png"
			,"extensions/potentiometerProgress.png"
			,"extensions/potentiometerButton.png");
		pm2->setPosition(ccp(m_frwidth + 100,s_fheight));
		pm2->setTag(CM_EFFECTVOLEM);
		pm2->setValue(v/100.0);
		addChild(pm2);
		pm2->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigTab::metervalueChanged), CCControlEventValueChanged);

		cm->GetConfigV(CM_VIDIEOVOLEM,v);
		mPotentionMeter* pm3 = mPotentionMeter::create("extensions/potentiometerTrack.png"
			,"extensions/potentiometerProgress.png"
			,"extensions/potentiometerButton.png");
		pm3->setPosition(ccp(m_frwidth + 200,s_fheight));
		pm3->setTag(CM_VIDIEOVOLEM);
		pm3->setValue(v/100.0);
		addChild(pm3);
		pm3->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigTab::metervalueChanged), CCControlEventValueChanged);


		s_fheight -= potentiometer->getContentSize().height - 50;
		s = cm->GetConfigS("audio_bgm");
		CCLabelBMFont* label_audio_bgm = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		label_audio_bgm->setPosition(ccp(m_frwidth,s_fheight));
		label_audio_bgm->setColor(ccBLUE);
		addChild(label_audio_bgm);

		s = cm->GetConfigS("audio_eff");
		CCLabelBMFont* label_audio_eff = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		label_audio_eff->setPosition(ccp(m_frwidth + 100,s_fheight));
		label_audio_eff->setColor(ccBLUE);
		addChild(label_audio_eff);

		s = cm->GetConfigS("audio_vid");
		CCLabelBMFont* label_audio_vid = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		label_audio_vid->setPosition(ccp(m_frwidth + 200,s_fheight));
		label_audio_vid->setColor(ccBLUE);
		addChild(label_audio_vid);

		//////////////////////////////////////////////////////////////////////////
		s_fheight -= 40;
		// Add the slider
		s = cm->GetConfigS("text_speed");
		CCLabelBMFont* label_text_speed = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		label_text_speed->setPosition(ccp(m_flwidth,s_fheight));
		addChild(label_text_speed);

		cm->GetConfigV(CM_TEXTSPEED,v);
		mControlSlider *slider = mControlSlider::create("extensions/sliderTrack.png","extensions/sliderProgress.png" ,"extensions/sliderThumb.png");
		slider->setMinimumValue(2.0f); // Sets the min value of range
		slider->setMaximumValue(100.0f); // Sets the max value of range
		slider->setValue(v);
		slider->setPosition(ccp(m_frwidth  + 100, s_fheight));
		slider->setTag(CM_TEXTSPEED);
		addChild(slider);
		slider->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigTab::sliderChanged), CCControlEventValueChanged);


		s_fheight -= 20;
		// Add the slider
		s = cm->GetConfigS("text_auto");
		CCLabelBMFont* label_text_auto = CCLabelBMFont::create(s.c_str(), FNT_CHN);
		label_text_auto->setPosition(ccp(m_flwidth,s_fheight));
		addChild(label_text_auto);

		cm->GetConfigV(CM_AUTOTIME,v);
		mControlSlider *slider1 = mControlSlider::create("extensions/sliderTrack.png","extensions/sliderProgress.png" ,"extensions/sliderThumb.png");
		slider1->setMinimumValue(10.0f); // Sets the min value of range
		slider1->setMaximumValue(200.0f); // Sets the max value of range
		slider1->setValue(v);
		slider1->setPosition(ccp(m_frwidth  + 100, s_fheight));
		slider1->setTag(CM_AUTOTIME);
		addChild(slider1);
		slider1->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigTab::sliderChanged), CCControlEventValueChanged);
	}

	void ShowTab(int itab = 0){
		switch(itab){
		case 0:
			SystemC_Tab();
			break;
		}
	}

	void metervalueChanged(CCObject* sender, CCControlEvent controlEvent)
	{
		CCControlPotentiometer* pControl = (CCControlPotentiometer*)sender;
		int tag = pControl->getTag();
		cm->SetConfigV(tag,pControl->getValue()*100);
		switch(tag){
		case(CM_BGMVOLEM):
			{
				ALSingle::sharedALSingle()->RefreshBgmV(pControl->getValue());
				break;
			}
		case(CM_EFFECTVOLEM):
			{
				ALSingle::sharedALSingle()->RefreshEffect(pControl->getValue());
				break;
			}
		case(CM_VIDIEOVOLEM):
			{
				ALSingle::sharedALSingle()->RefreshPlayer(pControl->getValue());
				break;
			}
		default:
			break;
		}
	}

	void valueChanged(CCObject* sender, CCControlEvent controlEvent)
	{
		this;
		CCControlSwitch* pSwitch = (CCControlSwitch*)sender;
//		CCEGLView::sharedOpenGLView()->setFullScreen(pSwitch->isOn());
		cm->SetConfigV(CM_FULLSCREEN,pSwitch->isOn());
	}

	void sliderChanged(CCObject* sender, CCControlEvent controlEvent){
		CCControlSlider* pSlider = (CCControlSlider*)sender;
		cm->SetConfigV(pSlider->getTag(),pSlider->getValue());
		CCLOG(">Slider Changed");
	}
};

/* <档案 */
class SLTab : public Tab{
private:
	std::vector<Container*> m_vTabs;
	std::vector<Container*> m_vSLs;
	ConfigManager* cm;
	float m_fSHeight,m_fSWidth;
	int m_iCurTab;
	set<int> iSet;
	
public:
	bool m_bLockSave;
	void menuCallback(CCObject* sender)
	{
		int itag = ((Container *) sender)->getTag();
		ShowTab(itag);
	}

	SLTab(bool lock){
		iSet.clear();
		m_vTabs.clear();
		m_iCurTab = 0;
		m_oLockSave = NULL;
		m_bLockSave = lock;
		m_vSLs.assign(8,NULL);

		cm = ConfigManager::sharedConfigManager();
		Container* tabc = new Container();
		tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("save").c_str(),100,30,this,menu_selector(SLTab::menuCallback));
		tabc->setAnchorPoint(ccp(0,0));
		tabc->setPosition(ccp(0,500));
		addChild(tabc,1,0);
		if(m_bLockSave) tabc->onDisable();
		m_vTabs.push_back(tabc);

		tabc = new Container();
		tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("load").c_str(),100,30,this,menu_selector(SLTab::menuCallback));
		tabc->setAnchorPoint(ccp(0,0));
		tabc->setPosition(ccp(100,500));
		addChild(tabc,1,1);
		m_vTabs.push_back(tabc);


		m_fSWidth = 10;
		m_fSHeight = 250;
		for(int i = 0; i < 8; ++i){
			Add_Button(i);
		}

	}

	CCObject* m_oLockSave;

	void selqueryback(CCObject* pSender){
		int itag = ((Container *) pSender)->getTag();
		CCLOG(">SQB:%d",itag);
		if(itag == 1)
			savetofile();
	}

	void savetofile(){
		if(!m_oLockSave){
			CCLOG(">SQB: Invalid Save Object.");
			return;
		}
		int t_t = ((Container *) m_oLockSave)->getTag();

		StateCenter::sharedStateCenter()->f_save_file(CCString::createWithFormat("save_%d",t_t)->getCString());		
		((Container *) m_oLockSave)->removeFromParent();
		m_oLockSave = NULL;
		Add_Button(t_t);
	}

	void buttonback(CCObject* sender){
		int itag = ((Container *) sender)->getTag();
		switch(m_iCurTab){
		case 0:
			{
				CC_SAFE_RELEASE_NULL(m_oLockSave);
				m_oLockSave = sender;
				
				if(iSet.count(itag) == 0){
					savetofile();
				}else{
					vector<string> sl;
					sl.push_back("yes");
					sl.push_back("no");
				
					InfoTab::sharedInfoTab()->showselect("pre_rewrite", sl, this, menu_selector(SLTab::selqueryback));
				}
				break;
			}
		case 1:
			{
				StateCenter::sharedStateCenter()->f_load_file(CCString::createWithFormat("save_%d",itag)->getCString());
				break;
			}
		}
		CCLOG("sl:%d",itag);
		Refresh_Button();
	}

	void Refresh_Button(){
		std::vector<Container*>::iterator it;
		for(it = m_vSLs.begin(); it != m_vSLs.end(); ++it){
			(*it)->onNormal();
		}
	}

	void Add_Button(int tag){
		Container* tabc = new Container();
		//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));

		tabc->initString("save",150,200,"Images/button",this,menu_selector(SLTab::buttonback));
		tabc->setTag(tag);
		tabc->setAnchorPoint(ccp(0,0));
		
		CCString* fullPath = CCString::createWithFormat("save/save_%d.jpg",tag);
		CCTextureCache::sharedTextureCache()->removeTextureForKey(fullPath->getCString());
		CCSprite* css = CCSprite::create(fullPath->getCString());
		if(css){
			iSet.insert(tag);
			css->setAnchorPoint(ccp(0,0));
			css->setScale(SL_TAB_FACTOR);
			tabc->addChild(css);
		}
		
		float m_fHeight = (tag>3 ? m_fSHeight - 210 : m_fSHeight);
		tabc->setPosition(ccp(m_fSWidth + (tag%4)*160,m_fHeight));
		addChild(tabc);
		m_vSLs[tag] = tabc;
	}

	/*
	|	itab >= 0 调用对应的itab
	|	itab = -2 调用itab 0
	*/
	void ShowTab(int itab = -2){
		static int m_iTab = -1;
		if(itab == m_iTab) return;
		Refresh_Button();
		if(itab > -1) m_iTab = itab;
		else m_iTab = 0;

		if(m_bLockSave){
			CCLOG("Save is now locked.");
			m_iTab = 1;
		}

		vector<Container*>::iterator it = m_vTabs.begin();
		for(; it != m_vTabs.end(); ++it){
			if((*it)->getTag() != m_iTab) (*it)->onNormal();
			else (*it)->onSelect();
		}

		m_iCurTab = m_iTab;
		switch(m_iTab){
		case 0:
			CCLOG("Save Tab Showing.");
			break;
		case 1:
			CCLOG("Load Tab Showing.");
			break;
		}

	}

	void CleanStates(){
		m_vTabs.clear();
		removeAllChildren();
	}

	~SLTab(){
		CCLOG(">sltab dec");
	}
};

class INFTab : public Tab
{
private:
	std::vector<Container*> m_vTabs;
	std::vector<Container*> m_vIFs;
	ConfigManager* cm;
	float m_fSHeight,m_fSWidth;
	int m_iCurTab;
	set<int> iSet;

	TextView *tv;

	vector<map<string,string>> vdata;
	int m_number;
	int m_iLastTag;

public:
	void buttonback(CCObject* sender){
		int itag = ((Container *) sender)->getTag();
		if(!itag) {
			((Container *) sender)->onNormal();
			return;
		}
		showinfo(itag);
	}

	void showinfo(int itag){
		if(m_iLastTag == itag) return;
		m_iLastTag = itag;

		for(vector<Container*>::iterator it = m_vIFs.begin(); it != m_vIFs.end(); ++it){
			if((*it)->getTag() == itag) (*it)->onSelect();
			else (*it)->onNormal();
		}
		CCLOG(">Prepare for tag:%d",itag);
		if(tv) tv->removeFromParent();
		CC_SAFE_RELEASE_NULL(tv);
		tv = new TextView(659,400,itag);
		addChild(tv);

	}

	void menuCallback(CCObject* sender)
	{
		int itag = ((Container *) sender)->getTag();
		//TODO: 跳转到指定的Tab
	}

	INFTab(){
		iSet.clear();
		m_vTabs.clear();
		m_iCurTab = 0;
		m_iLastTag = -1;
		
		tv = NULL;

		cm = ConfigManager::sharedConfigManager();
		//////////////////////////////////////////////////////////////////////////
		Container* tabc = new Container();
		tabc->initwithPsz("Images/tab_head.png","Test",100,30,this,menu_selector(SLTab::menuCallback));
		tabc->setAnchorPoint(ccp(0,0));
		tabc->setPosition(ccp(0,500));
		addChild(tabc,1,0);
		m_vTabs.push_back(tabc);
	}

	~INFTab(){
		CC_SAFE_RELEASE_NULL(tv);
	}

	void Add_Button(string name, int tag, int a_infoid){
		Container* tabc = new Container();

		tabc->initString(name.c_str(),150,30,"Images/button",this,menu_selector(INFTab::buttonback));
		tabc->setTag(a_infoid);
		tabc->setAnchorPoint(ccp(0,0));
		if(!a_infoid) tabc->onDisable();

		float m_fHeight = (tag>3 ? m_fSHeight - 35 : m_fSHeight);
		tabc->setPosition(ccp(m_fSWidth + (tag%4)*160,m_fHeight));
		addChild(tabc);
		m_vIFs.push_back(tabc);
	}

	void Information_Tab(){
		CCLOG(">IT.tag not used yet:%d",m_iCurTab);

		DBUtil::initDB("save.db"); 
		string sqlstr= CCString::createWithFormat("select infoid,name,lockst from infor_table where groupid = %d AND lineid = 0 LIMIT 8 OFFSET 0",m_iCurTab)->getCString();
		//使用lineid =0 来获得 情报本身的锁状态。//DCHANGE<<<
		vdata = DBUtil::getDataInfo(sqlstr,NULL);
		m_number = vdata.size();
		m_vIFs.clear();
		m_fSWidth = 2;
		m_fSHeight = 465;

		for(int i = 0;i<m_number;i++){
			map<string,string> t_ssm = (map<string,string>) vdata.at(i);
			string t_sname = t_ssm.at("name");
			int t_itag = stoi(t_ssm.at("infoid"));
			string t_slock = t_ssm.at("lockst");
			if(t_slock.size() == 1 || StateCenter::sharedStateCenter()->g_lock_query(t_slock) > 0){
				Add_Button(t_sname, i, t_itag);
			}else{
				Add_Button("[????]", i, 0);
			}
		}

		DBUtil::closeDB();

		showinfo(1);


	}

	/*
	|	itab >= 0 调用对应的itab
	|	itab = -2 调用默认itab
	*/
	void ShowTab(int itab = -2){
		static int m_iTab = -1;
		if(itab == m_iTab) return;
		
		if(itab > -1) m_iTab = itab;
		else m_iTab = 0;	//default

		vector<Container*>::iterator it = m_vTabs.begin();
		for(; it != m_vTabs.end(); ++it){
			if((*it)->getTag() != m_iTab) (*it)->onNormal();
			else (*it)->onSelect();
		}

		m_iCurTab = m_iTab;
		//switch(m_iTab){
		//case 0:
		//	
		//	CCLOG("Save Tab Showing.");
		//	break;
		//case 1:
		//	CCLOG("Not used yet.");
		//	break;
		//}
		Information_Tab();
	}

	void CleanStates(){
		removeAllChildren();
	}
};

/* <物品 */
class PACTab : public Tab, public InterfaceEffect		//Package Tab ||
{
private:
	int m_iTab;
	ListDBView<ItemCell>* ldb;
	CCLabelTTF* m_clt;
	vector<Container*> m_vBtns;

	int m_iItem;
	int m_iIndex;

	ItemCellData* m_sIcd;		//member select item cell data.

public:
	void itemback(CCObject* sender){
		m_iIndex = ((Container *) sender)->getTag();
		m_sIcd = (ItemCellData*) StateCenter::sharedStateCenter()->f_get_itemlist(m_iTab)->objectForKey(m_iIndex);
		m_iItem = m_sIcd->type_id;
		m_clt->setString(ldb->getdiscrip(m_iItem).c_str());
		RefreshButton();
		CCLOG(">itemselect");
	}

	void menuCallback(CCObject* sender)
	{
		int itag = ((Container *) sender)->getTag();
		
		//TODO: 跳转到指定的Tab
	}

	PACTab(){
		m_iTab = 1;	//for test
		ldb = NULL;
		
		RefreshData();
	}

	BYLayerDescendant* mb;
	void RefreshData(){
		mb = new BYLayerDescendant();
		mb->autorelease();
		mb->setAnchorPoint(ccp(0,0));
		mb->setPosition(ccp(1,1));

		CCScale9Sprite* nback = CCScale9Sprite::create("Images/button.png"); 
		nback->setAnchorPoint(ccp(0,0));
		nback->setContentSize(CCSize(450,450));
		nback->setOpacity(110);
		mb->addChild(nback,-1);
		addChild(mb);

		m_clt = CCLabelTTF::create();
		m_clt->setAnchorPoint(ccp(0,1));
		m_clt->setFontName( FNT_UI_LABEL);
		m_clt->setDimensions(CCSizeMake(250,0));
		m_clt->setFontSize(20);
		m_clt->setPosition(ccp(450,450));

		addChild(m_clt);

		reinitldb();

		Container* tabc = new Container();
		//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));

		tabc->initString(ConfigManager::sharedConfigManager()->GetConfigS("use").c_str(),124,22,"Images/button",this,menu_selector(PACTab::buttonback));
		tabc->setTag(1);
		tabc->setAnchorPoint(ccp(0,0));
		tabc->setPosition(ccp(450,1));
		mb->addChild(tabc);
		m_vBtns.push_back(tabc);

		tabc = new Container();
		//tabc->initwithPsz("Images/tab_head.png",cm->GetConfigS("system").c_str(),100,30,this,menu_selector(ConfigTab::menuCallback));

		tabc->initString(ConfigManager::sharedConfigManager()->GetConfigS("discard").c_str(),124,22,"Images/button",this,menu_selector(PACTab::buttonback));
		tabc->setTag(2);
		tabc->setAnchorPoint(ccp(0,0));
		tabc->setPosition(ccp(575,1));
		mb->addChild(tabc);
		m_vBtns.push_back(tabc);
		RefreshButton();
	
	}

	void discardback(CCObject* pSender){
		Container* t_ct = (Container*) pSender;
		int di = t_ct->getTag();

		m_sIcd->sum = m_sIcd->sum - di;
		
		if(m_sIcd->sum == 0){
			StateCenter::sharedStateCenter()->f_get_itemlist(m_iTab)->removeObjectForKey(m_iIndex);
			m_iItem = 0;
			reinitldb();
		}else{
			ldb->RefreshSingleItem(m_iIndex,m_sIcd->sum);
		}

	}

	void reinitldb(){
		//
		m_clt->setString("Used OUt.");
		if(ldb){
			ldb->removeFromParent();
			CC_SAFE_RELEASE_NULL(ldb);
		}

		CCDictionary* m_cid =  StateCenter::sharedStateCenter()->f_get_itemlist(m_iTab);
		string t_sMask;
		CCDictElement* pElement = NULL;
		if(!m_cid) {			//TODO:物品列表为空时添加相应的显示
			CC_SAFE_RELEASE_NULL(m_cid);
			return;	
		}				
		
		CCDICT_FOREACH(m_cid,pElement)
		{
			t_sMask +=  CCString::createWithFormat("%d,",((ItemCellData*) pElement->getObject())->type_id)->getCString();
		}
		if(t_sMask.length()<1) return;
		t_sMask.erase(t_sMask.length()-1);

		CCString* t_csSql = CCString::createWithFormat("select * from item_list where groupid = %d and itemid IN (%s)",m_iTab,t_sMask.c_str());
		ldb = new ListDBView<ItemCell>((float)444,(float)444, t_csSql->getCString(),m_cid,(CCObject*) this,menu_selector(PACTab::itemback));
		if(ldb->init()){
			
			ldb->setAnchorPoint(ccp(0,1));
			ldb->setPosition(ccp(5,0));
			ldb->setContentSize(CCSizeMake(444,444));
			mb->addChild(ldb);

			if(m_iItem >0) {
				ldb->SetSelect(m_iIndex);
				m_clt->setString(ldb->getdiscrip(m_iItem).c_str());
			}
		}else{
			CC_SAFE_RELEASE_NULL(ldb);
		}


		RefreshButton();
	}

	/* <功能按钮 */
	void buttonback(CCObject* pSender){
		int itag = ((Container *) pSender)->getTag();
		switch(itag){
		case(1):
			{
				EffectControler::sharedEffectControler()->md_use_item(this,m_iItem);
				break;
			}
		case(2):
			{
				InfoTab::sharedInfoTab()->showsteper("pre_discard",this,menu_selector(PACTab::discardback),m_sIcd->sum - m_sIcd->lock);		//con need,lock should be for equiped items or some other items that can not be droped
				break;
			}
		}
		RefreshButton();
	}
	//////////////////////////////////////////////////////////////////////////
	// InterfaceView

	virtual void get_target(){
		int t_iTarget = EffectControler::sharedEffectControler()->m_iTarget;		//TODO: chech if it is 0
		if(t_iTarget == -1){					//Target == -1 . there is no need for target. use it here is just for notifaction to view.
			m_sIcd->sum -= 1;
			EffectControler::sharedEffectControler()->md_act_item(NULL);
		}else{
			mb->m_bIsEnabled = false;													//disable mb event.

			CC_SAFE_RELEASE_NULL(m_caTList);
			m_caTList	=	new CCArray();												//DONE:it's cleared by ec every effect.

			removeChildByTag(0x3333);

			CharaListView* clw = new CharaListView();
			
			clw->m_iCLVState = -1;
			clw->m_iMiniType = 1;
			clw->m_iItemCount = m_sIcd->sum;

			clw->init();

			clw->setbegin(0);
			clw->setAnchorPoint(CCPointZero);
			clw->setPosition(ccp(51,141));

			clw->setactivator(this,menu_selector(PACTab::targetBack));

			clw->setTag(0x3333);
			addChild(clw,3);
			//clw->release();
		}


	}

	void targetBack( CCObject* pSender)
	{
		//IN:TODO:if target == 0 , then it means affecting all the chara, we just get that the sender is back.

		CharaState* t_cs = (CharaState*) pSender;
		if(t_cs->getTag() < 0)
		{
			EffectControler::sharedEffectControler()->f_effect_over();
			EffectControler::sharedEffectControler()->f_clear();
			//removeChildByTag(0x3333);
			((CharaListView*) getChildByTag(0x3333))->setVisible(false);
			((CharaListView*) getChildByTag(0x3333))->autorelease();
			mb->m_bIsEnabled = true;
		}else{
			m_caTList->addObject(t_cs->getcontent());
			int ti = ((CharaListView*) getChildByTag(0x3333))->m_iItemCount;

			m_sIcd->sum--;
			ldb->RefreshSingleItem(m_iIndex,m_sIcd->sum);
			CCLOG("[ECICS]>Target back, add to catlist&callc.");

			EffectControler::sharedEffectControler()->md_act_item(m_caTList);

			
			if(m_sIcd->sum < 1) {							//
				if(m_sIcd){
					StateCenter::sharedStateCenter()->f_get_itemlist(m_iTab)->removeObjectForKey(m_iIndex);
					m_iItem = 0;
					reinitldb();
					m_sIcd = NULL;
				}
				
			}
		}

	}

	virtual void refresh_view(){
		if(m_sIcd->sum == 0){
			StateCenter::sharedStateCenter()->f_get_itemlist(m_iTab)->removeObjectForKey(m_iIndex);
			m_iItem = 0;
		}
		reinitldb();
	}

	void effect_over()
	{
		CC_SAFE_RELEASE_NULL(m_caTList);
		CCLOG("[ECICS]>Effect over&clear states.");
		m_caTList	=	new CCArray();			// only release this list when clv_mini is gone.
	}

	void show_text( string s )
	{
		((CharaListView*) getChildByTag(0x3333))->showtargettext(s);
		((CharaListView*) getChildByTag(0x3333))->RefreshListValue();
		//reinitldb();
		EffectControler::sharedEffectControler()->f_sp_gonext();
	}

	//////////////////////////////////////////////////////////////////////////

	void RefreshButton(){			//再进一步的判定中确定物品是否能够使用时调整m_Btns[1]的显示状态
		bool t_bVisible = (m_iItem > 0);
		for(vector<Container*>::iterator it = m_vBtns.begin(); it != m_vBtns.end(); ++it){
			(*it)->onNormal();
			(*it)->setVisible(t_bVisible);
		}
		m_clt->setVisible(t_bVisible);

	}

	void ShowTab(int itab = -2){

	}

	~PACTab(){
		CC_SAFE_RELEASE_NULL(ldb);
	}

	void CleanStates(){
		CCNode* tcn = getChildByTag(0x3333);
		if(tcn){
			tcn->autorelease();
		}


		removeAllChildren();
	}
};

class STATab : public Tab{
public:
	void menuCallback(CCObject* sender)
	{
		int itag = ((Container *) sender)->getTag();
		//TODO: 跳转到指定的Tab
	}

	STATab(){
		removeAllChildren();

		CharaListView* t_clv = new CharaListView();
		
		t_clv->m_iCLVState = 1;
		t_clv->init();
		t_clv->setbegin(0);
		t_clv->setAnchorPoint(CCPointZero);
		t_clv->setPosition(ccp(51,1));

		addChild(t_clv);
		t_clv->autorelease();
	}

	void ShowTab(int itab = -2){

	}

	void CleanStates(){
		removeAllChildren();
	}
};

#endif
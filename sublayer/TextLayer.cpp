#include "sublayer/TextLayer.h"

#include "GameManager.h"
#include "StateCenter.h"
#include "SoundManager.h"

#include "utils/MouseMenu.h"
#include "sprite_nodes/CCSprite.h"
#include <string>
#include <string.h>
#include "actions/CCAction.h"
#include <map>

USING_NS_CC;

#define TEXTLAYER_PRIO_TOUCH 2
#define AUTO_ACTION_TAG 1001
#define SKIP_TIME 0.1
#define FONT_WIDTH 26			// [AT]: If you change this value remember to change the size of "Images/block.png" to FW + 1.

#define UI_ZORDER 100

using namespace std;

static const char* tBtnString[] = {"AUTO","SKIP","SYS","SAVE","LOAD","LOG","HIDE"};

CCDictionary* ToTextLayer::lockstate = NULL;

ToTextLayer::~ToTextLayer(){
	CCLOG(">TextLayer desconstruct.");
	CC_SAFE_RELEASE_NULL(lockstate);
}

void ToTextLayer::Close(){
	Pause();	
	//FlushText("",true);
	m_Label->setString("");
	setVisible(false);
}

void ToTextLayer::Pause(){
	setTouchEnabled(false);
	this->unschedule( schedule_selector(ToTextLayer::StreamText));
	unscheduleUpdate();
}

void ToTextLayer::Resume(){
	GetTimePara();
	setTouchEnabled(true);
	setVisible(true);
	this->schedule( schedule_selector(ToTextLayer::StreamText), m_fTText);
	if(m_bIsSkip) scheduleUpdate();
}

ToTextLayer::ToTextLayer() {
	if (! CCLayer::init() )
	{
		return;
	}

	do{

		m_bIsNoFade = true;
		e_layerstate = 0;
		sel_menu = NULL;
		GetTimePara();
		this->setTouchEnabled(true);
		m_bIsAuto = false;
		m_bIsSkip = false;
//		this->scheduleUpdate();
	}while(0);

	return;
}

void ToTextLayer::GetTimePara(){
	m_fTAuto = ConfigManager::sharedConfigManager()->GetConfigF(CM_AUTOTIME,100);
	m_fTText = ConfigManager::sharedConfigManager()->GetConfigF(CM_TEXTSPEED,1000);
}

void ToTextLayer::ELoad(){
	Script* t_sc = StateCenter::sharedStateCenter()->m_scTags;
	for(int i = 0; i<t_sc->m_snum; ++i){
		Script* t = (Script*) t_sc->scriptnodes->objectAtIndex(i);
		DerLoadImg(t);
	}
}

void ToTextLayer::onEnter()
{
	lockstate	=	NULL;


	TagMap.clear();
	PathMap.clear();
	s = CCDirector::sharedDirector()->getWinSize();
	m_bHasFinal = false;

	m_height = 130;
	m_avatarwidth = 145;
	m_textwidth = s.width - m_avatarwidth - 50;
	m_sBox = CCSprite::create("Images/ui_avg_box.png");
	m_sBox->setAnchorPoint(CCPoint(0.5,0));
	m_sBox->setPosition(ccp(s.width/2,0));
	m_sBox->ignoreAnchorPointForPosition(false);
	addChild(m_sBox, UI_ZORDER);

	m_Label = CCLabelTTF::create("", FNT_AVG_TTF, FONT_WIDTH, CCSize(m_textwidth, m_height), kCCTextAlignmentLeft);
	mSingeWidth = FONT_WIDTH;
	m_Label->setPosition(CCPointZero);
	m_Label->setAnchorPoint(CCPointZero);

	cns_blocks = CCSpriteBatchNode::create("Images/block.png");

	//m_Label->setColor(ccc3(0,0,0));
	cpn_textcn = CCClippingNode::create();
	cpn_textcn->setContentSize(CCSize(m_textwidth, m_height));
	cpn_textcn->setPosition( ccp((s.width + m_avatarwidth -50)/ 2, m_height) );
	cpn_textcn->setAnchorPoint(CCPoint(0.5,1));
	cpn_textcn->addChild(m_Label);
	cpn_textcn->setStencil(cns_blocks);
	//cpn_textcn->setInverted(true);
	
	m_sBox->addChild(cpn_textcn);

	m_Name = CCLabelBMFont::create("",FNT_CHN,300);
	m_Name->setAnchorPoint(CCPointZero);
	m_Name->setPosition(ccp(60,130));
	//m_Name->setColor(ccWHITE);
	addChild(m_Name,UI_ZORDER);

	//Prepare for menu view.
	mvBtns.clear();
	float t_left = 535;
	float t_bottom = 10;
	
	// auto skip save load log hid
	TlBtn* tb_auto;
	for(int i = 0;i<7;++i){
		tb_auto = new TlBtn();
		tb_auto->setAnchorPoint(CCPointZero);
		tb_auto->setPosition(ccp(t_left,t_bottom));
		tb_auto->setTag(i);
		tb_auto->setstring(tBtnString[i]);
		tb_auto->setactivator(this,menu_selector(ToTextLayer::tlbback));
		mvBtns.push_back(tb_auto);
		addChild(tb_auto,UI_ZORDER);
		t_left += 45;
	}

	//CCMenuItemFont *item4 = CCMenuItemFont::create("Fade", this, menu_selector(TextLayer::FadeText) );
	//item4->setFontSizeObj(20);
	//item4->setFontName("Marker Felt");

	//CCMenuItemFont *item5 = CCMenuItemFont::create("Auto", this, menu_selector(TextLayer::StartAuto) );
	//item5->setFontSizeObj(20);
	//item5->setFontName("Marker Felt");

	//CCMenuItemFont *item6 = CCMenuItemFont::create("Skip", this, menu_selector(TextLayer::StartSkip) );
	//item6->setFontSizeObj(20);
	//item6->setFontName("Marker Felt");

	//menu = MouseMenu::menuWithItems(item4, item5, item6, NULL);;
	//menu->alignItemsHorizontallyWithPadding(2);

	//menu->setPosition(ccp((s.width + m_avatarwidth )/ 2, m_height + FONT_WIDTH));
	//menu->setAnchorPoint(CCPoint(0.5,0));
	//
	//addChild(menu,1);
	BYLayerModal::onEnter();
	
	
}

//static const char* tBtnString[] = {"AUTO","SKIP","SYS","SAVE","LOAD","LOG","HIDE"};
void ToTextLayer::tlbback( CCObject* sender )
{
	TlBtn* ttlbtn = (TlBtn*) sender;
	int tag = ttlbtn->getTag();
	m_bTouchProt = false;
	switch (tag)
	{
	case 0:
		{
			StartAuto(sender);
			if(m_bIsAuto) ttlbtn->onSelect();
			else ttlbtn->onNormal();
			break;
		}
	case 1:
		{
			StartSkip(sender);
			if(m_bIsSkip) ttlbtn->onSelect();
			else ttlbtn->onNormal();
			break;
		}
	case 2:
		{
			GameManager::sharedGameManager()->goConfig();
			ttlbtn->onNormal();
			break;
		}
	case 3:
		{
			 GameManager::sharedGameManager()->preConfig(0x1f,0x08,0);
			 ttlbtn->onNormal();
			 break;
		}
	case 4:
		{
			GameManager::sharedGameManager()->preConfig(0x1f,0x08,1);
			ttlbtn->onNormal();
			break;
		}
	case 5:
		{
			SoundManager::sharedSoundManager()->PlayHoverSFX();
			ttlbtn->onNormal();
			break;
		}
	case 6:
		{
			FadeText(sender);
			ttlbtn->onNormal();			
			break;
		}
	default:
		CCLOG(">[GS]Tag error, tlb mis.");
		break;
	}
}


void ToTextLayer::menucallback(CCObject* sender)
{
	m_bTouchProt = false;
	removeChildByTag(MENUTAG);
	
	int jump = ((CCMenuItemFont *) sender)->getTag();
	GameManager::sharedLogicCenter()->e_jump(jump);
	Script* jp = (Script*) lockstate->objectForKey(jump);
	sel_menu = NULL;
	if(jp) StateCenter::sharedStateCenter()->g_lock_change(jp);
	if(!m_bIsSkip) e_layerstate = 0;
	else e_layerstate = 4;
	CC_SAFE_RELEASE_NULL(lockstate);
	
}

bool ToTextLayer::DerLoadImg(Script* ts){	//meta
	const char* filename = ts->getstring("content");
	// TO Change: 
	//	x <标志是否为切换.
	//  tag <4001左边 4002右边

	int x,y;
	int tag		= ts->getint("tag");
	int flag	= ts->getint("flag");

	static int last = 0;

	switch (flag)
	{
	case -1: //Grey it
		{
			CCSprite* tcs =(CCSprite*) getChildByTag(tag);
			if(tcs) {
				tcs->runAction(CCTintBy::create(0,-130,-130,-130));
				//[0803]CCLog(">[GS]:gray the tj:%d",tag);
			}
				
			break;
		}
	case 1:  // 0:change the color;
		{
			//[0803]CCLog(">[GS]change color:fade-%d,light-%d",last,tag);
			CCSprite* tcs =(CCSprite*) getChildByTag(last);
			if(last != 0&&tcs) {
				tcs->runAction(CCTintBy::create(0,-130,-130,-130));
				//[0803]CCLog(">[GS]:fade succes");
			}
				
			tcs =(CCSprite*) getChildByTag(tag);
			CCFiniteTimeAction* dr = CCTintBy::create(0,-130,-130,-130);
			if(tcs) {
				tcs->runAction(dr->reverse());
				//[0803]CCLog(">[GS]:Light succes");
			}
				

			last = tag;
			return true;
		}			
	case 2:
		{
			x = ts->getfloat("x");
			y = ts->getfloat("y");

			removeChildByTag(tag);
			string name = ts->getstring("name");

			CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
			GameManager::sharedLogicCenter()->f_cachetest(CCString::createWithFormat("%s.png",filename)->getCString());
			CCSpriteBatchNode *sheet = CCSpriteBatchNode::create(CCString::createWithFormat("%s.png",filename)->getCString());
			cache->addSpriteFramesWithFile(CCString::createWithFormat("%s.plist",filename)->getCString());

			CCSpriterX* m_animator = CCSpriterX::create(CCString::createWithFormat("%s.SCML",filename)->getCString());


			m_animator->setPosition(ccp(x,y));
			m_animator->setAnchorPoint(CCPoint(0.5,0.5));
			m_animator->setTag(tag);
			m_animator->setRotation(ts->getfloat("angel"));

			float scalex = ts->getfloat("scalex");
			float scaley = ts->getfloat("scaley");
			//if(scalex != 0)
			//	m_animator->setScaleX(scalex);
			//if(scaley != 0) 
			//	m_animator->setScaleY(scaley);

			if(scalex != 0) {
				m_animator->setScale(scalex);
				//[0803]CCLog(">Scale.");
			}
			int zorder = ts->getint("zorder");

			TagMap[name] = tag;
			PathMap[name] = filename;

			m_animator->PlayAnim(ts->getstring("animate"),ts->getint("repeat"));
			sheet->addChild(m_animator);
			addChild(sheet,ts->getint("zorder"),tag);
			break;
		}
	default:
		{
			x = ts->getfloat("x");
			y = ts->getfloat("y");
			if(y == 0) y = 100;

			string name = ts->getstring("name");
			removeChildByTag(tag);

			GameManager::sharedLogicCenter()->f_cachetest(filename);		//读入缓存文件，如果文件未缓存而filename不是本地文件，将会出现错误。
			
			cocos2d::CCSprite* t_cs = cocos2d::CCSprite::create(filename);
			t_cs->setPosition(ccp(x,y));
			t_cs->setAnchorPoint(CCPoint(0.5,0.5));
			t_cs->setTag(tag);
			t_cs->setRotation(ts->getfloat("angel"));
			int alpha = ts->getint("alpha");
			if(alpha == 0) alpha = 255;
			t_cs->setOpacity(alpha);									// Change default to No. If more is need, change the x flag.

			//[0803]CCLog(">[GS]:Loading...");
			int zorder = ts->getint("zorder");
			addChild(t_cs,ts->getint("zorder"));
			if(zorder == 0 && ts->getint("link") == 0){
				t_cs->runAction(CCTintBy::create(0,-130,-130,-130));
				//[0803]CCLog(">[GS]:Load and grey:%d",tag);
			}
			if(ts->getint("link") == 1) {		
				last = tag;
				//[0803]CCLog(">[GS]:Load and remain:%d",tag);
			}
			TagMap[name] = tag;
			PathMap[name] = filename;
			return true;
			break;
		}
	}

	

	//int x = ts->getfloat("x");
	//int y = ts->getfloat("y");
	//int tag = ts->getint("tag");
	//string name = ts->getstring("name");
	//removeChildByTag(tag);

	//GameManager::sharedLogicCenter()->f_cachetest(filename);		//读入缓存文件，如果文件未缓存而filename不是本地文件，将会出现错误。
	//CCSprite* tmp = CCSprite::create(filename);
	//tmp->setPosition(ccp(x,y));
	//tmp->setAnchorPoint(CCPoint(0.5,0.5));
	//tmp->setTag(tag);
	//tmp->setRotation(ts->getfloat("angel"));
	//addChild(tmp);
	//TagMap[name] = tag;
	//PathMap[name] = filename;

	return true;
}


/*

// <播放声音可采用，callfunction，这个可以留到以后再做
// <如果有新的效果需求可以在这里添加，限制为ccaction的子类，且只支持单target，设计上是立绘层，不要加入太多的逻辑。

*/
CCAction* ToTextLayer::DerAction(Script* ts, int indent){				
	do{																			
		int targettag = TagMap[ts->getstring("target")];
		const char* type = ts->getstring("type");
		CCAction* t_ca = NULL;
		static map<string,CCAction*> a_map;

		if(strcmp(type,"sequence") == 0 || strcmp(type,"spawn") == 0 || strcmp(type,"repeat") == 0){
			int num = ts->m_snum;
			CCArray* sl = ts->scriptnodes;
			CCArray* ss = CCArray::createWithCapacity(num);				//store the actions

			for(int i=0;i<num;i++){
				t_ca = DerAction((Script*) sl->objectAtIndex(i),indent+1);
				ss->insertObject(t_ca,i);				// XML parse error will end unknown throw out here.
			}

			/*
				// <repeat--repeatforever || 当repeat没有子节点时，可以提供name来获取已经定义的action，注意，repeat只接受间隔型操作。
			*/

			if(strcmp(type,"sequence") == 0){
				t_ca = CCSequence::create(ss);
			}else if(strcmp(type,"spawn") == 0){
				t_ca = CCSpawn::create(ss);
			}else if(strcmp(type,"repeat") == 0){		
				int tiTimes;		// <Repeat should be single, multi them by seq or spawn.
				tiTimes = ts->getint("times");
				std::string t_sname;
				t_sname = ts->getstring("name");

				if(strlen(t_sname.c_str())>0) t_ca = a_map[t_sname];		// <提供name将获得优先权
				if(tiTimes>0) 
					t_ca = CCRepeat::create((CCActionInterval*) t_ca,tiTimes);
				else 
					t_ca = CCRepeatForever::create((CCActionInterval*) t_ca);
			}


		}

		/*
			// <如果提供的原始action不是间隔型的话将会出错
		*/

		else if(strcmp(type,"moveby") == 0){
			t_ca = CCMoveBy::create(ts->getfloat("duration"), ccp(ts->getfloat("x"),ts->getfloat("y")));
			CCLOG("move by:%f,%f,%f",ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}
		else if(strcmp(type,"reverse") == 0){
			t_ca = a_map[ts->getstring("orig")];
			t_ca = ((CCActionInterval*) t_ca)->reverse();						
		}else if(strcmp(type,"moveto") == 0){
			t_ca = CCMoveTo::create(ts->getfloat("duration"), ccp(ts->getfloat("x"),ts->getfloat("y")));
			CCLOG("move to:%f,%f,%f",ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"place") == 0){
			t_ca = CCPlace::create(ccp(ts->getfloat("x"),ts->getfloat("y")));
		}else if(strcmp(type,"scaleto") == 0){
			t_ca = CCScaleTo::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"scaleby") == 0){
			t_ca = CCScaleBy::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"tintto") == 0){
			t_ca = CCTintTo::create(ts->getfloat("duration"), ts->getint("r"), ts->getint("g"), ts->getint("b"));
		}else if(strcmp(type,"tintby") == 0){
			t_ca =  CCTintBy::create(ts->getfloat("duration"), ts->getint("r"), ts->getint("g"), ts->getint("b"));
		}else if(strcmp(type,"rotateby") == 0){
			t_ca = CCRotateBy::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"rotateto") == 0){
			t_ca =  CCRotateTo::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"fadeto") == 0){
			t_ca =  CCFadeTo::create(ts->getfloat("duration"), ts->getint("alpha"));
		}




		if(indent == 0) {
			CCSprite* p = (CCSprite*) getChildByTag(targettag);
			p->runAction(t_ca);
			a_map.clear();
			/* getChildByTag(targettag)->runAction(tmp); */
		}else{
			string name = ts->getstring("name");
			if(name.length()>0) a_map[ts->getstring("name")] = t_ca;
			return t_ca;
		}
			
	} while(0);
	return false;
}

void ToTextLayer::DerFinal(Script* ts){
	m_bHasFinal = true;
	int num = ts->getint("total");
	CCArray* ss = ts->scriptnodes;
	for(int i=0;i<num;i++){		
		Script* tmp_s = (Script*) ss->objectAtIndex(i);
		switch(tmp_s->getint("type")){
		case 0:
			{
				m_stop[tmp_s->getstring("name")] = tmp_s->getint("tag");
				break;
			}
		case 1:
			{
				m_remove[tmp_s->getstring("name")] = tmp_s->getint("tag");	
				break;
			}
		default:
			m_bHasFinal = false;
		}
		
		
									
	}

}

void ToTextLayer::DerSilent(Script* ts){
	StateCenter::sharedStateCenter()->g_lock_check(ts);
};

/*
//menu->setVisible(m_bIsNoFade);
// <AT:将不会暂停正在运行的立绘的动作。因为需求比较小而遍历的成本相对较高。
*/
void ToTextLayer::FadeText(CCObject* sender){
	m_bTouchProt = false;
	m_bIsNoFade = !m_bIsNoFade;
	setVisible(m_bIsNoFade);
	m_sBox->setVisible(m_bIsNoFade);

	if(!m_bIsNoFade) pauseSchedulerAndActions();
	else resumeSchedulerAndActions();
	if(sel_menu){
		sel_menu->setVisible(m_bIsNoFade);
	}
	
}

// <更新label
// <[in] line 目标文字串
// <[in] dst false时更新文字，true时强制刷新为fullline最终版
void ToTextLayer::FlushText(const char* line,bool dst){
	CCLog(">[TextLayer] FlushText() | %s",line);
	if(m_bIsShownOver)	return;
	if(dst){	
		if(line){
			m_Label->setString(line);
		}
		
		//lines.append(fulline,cur,sum-cur);
		//m_Label->setString(lines.c_str());
		cns_blocks->removeAllChildren();
		cpn_textcn->setInverted(true);
		StopStream();
	}else{
		m_Label->setString(line);
	}
}

void ToTextLayer::StreamText(float dt){
	if(!m_bIsNoFade) return;
	
	if(cur<sum){
		int t_cur = vi_text.at(cur);
		++cur;

		switch(t_cur)
		{
		case(0):
			{
				m_fBlockY -= FONT_WIDTH;				//CHV:The height of one line = FONT_WIDTH;
				mLineCount = 0;
				StreamText(0);
				return;
			}
		case(1):
			{
				mLineCount += mSingeWidth/2;
				break;;
			}
		default:		//	2&&3
			{
				mLineCount += mSingeWidth;
				break;
			}
		}

		CCLog("Insert block at:%f,%f.",mLineCount,m_fBlockY);
		CCSprite* t_cs = CCSprite::create("Images/block.png");
		t_cs->setAnchorPoint(ccp(1,1));
		t_cs->setPosition(ccp(mLineCount,m_fBlockY));
		cns_blocks->addChild(t_cs);

	}else{
		CCSprite* t_cs = CCSprite::create("Images/block.png");
		t_cs->setAnchorPoint(ccp(1,1));
		t_cs->setPosition(ccp(mLineCount+FONT_WIDTH,m_fBlockY));
		cns_blocks->addChild(t_cs);
		CCLog("We should stopstream.");
		StopStream();
	}

}

void ToTextLayer::FormText(){
	for(int i = 0; i< sum;)
	{	
		
		if(mLineCount+mSingeWidth>m_textwidth){
			mLineCount = 0;
			++m_iLine;
			lines += '\n\r';
			vi_text.push_back(0);
		}
		char t = fulline[i];
		if(t=='\n'){
			mLineCount = 0;
			++m_iLine;
			++i;
			lines += '\n\r';
			vi_text.push_back(0);
			continue;
		} else if((t&0xE0) == 0xE0){	//3byte
			lines += t;
			lines += fulline[i+1];
			lines += fulline[i+2];
			i += 3;
			mLineCount += mSingeWidth;
			vi_text.push_back(3);
		}else if((t&0xC0) == 0xC0){//2byte
			lines += t;
			lines += fulline[i+1];
			i += 2;
			mLineCount += mSingeWidth;
			vi_text.push_back(2);
		}else{//1byte
			lines += t;
			i++;
			mLineCount += mSingeWidth/2;
			vi_text.push_back(1);
		}
		m_iTextcount++;
	}
	//lines = fulline;
	//[0803]CCLog("forming text:\n%s",lines.c_str());
}

void ToTextLayer::ShowText(const char* line, const char* name){

	m_Name->setString(name);

	cur = 0;
	m_iTextcount = 0;
	sum = strlen(line);
	fulline = line;
	lines.clear();
	mLineCount = 0;
	vi_text.clear();
	m_iLine = 0;
	FormText();

	m_bIsShownOver = false;
	cns_blocks->removeAllChildren();
	cpn_textcn->setInverted(false);

	sum = m_iLine + m_iTextcount;
	mLineCount = 0;
	m_fBlockY = m_height;

	if(m_bIsSkip){
		FlushText(lines.c_str(),true);
	}else{
		CCLog(">[TextLayer]ShowText() || No Skip Mode.");
		FlushText(lines.c_str(),false);
		/* // <在这里配置文字间隔 */
		this->schedule( schedule_selector(ToTextLayer::StreamText),m_fTText);		
	}
}

void ToTextLayer::StartSkip(CCObject* sender){
	m_bTouchProt = false;
	if(!sender || m_bIsSkip){
		m_bIsSkip = false;
		if(e_layerstate == 4) e_layerstate = 0;
		unscheduleUpdate();
		if(m_bIsAuto) RegistAA();
	}else{
		if(e_layerstate != 1){
			m_bIsSkip = true;
			mLockNext = true;
			e_layerstate = 4;
			FlushText(NULL,true);
			scheduleUpdate();
		}
	}
}

void ToTextLayer::StartAuto(CCObject* sender){
	m_bTouchProt = false;
	if(!sender || m_bIsAuto){		//关闭自动模式
		stopActionByTag(AUTO_ACTION_TAG);
		m_bIsAuto = false;
		if(e_layerstate == 3){
			e_layerstate = 0;
		}
	}else{			
		m_bIsAuto = true;
		if(e_layerstate == 0){
			if(m_bIsShownOver) 
				RegistAA();
		}
	}
}

void ToTextLayer::AutoNext(){
	CCLog(">Auto Next");
	e_layerstate = 3;
	stopActionByTag(AUTO_ACTION_TAG);
	if(SoundManager::sharedSoundManager()->QueryCVSound()) StepNext();
	else RegistAA();
}

void ToTextLayer::RegistAA(){
	if(e_layerstate == 1) return;
	e_layerstate = 3;
	CCLOG(">RegistAA");
	stopActionByTag(AUTO_ACTION_TAG);
	CCAction* au = CCSequence::create(CCDelayTime::create(m_fTAuto),CCCallFunc::create(this,callfunc_selector(ToTextLayer::AutoNext)),NULL);
	au->setTag(AUTO_ACTION_TAG);
	runAction(au);
}

void ToTextLayer::StopStream(){					//文字显示完成后调用
	CCLog(">[TextLayer] StopStream()");
	m_bIsShownOver = true;
	this->unschedule( schedule_selector(ToTextLayer::StreamText));
	if(m_bIsSkip) {
		return;
	}
	if(m_bIsAuto){
		RegistAA();
	}else{
		if(e_layerstate == 3) e_layerstate = 0;
	}
}

void ToTextLayer::update(float dt)
{
	//CCLog(">[ToTextLayer] update");
	if(mLockNext){
		if(e_layerstate == 1) return;
		static float skip = 0;
		skip += dt;
		if(skip > SKIP_TIME){
			skip = 0;
			StepNext();
		}
	}else{
		GameManager::sharedLogicCenter()->e_gonext();
		mLockNext = true;
	}

}

void ToTextLayer::StepNext(){
	CCLog(">StepNext");
	if(e_layerstate == 1) return;
	if(m_bHasFinal){
		// <执行清理，将登记的name，tag对清除或暂停

		map<string,int>::iterator it=m_remove.begin();
		for(;it!=m_remove.end();++it){
			removeChildByTag(it->second,true);
			TagMap.erase(it->first);
			PathMap.erase(it->first);
		};

		it=m_stop.begin();
		for(;it!=m_stop.end();++it){
			getChildByTag(it->second)->stopAllActions();
		};


		/* <清理工作 */
		m_bHasFinal = !m_bHasFinal;
		m_remove.clear();
		m_stop.clear();

		/* <如果要支援存档，请在这里对各项属性进行登记 */
	}
	if(lockstate){
		Script* jp = (Script*) lockstate->objectForKey(-1);
		if(jp) StateCenter::sharedStateCenter()->g_lock_change(jp);
		CC_SAFE_RELEASE_NULL(lockstate);
	}
	if(m_bIsSkip) mLockNext = false;
	else GameManager::sharedLogicCenter()->e_gonext();
}

bool ToTextLayer::click(CCTouch *touch, CCEvent * pEvent)
{
	if(!m_bIsNoFade) { FadeText(NULL);return true;}
	if(!this->isTouchEnabled()) return true;
	CCLog(">[TextLayer] click() | e_layerstate:%d",e_layerstate);
	switch(e_layerstate){
	case 0:	//text showing
		{
			if(!m_bIsShownOver){
				this->unscheduleAllSelectors();	
				CCLOG("touch began ,prepareing flush.");
				FlushText(fulline.c_str(),true);
			}else{
				StepNext();
			}
			break;
		}
	case 1:	//do not go ahead. It's a lock.
		{
			CCLOG("TextLayer State 0x001. Lock.");
			break;
		} 
	case 2:	/* <吃掉MenuSel的事件*/
		{
			e_layerstate = 0;
			break;
		}
	case 3:	/* <Auto Mode接受到Click事件后自动结束Auto? */
		{
			break;
		}
	case 4: /* Skip Mode接受Click后直接结束 */
		{
			StartSkip(NULL);
			mvBtns[1]->onNormal();
			break;
		}
	}


	return true;
}

void ToTextLayer::DerLock(Script* ts)
{
	CC_SAFE_RELEASE_NULL(lockstate);
	lockstate = new CCDictionary();
	CCArray* locks = ts->scriptnodes;
	int num = ts->m_snum;
	for(int i = 0;i<num;i++){
		Script* tmp =(Script*) locks->objectAtIndex(i);
		lockstate->setObject(tmp,tmp->getint("jump"));
	}
}

bool ToTextLayer::DerSelMenu(Script* ts){
	do{
		CC_BREAK_IF(!ts);
		//CCDictionary* attrs = ts->attributes;
		CCArray* scrs = ts->scriptnodes;
		int n = ts->m_snum;

		CCArray* tm = CCArray::createWithCapacity(n);
		for(int i=0;i<n;i++){
			Script* tmp = (Script*) scrs->objectAtIndex(i);		//set more here if you want more.
			CCMenuItemFont::setFontName("Marker Felt");
			CCMenuItemFont *item6 = CCMenuItemFont::create(tmp->getstring("content"), this, menu_selector(ToTextLayer::menucallback));
			item6->setTag(tmp->getint("jump"));
			tm->insertObject(item6,i);
		}
		sel_menu = MouseMenu::createWithArray(tm);	

		sel_menu->setTouchEnabled(false);
		sel_menu->alignItemsHorizontally();
		sel_menu->setTag(MENUTAG);
		sel_menu->setPosition(ccp(s.width/2, s.height/2));
		addChild(sel_menu);
		e_layerstate = 1;

		return true;
	}while(0);
	return false;
}

void ToTextLayer::beforesnap()
{
	m_bSnap = cpn_textcn->isInverted();
	cpn_textcn->setInverted(true);
	cns_blocks->setVisible(false);
	//cpn_textcn->retain();
	//cpn_textcn->removeFromParentAndCleanup(false);
}

void ToTextLayer::aftersnap()
{
	//addChild(cpn_textcn);
	cpn_textcn->setInverted(m_bSnap);
	cns_blocks->setVisible(true);
}

void ToTextLayer::GS_Lock()
{
	setTouchEnabled(false);

	if(m_bIsSkip) StartSkip(NULL);
	//if(!m_bIsAuto) StartAuto(NULL);
	if(!m_bIsNoFade)  FadeText(NULL);

	m_bIsAuto = true;
	if(e_layerstate == 0){
		if(m_bIsShownOver) 
			RegistAA();
	}
	mvBtns[0]->onSelect();
	mvBtns[1]->onNormal();
}

void ToTextLayer::GS_unLock()
{
	StartAuto(NULL);
	mvBtns[0]->onNormal();
	setTouchEnabled(true);
}

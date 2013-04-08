#include "sublayer/TextLayer.h"

#include "GameManager.h"
#include "StateCenter.h"

#include "utils/MouseMenu.h"


#define TEXTLAYER_PRIO_TOUCH 2
#define AUTO_ACTION_TAG 1001
#define SKIP_TIME 0.1

using namespace std;

CCDictionary* TextLayer::lockstate = NULL;

void TextLayer::Close(){
	Pause();	
	//FlushText("",true);
	m_Label->setString("");
	setVisible(false);
}

void TextLayer::Pause(){
	setTouchEnabled(false);
	this->unschedule( schedule_selector(TextLayer::StreamText));
	unscheduleUpdate();
}

void TextLayer::Resume(){
	GetTimePara();
	setTouchEnabled(true);
	setVisible(true);
	this->schedule( schedule_selector(TextLayer::StreamText), m_fTText);
	if(m_bIsSkip) scheduleUpdate();
}

TextLayer::TextLayer() {
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

void TextLayer::GetTimePara(){
	m_fTAuto = ConfigManager::sharedConfigManager()->GetConfigF(CM_AUTOTIME,100);
	m_fTText = ConfigManager::sharedConfigManager()->GetConfigF(CM_TEXTSPEED,1000);
}

void TextLayer::ELoad(){
	Script* t_sc = StateCenter::sharedStateCenter()->m_scTags;
	for(int i = 0; i<t_sc->m_snum; ++i){
		Script* t = (Script*) t_sc->scriptnodes->objectAtIndex(i);
		DerLoadImg(t);
	}
}

void TextLayer::onEnter()
{
	TagMap.clear();
	PathMap.clear();
	s = CCDirector::sharedDirector()->getWinSize();
	m_bHasFinal = false;

	m_height = 200;
	m_avatarwidth = m_height;
	m_textwidth = s.width - m_avatarwidth;
	layer1 = CCLayerColor::create( ccc4(255, 255, 0, 80), 0.99f * s.width, m_height);
	layer1->setAnchorPoint(CCPoint(0.5,0));
	layer1->setPosition(ccp(s.width/2,0));
	layer1->ignoreAnchorPointForPosition(false);
	addChild(layer1, 1);

	m_Label = CCLabelTTF::create("", "fonts/msyhbd.ttf", 24, CCSize(m_textwidth, m_height), kCCTextAlignmentLeft);
	mSingeWidth = 24;

	//m_Label->setColor(ccc3(0,0,0));
	m_Label->setPosition( ccp((s.width + m_avatarwidth )/ 2, m_height) );
	m_Label->setAnchorPoint(CCPoint(0.5,1));
	layer1->addChild(m_Label);



	CCMenuItemFont *item4 = CCMenuItemFont::create("Fade", this, menu_selector(TextLayer::FadeText) );
	item4->setFontSizeObj(20);
	item4->setFontName("Marker Felt");

	CCMenuItemFont *item5 = CCMenuItemFont::create("Auto", this, menu_selector(TextLayer::StartAuto) );
	item5->setFontSizeObj(20);
	item5->setFontName("Marker Felt");

	CCMenuItemFont *item6 = CCMenuItemFont::create("Skip", this, menu_selector(TextLayer::StartSkip) );
	item6->setFontSizeObj(20);
	item6->setFontName("Marker Felt");

	menu = MouseMenu::menuWithItems(item4, item5, item6, NULL);;
	menu->alignItemsHorizontallyWithPadding(2);

	menu->setPosition(ccp((s.width + m_avatarwidth )/ 2, m_height + 24));
	menu->setAnchorPoint(CCPoint(0.5,0));
	
	
	addChild(menu,1);
	BYLayerModal::onEnter();
	
	
}

void TextLayer::menucallback(CCObject* sender)
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

bool TextLayer::DerLoadImg(Script* ts){	//meta
	const char* filename = ts->getstring("content");
	int x = ts->getfloat("x");
	int y = ts->getfloat("y");
	int tag = ts->getint("tag");
	string name = ts->getstring("name");
	removeChildByTag(tag);

	GameManager::sharedLogicCenter()->f_cachetest(filename);		//读入缓存文件，如果文件未缓存而filename不是本地文件，将会出现错误。
	CCSprite* tmp = CCSprite::create(filename);
	tmp->setPosition(ccp(x,y));
	tmp->setAnchorPoint(CCPoint(0.5,0.5));
	tmp->setTag(tag);
	tmp->setRotation(ts->getfloat("angel"));
	addChild(tmp);
	TagMap[name] = tag;
	PathMap[name] = filename;

	return true;
}

CCAction* TextLayer::DerAction(Script* ts, int indent){				//如果有新的效果需求可以在这里添加，限制为ccaction的子类，且只支持单target，设计上是立绘层，不要加入太多的逻辑。
	do																//播放声音可采用，callfunction，这个可以留到以后再做
	{		
		int targettag = TagMap[ts->getstring("target")];
		const char* type = ts->getstring("type");
		CCAction* tmp = NULL;
		static map<string,CCAction*> a_map;

		if(strcmp(type,"sequence") == 0 || strcmp(type,"spawn") == 0 || strcmp(type,"repeat") == 0){
			int num = ts->m_snum;
			CCArray* sl = ts->scriptnodes;
			CCArray* ss = new CCArray(num);				//store the actions

			for(int i=0;i<num;i++){
				tmp = DerAction((Script*) sl->objectAtIndex(i),indent+1);
				ss->insertObject(tmp,i);				//xml编码错误将会导致这边添加进未知的内容
			}

			if(strcmp(type,"sequence") == 0){
				tmp = CCSequence::create(ss);
			}else if(strcmp(type,"spawn") == 0){
				tmp = CCSpawn::create(ss);
			}else if(strcmp(type,"repeat") == 0){		//当repeat的参数为0时，为repeatforever；当repeat没有子节点时，可以提供name来获取已经定义的action，注意，repeat只接受间隔型操作。
				int times = ts->getint("times");		//不要给repeat多个子节点，可以使用sequence或者spawn封装一次
				string name = ts->getstring("name");
				if(strlen(name.c_str())>0) tmp = a_map[name];		//提供name将获得优先权
				if(times>0) tmp = CCRepeat::create((CCActionInterval*) tmp,times);
				else tmp = CCRepeatForever::create((CCActionInterval*) tmp);
			}


		}

		else if(strcmp(type,"moveby") == 0){
			tmp = CCMoveBy::create(ts->getfloat("duration"), ccp(ts->getfloat("x"),ts->getfloat("y")));
			CCLOG("move by:%f,%f,%f",ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}
		else if(strcmp(type,"reverse") == 0){
			tmp = a_map[ts->getstring("orig")];
			tmp = ((CCActionInterval*) tmp)->reverse();						//如果提供的原始action不是间隔型的话将会出错
		}
		else if(strcmp(type,"moveto") == 0){
			tmp = CCMoveTo::create(ts->getfloat("duration"), ccp(ts->getfloat("x"),ts->getfloat("y")));
			CCLOG("move to:%f,%f,%f",ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"place") == 0){
			tmp = CCPlace::create(ccp(ts->getfloat("x"),ts->getfloat("y")));
		}else if(strcmp(type,"scaleto") == 0){
			tmp = CCScaleTo::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"scaleby") == 0){
			tmp = CCScaleBy::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"tintto") == 0){
			tmp = CCTintTo::create(ts->getfloat("duration"), ts->getint("r"), ts->getint("g"), ts->getint("b"));
		}else if(strcmp(type,"tintby") == 0){
			tmp =  CCTintBy::create(ts->getfloat("duration"), ts->getint("r"), ts->getint("g"), ts->getint("b"));
		}else if(strcmp(type,"rotateby") == 0){
			tmp = CCRotateBy::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}else if(strcmp(type,"rotateto") == 0){
			tmp =  CCRotateTo::create(ts->getfloat("duration"), ts->getfloat("x"),ts->getfloat("y"));
		}




		if(indent == 0) {
			CCSprite* p = (CCSprite*) getChildByTag(targettag);
			p->runAction(tmp);
			a_map.clear();
			//getChildByTag(targettag)->runAction(tmp);
		}else{
			string name = ts->getstring("name");
			if(name.length()>0) a_map[ts->getstring("name")] = tmp;
			return tmp;
		}
			
	} while (0);
	return false;
}

void TextLayer::DerFinal(Script* ts){
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

void TextLayer::DerSilent(Script* ts){
	StateCenter::sharedStateCenter()->g_lock_check(ts);
};

void TextLayer::FadeText(CCObject* sender){
	m_bTouchProt = false;
	m_bIsNoFade = !m_bIsNoFade;
	setVisible(m_bIsNoFade);
	layer1->setVisible(m_bIsNoFade);
	menu->setVisible(m_bIsNoFade);
	//AT:将不会暂停正在运行的立绘的动作。因为需求比较小而遍历的成本相对较高。
	if(!m_bIsNoFade) pauseSchedulerAndActions();
	else resumeSchedulerAndActions();
	if(sel_menu){
		sel_menu->setVisible(m_bIsNoFade);
	}
	
}

//更新label
//[in] line 目标文字串
//[in] dst false时更新文字，true时强制刷新为fullline最终版
void TextLayer::FlushText(const char* line,bool dst){
	if(m_bIsShownOver)	return;
	if(dst){		
		lines.append(fulline,cur,sum-cur);
		m_Label->setString(lines.c_str());
		StopStream();
	}else{
		m_Label->setString(line);
	}
}

void TextLayer::ShowText(const char* line){

	cur = 0;
	m_iTextcount = 0;
	sum = strlen(line);
	fulline = line;
	lines.clear();
	mLineCount = 0;
	m_bIsShownOver = false;
	
	if(m_bIsSkip){
		FlushText(line,true);
	}else{
		this->schedule( schedule_selector(TextLayer::StreamText),m_fTText);		//在这里配置文字间隔
	}
}

void TextLayer::StartSkip(CCObject* sender){
	m_bTouchProt = false;
	if(!sender || m_bIsSkip){
		m_bIsSkip = false;
		if(e_layerstate == 4) e_layerstate = 0;
		unscheduleUpdate();
	}else{
		if(e_layerstate != 1){
			m_bIsSkip = true;
			e_layerstate = 4;
			FlushText(NULL,true);
			scheduleUpdate();
		}
	}
}

void TextLayer::StartAuto(CCObject* sender){
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

void TextLayer::AutoNext(){
	CCLOG(">Auto Next");
	e_layerstate = 3;
	stopActionByTag(AUTO_ACTION_TAG);
	StepNext();
}

void TextLayer::RegistAA(){
	if(e_layerstate == 1) return;
	e_layerstate = 3;
	CCLOG(">RegistAA");
	CCAction* au = CCSequence::create(CCDelayTime::create(m_fTAuto),CCCallFunc::create(this,callfunc_selector(TextLayer::AutoNext)),NULL);
	au->setTag(AUTO_ACTION_TAG);
	runAction(au);
}

void TextLayer::StopStream(){					//文字显示完成后调用
	m_bIsShownOver = true;
	this->unschedule( schedule_selector(TextLayer::StreamText));
	if(m_bIsSkip) {
		return;
	}
	if(m_bIsAuto){
		RegistAA();
	}else{
		if(e_layerstate == 3) e_layerstate = 0;
	}
}

void TextLayer::StreamText(float dt){
	if(!m_bIsNoFade) return;
	if(cur<sum){
		if(mLineCount+mSingeWidth>m_textwidth){
			mLineCount = 0;
			lines += '\n';
		}

		char t = fulline[cur];
		if((t&0xE0) == 0xE0){	//3byte
				lines += t;
				lines += fulline[cur+1];
				lines += fulline[cur+2];
				cur += 3;
				mLineCount += mSingeWidth;
		}else if((t&0xC0) == 0xC0){//2byte
				lines += t;
				lines += fulline[cur+1];
				cur += 2;
				mLineCount += mSingeWidth;
		}else{//1byte
			lines += t;
			cur++;
			mLineCount += mSingeWidth/2;
		}
		//CCLOG("streaming text:%s",lines.c_str());
		m_iTextcount++;
		FlushText(lines.c_str());
	}else{
		StopStream();
	}

}

void TextLayer::update(float dt)
{
	if(e_layerstate == 1) return;
	static float skip = 0;
	skip += dt;
	if(skip > SKIP_TIME){
		skip = 0;
		StepNext();
	}
}

void TextLayer::StepNext(){
	CCLOG(">StepNext");
	if(e_layerstate == 1) return;
	if(m_bHasFinal){
		//执行清理，将登记的name，tag对清除或暂停

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


		//清理工作
		m_bHasFinal = !m_bHasFinal;
		m_remove.clear();
		m_stop.clear();

		//如果要支援存档，请在这里对各项属性进行登记
	}
	if(lockstate){
		Script* jp = (Script*) lockstate->objectForKey(-1);
		if(jp) StateCenter::sharedStateCenter()->g_lock_change(jp);
		CC_SAFE_RELEASE_NULL(lockstate);
	}
	GameManager::sharedLogicCenter()->e_gonext();
}

bool TextLayer::click(CCTouch *touch, CCEvent * pEvent)
{
	if(!m_bIsNoFade) { FadeText(NULL);return true;}
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
	case 2:	//吃掉MenuSel的事件
		{
			e_layerstate = 0;
			break;
		}
	case 3:	//Auto Mode接受到Click事件后自动结束Auto?
		{
			break;
		}
	case 4: //Skip Mode接受Click后直接结束
		{
			StartSkip(NULL);
			break;
		}
	}


	return true;
}

void TextLayer::DerLock(Script* ts)
{
	lockstate = new CCDictionary();
	CCArray* locks = ts->scriptnodes;
	int num = ts->m_snum;
	for(int i = 0;i<num;i++){
		Script* tmp =(Script*) locks->objectAtIndex(i);
		lockstate->setObject(tmp,tmp->getint("jump"));
	}
}

bool TextLayer::DerSelMenu(Script* ts){
	do{
		CC_BREAK_IF(!ts);
		//CCDictionary* attrs = ts->attributes;
		CCArray* scrs = ts->scriptnodes;
		int n = ts->m_snum;

		CCArray* tm = new CCArray(n);
		for(int i=0;i<n;i++){
			Script* tmp = (Script*) scrs->objectAtIndex(i);		//set more here if you want more.
			CCMenuItemFont::setFontName("Marker Felt");
			CCMenuItemFont *item6 = new CCMenuItemFont();
			item6 = CCMenuItemFont::create(tmp->getstring("content"), this, menu_selector(TextLayer::menucallback));
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
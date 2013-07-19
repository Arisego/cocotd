#include "System.h"

#include "pthread.h"

#include "GameScene.h"

#include "GameManager.h"
#include "SoundManager.h"
#include "CCSpriterX.h"

#include "utils/Scriptor.h"
#include "utils/ScirptDer.h"
#include "sublayer/InfoTab.h"
#include "utils/EffectControler.h"
#include "packui/InterfaceEffect.h"


#define SELTAG 1010
#define FRETAIN CCCallFunc::create(this,callfunc_selector(GameScene::FlagRetain))
#define FRELEASE CCCallFunc::create(this,callfunc_selector(GameScene::FlagRelease))

using namespace cocos2d;

typedef enum
{
	tLySpalash = 0,
	tLyMap,
	tLySpot,
	tLyCG,
	tLyText,
	tLyMovie,
	tLyMod
}LayerTag;

CCDictionary* Imgstack = NULL;


// 逻辑中心转移到这里
bool GameScene::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! CCScene::init());
		ml = NULL;
		snapshot = NULL;
		ScriptList = NULL;

		m_StageState = 0;
		CC_SAFE_RELEASE_NULL(Imgstack);
		this->scheduleUpdate();
		
        bRet = true;

    } while (0);

    return bRet;
}
//////////////////////////////////////////////////////////////////////////
// pThread for init.

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool initover;
int sum, coun;
Scriptor* s_sp;
string fscp;

GameScene::~GameScene(){

	
//	Imgstack->removeAllObjects();
	if(initover) 
		CC_SAFE_RELEASE_NULL(Imgstack);
	CC_SAFE_RELEASE_NULL(ScriptList);

	CC_SAFE_RELEASE_NULL(snapshot);
//	CC_SAFE_RELEASE_NULL(tMovie);
//	ALSingle::purgeSharedALSingle();
	
	CCLOG(">GameScene Destruct.");				//ERR:122;
}

void cleanup(void *arg){  
	initover = true;
}  

void *initscript(void *arg){ 
	//pthread_mutex_lock(&mutex);
	CCLOG(">T_Scrip Begin");
	s_sp = new Scriptor();
	s_sp->parse_file(fscp.c_str());
	CCLOG(">T_Scrip End");
	pthread_mutex_unlock(&mutex);
	return ((void *)0); 
}

void *initmusic(void *arg){ 
	CCLOG(">T_Music:Begin.");
	//SoundManager::sharedSoundManager()->PreLoadSrc("d");	
	CCArray* mss = s_sp->initcs;
	int i = 0;
	Script* s;
	for(;i<mss->count();++i){
		s = (Script*) mss->objectAtIndex(i);
		if(s->getint("type") == 2) break;			// Initcs -- Type == 2 -- Music Initilize. 唯一
	//	CC_SAFE_RELEASE_NULL(s);
	}
	CCLOG(">T_Music:Preparing Buffer.");
	CCArray* sss = s->scriptnodes;
	Script* t;
	for(i=0;i<s->m_snum;++i){
		t = (Script*) sss->objectAtIndex(i);
		SoundManager::sharedSoundManager()->PreLoadSrc(t->getstring("path"));
	}
	CCLOG(">T_Music:Return.");
	return ((void *)0);
}

void initImg(Script* sp){
	CCLOG(">InitImg. Thread. Begin.%s",sp->getstring("path"));
	
	unsigned long filesize;
	FileIO* fi = new FileIO(sp->getstring("path"));

	if(!Imgstack) Imgstack = new CCDictionary();
	for(int i = 0;i<sp->m_snum;++i){
		Script* t = (Script*) sp->scriptnodes->objectAtIndex(i);	//TODO:根据后缀名改变Image的读入类型
		CCLOG(">>Inert Img. %s. Begin.", t->getstring("path"));

		CCImage* image = new CCImage();
		unsigned char* pBuffer = fi->getFileData(t->getstring("copname"),&filesize);
		image->initWithImageData((void*)pBuffer, filesize, CCImage::kFmtPng);
		delete pBuffer;
		ImageInfo *pImageInfo = new ImageInfo();
		//pImageInfo->autorelease();

		pImageInfo->image = image;
		pImageInfo->imageType = CCImage::kFmtPng;		

		Imgstack->setObject(pImageInfo,t->getstring("path"));	//TODO:根据脚本载入资源
		pImageInfo->autorelease();

		CCLOG(">>Inert Img. %s. End.", t->getstring("path"));
	}



	CCLOG(">InitImg. Thread. Out.");
	CC_SAFE_DELETE(fi);
}

void *initfiles(void *arg){ 
	CCLOG(">T_InitFile:Begin.");
	static int counter = 0;

//	pthread_cleanup_push(cleanup,"thread 1 first handler");
	

	int err;
	pthread_t tid2;
		//,music;		
	pthread_mutex_lock(&mutex);
	err = pthread_create(&tid2,NULL,initscript,(void *)1); 
	if( err != 0){  
		fprintf(stderr,"create thread2 failed: %s",strerror(err));  
		exit(1);  
	} 
	//pthread_join(tid2,NULL);
	CCLOG(">PT_LOCK");
	pthread_mutex_lock(&mutex);
	CCLOG(">PT_LOCK_PASS");
	//Img Cache Begin.Use Png only.
	CCArray* mss = s_sp->initcs;
	int i = 0;
	Script* s;
	for(;i<mss->count();++i){
		s = (Script*) mss->objectAtIndex(i);
		if(s->getint("type") == 1) initImg(s);			// Initcs -- Type == 1 -- Png Pack. Multi.
		//CC_SAFE_RELEASE_NULL(s);
	}


//	pthread_create(&music,NULL,initmusic,(void *)1);
//	pthread_join(music,NULL);
	pthread_mutex_unlock(&mutex);

	CCLOG(">T_InitFile:Return.");
	initover = true;
//	pthread_cleanup_pop(1);  
	return ((void *)0);   
}
//////////////////////////////////////////////////////////////////////////
bool GameScene::f_cachetest(const char* rscMask){	//移动缓存并记录进map
	do 
	{	
		ImageInfo *img = (ImageInfo *) m_pImages->objectForKey(rscMask);
		CC_BREAK_IF(!img);

		CCTextureCache::sharedTextureCache()->addUIImage(img->image,rscMask);
		m_pImages->removeObjectForKey(rscMask);
	} while (0);
	return true;
}

void GameScene::f_initover(){
	this->removeChildByTag(tLySpalash,true);	//移除并清理splash层

	m_pImages = Imgstack;
	ScriptList = s_sp->m_caScript;
	ScriptList->retain();
	m_IScriptSum = s_sp->sn;
	CC_SAFE_DELETE(s_sp);
	
	//////////////////////////////////////////////////////////////////////////
	
	

	ml = new MapLayer();
	ml->autorelease();
	this->addChild(ml,tLyMap);
	
	ml->f_init();
	AddState(ml);


	//从这里开始展开textlayer
	//ImageInfo *img = (ImageInfo *) m_pImages->objectForKey("Images/background.png");
	//CCTextureCache::sharedTextureCache()->addUIImage(img->image,"Images/background.png");
	//m_pImages->removeObjectForKey("Images/background.png");

	//背景层
	bg = CCLayer::create();
	BgImg = NULL;
	this->addChild(bg,tLyCG);
	

	m_IsTextShown = false;
	te = new TextLayer();
	te->autorelease();
	e_curscript = 0;
	addChild(te,tLyText);
	AddState(te);
	
	m_IsTextShown = true;
	e_TextAuto = false;
	//
	tMovie = new MovieLayer();
	tMovie->autorelease();
	AddState(tMovie);
	this->addChild(tMovie,tLyMovie);

	//tMod = new ModelLayer();
	//AddState(tMod);
	//addChild(tMod,tLyMod);

	//textlayer调试在这里结束			以上就是进入textlayer需要的入口

	StateCenter* t_sc = StateCenter::sharedStateCenter();
	if(t_sc->m_bIsLoad){
		if(! t_sc->g_load_file()) exit(0x5001);
		e_curscript = t_sc->m_iJump;
		//e_State = t_sc->m_oldstate;
		t_curscript = t_sc->m_iTJump;
		te->ELoad();

		m_sBgi = t_sc->m_sBgi;
		addBackImg();
		tMovie->cleanupplayer();
		e_update(t_sc->m_oldstate);

		if(e_State == 1){
			tScriptList = ml->tm->f_scrfind(ml->tm->m_sNaScp,ml->tm->m_iNaScp)->scriptnodes;
			t_ssum = tScriptList->count();
		}

		e_act();
		if(e_State != t_sc->m_iState) {
			CCLOG("Loading may be incorrect!Tring to fix it automaticly.");
			e_update(t_sc->m_iState);
		}
		m_bLoadProtect = true;

	}else{

		e_update(0);
		e_act();
		m_bLoadProtect = false;
	}


}

void GameScene::TextChange(float dt){
	//if(!m_IsTextShown){
	//	m_IsTextShown = true;

	//}else{

	//	
	//	m_IsTextShown = false;
	//}
	//te->setVisible(m_IsTextShown);
}

bool GameScene::e_getscript(){
	return false;
}

void GameScene::onExit(){
//	CC_SAFE_RELEASE_NULL(tMovie);
	CCScene::onExit();
}

void GameScene::update(float dt)	//负责整个scene的初始化
{
	switch(m_StageState){
	case(0):			// 0 --> stage未初始化
		{
			visibleSize = CCDirector::sharedDirector()->getVisibleSize();
			origin = CCDirector::sharedDirector()->getVisibleOrigin();
			initover = false;

			if(!Imgstack) Imgstack = new CCDictionary();
			Imgstack->removeAllObjects();

			m_pImages = NULL;

			StateCenter* t_sc = StateCenter::sharedStateCenter();
			if(t_sc->m_bIsLoad){
				scpfile = t_sc->m_sName;
			}


			fscp = scpfile;
			
	
			//GameManager::sharedLogicCenter()->getPosition();
			int err;  
			pthread_t tid1;	
			///thread moving to end?





			//////////////////////////////////////////////////////////////////////////

			CCLayer* SplashLayer = CCLayer::create();
			CCLabelTTF* pLabel = CCLabelTTF::create("LOADING...", "Arial", 24);
			CC_BREAK_IF(! pLabel);

			// Get window size and place the label upper. 
			CCSize size = CCDirector::sharedDirector()->getWinSize();
			pLabel->setPosition(ccp(size.width / 2, size.height - 50));

			// Add the label to HelloWorld layer as a child layer.
			SplashLayer->addChild(pLabel, 1);
			SplashLayer->setTouchEnabled(false);

			CCSpriteFrameCache *cache = CCSpriteFrameCache::sharedSpriteFrameCache();
			CCSpriteBatchNode *sheet = CCSpriteBatchNode::create("sprite/gongbin.png");
			cache->addSpriteFramesWithFile("sprite/gongbin.plist");
			SplashLayer->addChild(sheet);
			CCSpriterX *animator;
			animator = CCSpriterX::create("sprite/gongbin.SCML");


			animator->setPosition(ccp(400, 100));
			animator->setAnchorPoint(ccp(0,0));
			animator->setScale(0.8f);
			animator->PlayAnim("Idle");

			sheet->addChild(animator);

			this->addChild(SplashLayer,100,tLySpalash);
			//SoundManager::sharedSoundManager()->PlayMusic();
			//SoundManager::sharedSoundManager()->PreLoadSrc("d");
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			err = pthread_create(&tid1,NULL,initfiles,(void *)1);  

			if( err != 0){  
				fprintf(stderr,"create thread1 failed: %s",strerror(err));  
				exit(1);  
			} 
			//////////////////////////////////////////////////////////////////////////

			m_StageState = 1;
			break;
		}
	case(1):			// 1 --> stage初始化中
		{
			if(initover) m_StageState = 2;
			else CCLOG("Init being called.Render the splash.");
			break;
		}
	case(2):			//2 --> stage初始化完成
		{
			f_initover();	
			m_StageState = 3;
			unscheduleUpdate();
			m_flag = 0;
			break;
		}

	case(3):			//3 --> stage进入循环
		{
			if(m_bCanSnap && m_bScrLock){
				m_bScrLock = false;
				unscheduleUpdate();
				e_act();
			}
			break;
			//unschedule(update);
		}
	default:
		{
			//CCLOG("!!!!!!!!!!!Unkonwn stage state.");
			exit(1);
		}
	}
} 

void GameScene::Snap(){
	//if(!m_bCanSnap) return;
	CC_SAFE_RELEASE_NULL(snapshot);
	ml->show_hud();
	ml->beforesnap();
	if(te) te->beforesnap();
	GameManager::sharedGameManager()->prePareSave();

	CCDirector::sharedDirector()->setNextDeltaTimeZero(true);
	CCSize winsize = CCDirector::sharedDirector()->getWinSize();
	//CCLayerColor* whitePage = CCLayerColor::layerWithColor(ccc4(255, 255, 255, 0), winsize.width, winsize.height);
	CCRenderTexture* rtx = CCRenderTexture::renderTextureWithWidthAndHeight(winsize.width, winsize.height);
	rtx->setPosition(winsize.width / 2, winsize.height / 2);
	//rtx->beginWithClear(0.5,1,1,0);
	rtx->begin();

	ml->snap();
	visit();

	rtx->end();

	ml->close_hud();
	addChild(ml,tLyMap);
	ml->aftersnap();
	if(te) te->aftersnap();
	
	GameManager::sharedGameManager()->finisSave();
	//snapshot = new CCImage();
	snapshot = rtx->newCCImage();
}

void GameScene::PrepareSave(){
	StateCenter::sharedStateCenter()->f_get_state();
	Snap();
}

void GameScene::PreQuit(){
	//PrepareSave();
	StatesManager::PreQuit();
	ml->Close();
}

void GameScene::e_update(int new_State)	
{
	if(e_State == new_State) return;
	las_state = e_State;
	switch(e_State){
	case 3:
		{
			tMovie->Close();
			break;
		}
	case 1:
		{
			m_bLoadProtect = false;
			break;
		}
	}



	switch(new_State){
	case 0:		//showing texts
		{
			ActiveOnly(te);
			break;
		}
	case 1:		//showing msp texts
		{
			ActiveOnly(te);			
			break;
		}
	case 2:		//地图控制
		{
			m_bLoadProtect = false;
			OpenOnly(ml);
			break;
		}
	case 3:		//MoviePlayer;
		{
			ActiveOnly(tMovie);
			te->Close();
			break;
		}
	default:	//unknown state
		{
		}
	}
	e_State = new_State;
}

void GameScene::e_tmpscript(CCArray* sl,int sum){
	if(e_State == 1 && m_bLoadProtect){
		return;
	}
	tScriptList = sl;
	t_ssum = sum;
	t_curscript = 0;
	e_update(1);
	e_act();
}

void GameScene::e_handlecurs(Script* s){
	int t_sum = s->getint("total");
	CCArray* acts = s->scriptnodes;
	//CCLOG("script handle.");
	for (int i = 0;i<t_sum;i++)		//multi here?
	{
		//CCLOG("handle scripte:%d",i);
		Script* tmp = (Script*) acts->objectAtIndex(i);//use tag to define node's having state
		switch(tmp->type)
		{
		case sShowText:
			{
				te->ShowText(tmp->getstring("content"),tmp->getstring("name"));
				static string slatst = "";
				SoundManager::sharedSoundManager()->StopSound(slatst.c_str());
				slatst = tmp->getstring("audio");
				SoundManager::sharedSoundManager()->PlaySound(slatst.c_str());
				break;
			}
		case sSelect:
			{
				te->DerSelMenu(tmp);
				break;
			}
		case sLoadImg:
			{
				te->DerLoadImg(tmp);
				break;
			}
		case sAction:
			{
				te->DerAction(tmp);
				break;
			}
		case sFinal:
			{
				te->DerFinal(tmp);
				break;
			}
		case sLock:
			{
				te->DerLock(tmp);
				break;
			}
		case sSilent:
			{
				te->DerSilent(tmp);
				break;
			}
		case sChange:
			{
				DerChange(tmp);
				break;
			}
		case sMapAction:
			{
				ml->tm->DerMapAct(tmp);
				break;
			}
		default:
			break;
		}
	}
}

void GameScene::addBackImg(){
	if(m_sBgi.length()<1) return;
	f_cachetest(m_sBgi.c_str());
	BgImg = CCSprite::create(m_sBgi.c_str());
	bg->addChild(BgImg);
}

void GameScene::DerChange(Script* s){
	int type = s->getint("type");
	switch(type){
	case 0:	//更换背景
		{
			if(BgImg) 
			{
				BgImg->removeFromParent();
				//CC_SAFE_RELEASE(BgImg);
				BgImg = NULL;
			}
			m_sBgi = s->getstring("content");
			addBackImg();
			BgImg->setAnchorPoint(CCPointZero);
			BgImg->setPosition(ccp(s->getfloat("x"), s->getfloat("y")));

			break;
		}
	case 1:  //清除背景
		{
			if(BgImg) 
			{
				BgImg->removeFromParent();
				m_sBgi.clear();
				BgImg = NULL;
			}
			break;
		}
	case 2:	//动态载入方案可以让ml载入一个空的tm来防止非法调用
		{
			ml->openTileMap(s->getstring("content"),s->getstring("script"));
			break;
		}
	case -2:
		{
			ml->openBattleMap(s->getstring("content"),0);
			break;
		}
	case 3:	//地图的载入方案>>1.初始化时统一载入并保存在ml中.2.运行时载入使用case2.考虑到相同地图使用不同脚本的情况，第一种可以设计分离，第二种实时性比较好。
		{
			if(BgImg){
				BgImg->removeFromParent();
				BgImg = NULL;
				
			}
			bg->setVisible(false);	
			ml->f_resumeall();			
			ml->f_resumecon();
			e_update(2);
			break;
		}
	case 4:	//停止地图一切活动，因为目前还没有关闭地图控制的策略，在tilemap中加入相关变量来控制，目前需要对ml的功能进行强化
		{
			ml->tm->pauseSchedulerAndActions();
			break;
		}
	case 5:	//播放视频
		{
			tMovie->set_media(s->getstring("content"));
			e_update(3);
			break;
		}
	case 6://test 
		{
			InfoTab* it = InfoTab::sharedInfoTab();
			it->showinfo(s->getstring("content"));
			
			break;
		}
	case 7:
		{
			//static bool test_lock = true;					//防止功能测试与测试脚本的冲突，test only。
			//if(test_lock){
				EffectControler::sharedEffectControler()->md_use_item(NULL,s->getint("id"));
/*				test_lock = false;
			}
	*/		
			break;
		}
	case 8://play bgm
		{
			SoundManager::sharedSoundManager()->PlayMusic(s->getstring("path"));
			break;
		}
	case 9:		// <cg的移动等放到这里
		{
			if(!BgImg) return;
			switch (s->getint("val"))
			{
			case 0:
				{
					m_flag = 0;
					float x = s->getfloat("x");
					float y = s->getfloat("y");
					float t = s->getfloat("t");
					BgImg->runAction(CCSequence::create(FRETAIN,CCMoveBy::create(t,ccp(x,y)),FRELEASE,NULL));
					break;
				}
			default:
				break;
			}
			break;
		}
	case 10: //Back
		{
			this->PreQuit();
			GameManager::sharedGameManager()->runSceneWithId(GameManager::SceneId::SCENE_MENU);
			
			break;
		}
	}
}

void GameScene::e_act(){
	if(!m_bCanSnap){
		m_bScrLock = true;
		scheduleUpdate();
		return;
	}
	las_state = e_State;
	switch(e_State){
	case 0:	//Txt脚本
		{
			if(e_curscript<m_IScriptSum){
				//CCLOG("cur 0 script:%d",e_curscript);
				Script* cur = (Script*) ScriptList->objectAtIndex(e_curscript);
				e_handlecurs(cur);
			}
			break;
		}
	case 1://地图脚本
		{
			//ml->tm->pauseSchedulerAndActions();				//TODO:改用脚本控制而不是使用统一的行为
			//ml->f_stopcontrol();
			if(t_curscript<t_ssum){
				Script* cur = (Script*) tScriptList->objectAtIndex(t_curscript);
				e_handlecurs(cur);
			}else{
				e_update(2);
			}
			break;
		}

	}

}

void GameScene::e_gonext(){
	//CCLOG("Go next being called!!!!!!");
	if(m_flag>0) return;
	switch(e_State){
	case 0:
		{
			e_curscript++;
			e_act();
			break;
		}
	case 1:
		{
			t_curscript++;
			e_act();
			break;
		}

	}

}

void GameScene::e_jump(int j){
	//CCLOG("e_jump being called!!!");
	switch(e_State){
	case 0:
		{
			if(j<0) {
				e_curscript++;
			}else{
				e_curscript = j;			
			}
			e_act();
			break;
		}
	case 1:
		{
			if(j<0) {
				t_curscript++;
			}else{
				t_curscript = j;			
			}
			e_act();
			break;
		}

	}
	//CCLOG("Jump!:%d",j);
}

// Effect Meta must control the update by them self.
void GameScene::FlagRelease(){
	--m_flag;
	if(m_flag<1) {
		e_gonext();
	}
	
}

void GameScene::FlagRetain(){
	++m_flag;
}
#include "ALSingle.h"

#include "cocos2d.h"
#include "packui/ConfigManager.h"
#include "pthread.h"
#include "GameManager.h"

USING_NS_CC;

ALSingle *ALSingle::mSharedALSingle = NULL;
map<string,ALuint> ALSingle::musicplayers;

ALSingle::ALSingle()
{
}


ALSingle::~ALSingle()
{
	
	KillALData();
	alDeleteSources(1,&bgm_player);
	CleanOgg();

	ConfigManager::purgeSharedConfigManager();
}


ALSingle *ALSingle::sharedALSingle()
{
	if (mSharedALSingle == NULL)
	{
		mSharedALSingle = new ALSingle();
		if (!mSharedALSingle || !mSharedALSingle->init())
		{
			CC_SAFE_DELETE(mSharedALSingle);
		}

	}

	return mSharedALSingle;

}


void ALSingle::purgeSharedALSingle()
{
	if(GameManager::sharedGameManager()->isActivate()) return;
	CC_SAFE_DELETE(mSharedALSingle);
}


bool ALSingle::init()
{
	bool bRet = false;
	do 
	{
		alutInit(NULL, 0);

		// 源声音的位置.  
		SourcePos[0] = 0.0;
		SourcePos[1] = 0.0;
		SourcePos[2] = 0.0;  

		// 源声音的速度.  
		SourceVel[0] = 0.0;
		SourceVel[1] = 0.0;
		SourceVel[2] = 0.0; 

		// 听者的位置.  
		ListenerPos[0] = 0.0;
		ListenerPos[1] = 0.0;
		ListenerPos[2] = 0.0;  

		// 听者的速度  
		ListenerVel[0] = 0.0;
		ListenerVel[1] = 0.0;
		ListenerVel[2] = 0.0;  

		// 听者的方向 (first 3 elements are "at", second 3 are "up")  
		ListenerOri[0] = 0.0;
		ListenerOri[1] = 0.0;
		ListenerOri[2] = -1;
		ListenerOri[3] = 0.0;
		ListenerOri[4] = 1;
		ListenerOri[5] = 0;  

		SetListenervalues();
		alGenSources(1 ,&bgm_player); 
		int ti;
		ConfigManager::sharedConfigManager()->GetConfigV(CM_BGMVOLEM,ti);
		f_gBgm = ti/100.0;

		ConfigManager::sharedConfigManager()->GetConfigV(CM_VIDIEOVOLEM,ti);
		f_gMusics = ti/100.0;

		ConfigManager::sharedConfigManager()->GetConfigV(CM_EFFECTVOLEM,ti);
		f_gEffect = ti/100.0;
		bgm_playing = false;
		movplay = NULL;

		bRet = true;

	} while (0);

	return bRet;
}

void ALSingle::preloadBackgroundMusic(const char* pszFilePath){
	OpenOgg(pszFilePath);
}

void ALSingle::preloadEffect(const char* pszFilePath){
	GetLoadedALBuffer(pszFilePath);
}

pthread_t up_id;
bool msBUpdate;
void *tUpdater(void *arg)
{
	while (msBUpdate) {
		pthread_testcancel();
		if(!GameManager::sharedGameManager()->isActivate()) break;
		ALSingle::sharedALSingle()->Update(0.1);
		Sleep(0);
	}
	return NULL;
}

void ALSingle::playBackgroundMusic(const char* pszFilePath, bool bLoop){
	if(!strcmp(bgpsz.c_str(),pszFilePath)) return;
	bgpsz = pszFilePath;
	Ogg_F tmp = oggs[pszFilePath];
	ALuint player = bgm_player;
	ALuint buffer[BUFFER_NUM];		
	
	stopBackgroundMusic();
	bgm_playing = true;

	alGenBuffers(BUFFER_NUM,buffer);	

	alSourcef (player, AL_PITCH, 1.0 );
	alSourcef (player, AL_GAIN, f_gBgm );
	alSourcefv(player, AL_POSITION, SourcePos);
	alSourcefv(player, AL_VELOCITY, SourceVel);
	alSourcei (player, AL_LOOPING, bLoop ); 

	char pData[BUFFER_SIZE];
	ALenum format;
	ALfloat freq;
	ALsizei bytelength;

	for (int iLoop = 0; iLoop < BUFFER_NUM; iLoop++)
	{
		if ((bytelength =ReadOgg(pszFilePath, pData, format, freq))>0)
		{
			alBufferData(buffer[iLoop], format, pData, bytelength, freq);
			alSourceQueueBuffers(player, 1, &buffer[iLoop]);
		}
	}
	tmp.idle = false;
	tmp.loop = bLoop;
	alSourcePlay(player);

	if(musicplayers.size() < 1) {			//初始化循环介入
		//sc->schedule(schedule_selector(ALSingle::Update),0.3);
		msBUpdate = true;
		pthread_create(&up_id, NULL, tUpdater, NULL);
	}
	return;
}

void ALSingle::stopBackgroundMusic(){
	if(!bgm_playing) return;
	bgm_playing = false;
	if(musicplayers.size() < 1){
		msBUpdate = false;
		pthread_cancel(up_id);
	}
	alSourceStop(bgm_player);

	int processed,queued;
	alGetSourcei(bgm_player, AL_BUFFERS_PROCESSED, &processed); 
	alGetSourcei(bgm_player, AL_BUFFERS_QUEUED, &queued); 
 
	while (processed--) 
	{ 
		ALuint  buffer; 
		alSourceUnqueueBuffers(bgm_player, 1, &buffer); 
		alDeleteBuffers(1, &buffer); 
	} 
	Ogg_F t = oggs[bgpsz];
	ov_raw_seek(t.oggFile,0);		// <重置

	alDeleteSources(1, &bgm_player);
	alGenSources(1, &bgm_player);
}


bool ALSingle::PlayOgg( const char* name,bool bloop /*= true*/ )
{
	Ogg_F tmp = oggs[name];
	if(!tmp.idle) return false;
	ALuint player;
	ALuint buffer[BUFFER_NUM];		

	alGenSources(1,&player);		
	alGenBuffers(BUFFER_NUM,buffer);
	alSourcef (player, AL_GAIN, f_gMusics );

	char pData[BUFFER_SIZE];
	ALenum format;
	ALfloat freq;
	ALsizei bytelength;

	for (int iLoop = 0; iLoop < BUFFER_NUM; iLoop++)
	{
		if ((bytelength =ReadOgg(name, pData, format, freq))>0)
		{
			alBufferData(buffer[iLoop], format, pData, bytelength, freq);
			alSourceQueueBuffers(player, 1, &buffer[iLoop]);
		}
	}
	tmp.idle = false;
	tmp.loop = bloop;
	alSourcePlay(player);
	musicplayers[name] = player;
	if(musicplayers.size() == 1 && !bgm_playing) {			//初始化循环介入
		msBUpdate = true;
		pthread_create(&up_id, NULL, tUpdater, NULL);
	}
	return true;
}


void ALSingle::StopUpdate()
{
	msBUpdate = false;
	int *tret;
	CCLog(">[ALSingle]StopUpdate() || begin");
	//GameManager::sharedGameManager()->Disable();
	pthread_cancel(up_id);
	pthread_join(up_id,(void**) &tret);
	CCLog(">[ALSingle]StopUpdate() || begin");
}

void ALSingle::StopEffect(const char* pszFilePath){
	ALuint t;
	t = EffectList[pszFilePath];
	if(t){ 
		//alSourcef (t, AL_GAIN, 0.1 );
		alSourceStop(t);
	} 
	EffectList.erase(pszFilePath);
}

void ALSingle::DownEffect(const char* pszFilePath){
	ALuint t;
	t = EffectList[pszFilePath];
	if(t){ 
		//alSourcef (t, AL_GAIN, 0.8 );
		//alSourceStop(t);
	} 
	EffectList.erase(pszFilePath);
}

void ALSingle::playEffect(const char* pszFilePath, ALfloat x, ALfloat y, ALfloat z, bool aloop){		// <播放Effect，默认行为：去除前一个相同音效并播放
	if(!CCFileUtils::sharedFileUtils()->isFileExist(pszFilePath)) return;

	ALint i;
	ALuint t;

	static ALfloat EfSourcePos[3];
	EfSourcePos[0] = x;
	EfSourcePos[1] = y;
	EfSourcePos[2] = z;  

	double theta = (double) (rand() % 360) * 3.14 / 180.0;

	EfSourcePos[0] = -float(cos(theta));
	EfSourcePos[1] = -float(rand()%2);
	EfSourcePos[2] = -float(sin(theta));

	bool iner = false;
	int c = 0;
	//[0803]CCLog(">[ALMIXBUG]:Begin Play...|%s_pos:%f,%f,%f.",pszFilePath,EfSourcePos[0],EfSourcePos[1],EfSourcePos[2]);
	t = EffectList[pszFilePath];
	CCLog(">[ALMIXBUG]:Tring to get buffered source...|%d",t);
	if(!t)
	{
		CCLog(">[ALSINGLE] Not In Group...");
		for (vector<ALuint>::iterator iter = Sources.begin(); iter != Sources.end(); ++iter)
		{
			c++;
			ALuint tmp = *iter;
			alGetSourcei(tmp,AL_SOURCE_STATE,&i);
			if(i == AL_STOPPED) {
				iner = true;
				t = tmp;
				CCLog(">[ALMIXBUG]:Find a empty source...|%d",t);
				break;
			}
		}

		if(c>NUM_EFFECT) {
			t = Sources[0];
			alSourceStop(t);
			CCLog(">[ALMIXBUG]:No empty,get the first one and stop it...|%d",t);
		}
		if(!iner){
			alGenSources(1,&t);
			Sources.push_back(t);
			CCLog(">[ALMIXBUG]:Generate a new source,empty...|%d",t);
		}

		
	}

	//alDeleteSources(1,&t);
	//alGenSources(1,&t);
	
	ALuint buffer = GetLoadedALBuffer(pszFilePath);
	CCLOG(">[ALMIXBUG]:Ready for play...|%d,buf:%d",t,buffer);

	alSourcei (t, AL_BUFFER, buffer );
	alSourcef (t, AL_PITCH, 1.0 );
	alSourcef (t, AL_GAIN, f_gEffect );
	alSourcefv(t, AL_POSITION, EfSourcePos);
	alSourcefv(t, AL_VELOCITY, SourceVel);
	alSourcei (t, AL_LOOPING, aloop ); 

	
	string moto = ListedEffect[t];
	if(moto.compare(pszFilePath)) EffectList.erase(moto);
	EffectList[pszFilePath] = t;
	ListedEffect[t] = pszFilePath;
	alSourcePlay(t);

}

string ALSingle::GetALErrorString(ALenum err)
{
switch(err)
{
case AL_NO_ERROR:
	return string("AL_NO_ERROR");
	break; 


case AL_INVALID_NAME:
	return string("AL_INVALID_NAME");
	break; 


case AL_INVALID_ENUM:
	return string("AL_INVALID_ENUM");
	break; 


case AL_INVALID_VALUE:
	return string("AL_INVALID_value");
	break; 


case AL_INVALID_OPERATION:
	return string("AL_INVALID_OPERATION");
	break; 


case AL_OUT_OF_MEMORY:
	return string("AL_OUT_OF_MEMORY");
	break;
default:
	return string("UNKnown");
	break;
};
}

bool ALSingle::QueryEffect( const char* pszFilePath )
{
	ALuint t;		//src
	ALint i;		//state

	t = EffectList[pszFilePath];
	if(t){
		alGetSourcei(t,AL_SOURCE_STATE,&i);
		return i == AL_STOPPED;
	}else{
		//[0803]CCLog(">[ALSingle]QueryEffect get an incorect string from soundmanager:%s",pszFilePath);
		return true;
	}
}


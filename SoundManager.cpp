#include "cocos2d.h"
//#include "SimpleAudioEngine.h"

#include "GameManager.h"
#include "SoundManager.h"

#include "utils/ALSingle.h"

#define PCM_HOVER "sound/pcm/hover.wav"
#define PCM_DECIDE "sound/pcm/decide.wav"
#define PCM_DISABL "sound/pcm/disable.ogg"

SoundManager *SoundManager::mSharedSoundManager = NULL;


SoundManager *SoundManager::sharedSoundManager()
{
	if (mSharedSoundManager == NULL)
	{
		mSharedSoundManager = new SoundManager();
		if (!mSharedSoundManager || !mSharedSoundManager->init())
		{
			CC_SAFE_DELETE(mSharedSoundManager);
		}

	}

	return mSharedSoundManager;

}


void SoundManager::purgeSharedSoundManager()
{
    CC_SAFE_DELETE(mSharedSoundManager);
	ALSingle::purgeSharedALSingle();

}


bool SoundManager::init()
{
	bool bRet = false;
    do 
    {
		ALSingle *audioEngine = ALSingle::sharedALSingle();
		//audioEngine->OpenOgg(CCFileUtils::sharedFileUtils()->fullPathForFilename("1.ogg"));

		audioEngine->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_HOVER).c_str());
		audioEngine->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_DECIDE).c_str());

        bRet = true;

    } while (0);

    return bRet;
}

void SoundManager::update(float dt){
	CCLog(">[SoundManager] update()");
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	//audioEngine->Update(dt);
}

SoundManager::SoundManager()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
}


SoundManager::~SoundManager()
{
	ALSingle::purgeSharedALSingle();
}                              

void SoundManager::PreLoadSrc(const char* pszFile){
	CCLOG(">Sound Manager Preload:%s",pszFile);
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->preloadBackgroundMusic(CCFileUtils::sharedFileUtils()->fullPathForFilename(pszFile).c_str());
}

void SoundManager::PlayMusic(const char* pszFile)
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playBackgroundMusic(CCFileUtils::sharedFileUtils()->fullPathForFilename(pszFile).c_str());
}
	

void SoundManager::StopMusic()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->stopBackgroundMusic();

}


void SoundManager::PlayDecideSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->DownEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_DECIDE).c_str());
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_DECIDE).c_str());

}
	

void SoundManager::PlayHoverSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->DownEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_HOVER).c_str());
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_HOVER).c_str());
}

void SoundManager::PlayDisablSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->DownEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_DISABL).c_str());
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(PCM_DISABL).c_str());
}

void SoundManager::PlayEffect(const char* path, float x, float y, float z, bool aloop){
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str(),  x,  y, z, aloop);
}

void SoundManager::PlayCVSound( const char* path, float x, float y, float z, bool aloop)
{
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str(),  x,  y, z, aloop);
	m_sLast = CCFileUtils::sharedFileUtils()->fullPathForFilename(path);
}

void SoundManager::StopCVSound(const char* path){
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->StopEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str());
}

bool SoundManager::QueryCVSound()
{
	return ALSingle::sharedALSingle()->QueryEffect(m_sLast.c_str());
}

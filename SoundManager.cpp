#include "cocos2d.h"
//#include "SimpleAudioEngine.h"

#include "GameManager.h"
#include "SoundManager.h"

#include "utils/ALSingle.h"



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

		audioEngine->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("sound/laser.wav").c_str());
		audioEngine->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("sound/hit.wav").c_str());

        bRet = true;

    } while (0);

    return bRet;
}

void SoundManager::update(float dt){
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	//audioEngine->Update(dt);
}

SoundManager::SoundManager()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
}


SoundManager::~SoundManager()
{
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


void SoundManager::PlayLaserSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("sound/laser.wav").c_str());

}
	

void SoundManager::PlayHitSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename("sound/hit.wav").c_str());

}

void SoundManager::PlayEffect(const char* path, float x, float y, float z, bool aloop){
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str(),  x,  y, z, aloop);
}

void SoundManager::PlaySound( const char* path, float x, float y, float z, bool aloop)
{
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str(),  x,  y, z, aloop);
	m_sLast = CCFileUtils::sharedFileUtils()->fullPathForFilename(path);
}

void SoundManager::StopSound(const char* path){
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->StopEffect(CCFileUtils::sharedFileUtils()->fullPathForFilename(path).c_str());
}

bool SoundManager::QuerySound()
{
	return ALSingle::sharedALSingle()->QueryEffect(m_sLast.c_str());
}

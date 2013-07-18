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
		//audioEngine->OpenOgg(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("1.ogg"));

		audioEngine->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("sound/laser.wav"));
		audioEngine->preloadEffect(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("sound/hit.wav"));

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
	audioEngine->preloadBackgroundMusic(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pszFile));
}

void SoundManager::PlayMusic(const char* pszFile)
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playBackgroundMusic(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(pszFile));

}
	

void SoundManager::StopMusic()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->stopBackgroundMusic();

}


void SoundManager::PlayLaserSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("sound/laser.wav"));

}
	

void SoundManager::PlayHitSFX()
{
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath("sound/hit.wav"));

}

void SoundManager::PlaySound( const char* path )
{
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->playEffect(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(path));
}

void SoundManager::StopSound(const char* path){
	if(strlen(path)<1) return;
	ALSingle *audioEngine = ALSingle::sharedALSingle();
	audioEngine->StopEffect(CCFileUtils::sharedFileUtils()->fullPathFromRelativePath(path));
}
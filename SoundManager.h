#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include "cocos2d.h"
USING_NS_CC;

class SoundManager : public CCNode
{
public:
	static SoundManager *sharedSoundManager();
	static void purgeSharedSoundManager();

	void PlayMusic(const char* pszFile);
	void StopMusic();
	void PreLoadSrc(const char* pszFile);

	void PlayLaserSFX();
	void PlayHitSFX();
	void update(float dt);

	//For CV voice only. Never call it for other use.
	void PlaySound(const char* path);
	void StopSound(const char* path);
	bool QuerySound();		//Return true if last cv sound is stopped.

	std::string m_sLast;

protected:
	SoundManager();
	~SoundManager();

	bool init();

private:
	static SoundManager *mSharedSoundManager;

};


#endif


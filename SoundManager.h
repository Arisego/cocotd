#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include "cocos2d.h"
USING_NS_CC;

class SoundManager : public CCNode
{
public:
	static SoundManager *sharedSoundManager();
	static void purgeSharedSoundManager();

	void update(float dt);

	/* <背景音乐类	| OGG */
	void PlayMusic(const char* pszFile);		/* <播放指定的背景音乐 */
	void StopMusic();							/* <一次只有一个背景音乐 */
	void PreLoadSrc(const char* pszFile);		/* <由于已经使用了流机制，预读缓存不再被使用。 */

	/* <SE播放类	| WAV OGG */
	void PlayDecideSFX();
	void PlayHoverSFX();
	void PlayDisablSFX();
	void PlayEffect(const char* path, float x = 0, float y = 0, float z = 0, bool aloop = false);
	
	/*  <播放语音	| OGG */
	void PlayCVSound(const char* path, float x = 0, float y = 0, float z = 0, bool aloop = false);	/* <播放指定的语音 */
	void StopCVSound(const char* path);																/* <文本自动前进时会查询 */
	bool QueryCVSound();																			/* <查询语音是否播放完毕 */

	std::string m_sLast;

protected:
	SoundManager();
	~SoundManager();

	bool init();

private:
	static SoundManager *mSharedSoundManager;

};


#endif


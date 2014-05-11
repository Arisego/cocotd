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

	/* <����������	| OGG */
	void PlayMusic(const char* pszFile);		/* <����ָ���ı������� */
	void StopMusic();							/* <һ��ֻ��һ���������� */
	void PreLoadSrc(const char* pszFile);		/* <�����Ѿ�ʹ���������ƣ�Ԥ�����治�ٱ�ʹ�á� */

	/* <SE������	| WAV OGG */
	void PlayDecideSFX();
	void PlayHoverSFX();
	void PlayDisablSFX();
	void PlayEffect(const char* path, float x = 0, float y = 0, float z = 0, bool aloop = false);
	
	/*  <��������	| OGG */
	void PlayCVSound(const char* path, float x = 0, float y = 0, float z = 0, bool aloop = false);	/* <����ָ�������� */
	void StopCVSound(const char* path);																/* <�ı��Զ�ǰ��ʱ���ѯ */
	bool QueryCVSound();																			/* <��ѯ�����Ƿ񲥷���� */

	std::string m_sLast;

protected:
	SoundManager();
	~SoundManager();

	bool init();

private:
	static SoundManager *mSharedSoundManager;

};


#endif


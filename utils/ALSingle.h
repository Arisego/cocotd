#ifndef __AL_SINGLE_H__
#define __AL_SINGLE_H__

#include <conio.h>
#include <stdlib.h>
#include <time.h>

#include "al/al.h"
#include "al/alc.h"
#include "al/alut.h"

#include "vorbis/vorbisfile.h"  
#include "utils/MoviePlayer.h"

#include <vector>
#include <map>

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

#define NUM_EFFECT		20			//用于播放effect的源的个数
#define BUFFER_SIZE     10240       // 
#define BUFFER_NUM		50			//根据具体的update的频率来决定一个性能上最好的值，最好是1s一次！！


class ALSingle
{
public:
	CCScene* sc;

	static ALSingle *sharedALSingle();
	static void purgeSharedALSingle();

	ALfloat SourcePos[3];   
	ALfloat SourceVel[3];  
	ALfloat ListenerPos[3];   
	ALfloat ListenerVel[3];  
	ALfloat ListenerOri[6]; 

	void preloadBackgroundMusic(const char* pszFilePath);
	void preloadEffect(const char* pszFilePath);

	ALuint bgm_player;
	bool bgm_playing;
	
	void playBackgroundMusic(const char* pszFilePath, bool bLoop);
	void playBackgroundMusic(const char* pszFilePath) {
		this->playBackgroundMusic(pszFilePath, false);
	}

	void stopBackgroundMusic();
	void playEffect(const char* pszFilePath);
	void StopEffect(const char* pszFilePath);

	void SetListenervalues()
	{
		alListenerfv(AL_POSITION, ListenerPos);
		alListenerfv(AL_VELOCITY, ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	} 


	void KillALData()
	{
		KillALLoadedData();
		for (vector<ALuint>::iterator iter = Buffers.begin(); iter != Buffers.end(); ++iter)
		{
			ALuint t = *iter;
			alDeleteBuffers(1, &t); 
		}
		for (vector<ALuint>::iterator iter = Sources.begin(); iter != Sources.end(); ++iter)
		{
			ALuint t = *iter;
			alDeleteBuffers(1, &t); 
		}

		Buffers.clear();
		Sources.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	struct Ogg_F{
		vorbis_info *pInfo;  
		OggVorbis_File *oggFile;
		bool idle;
		bool loop;
	};

	class Player{

	};

	map<string,Ogg_F> oggs;
	static map<string,ALuint> musicplayers;
	string bgpsz;
	
	void CleanOgg(){			//
		

		map<string,Ogg_F>::iterator bt = oggs.begin();
		for(;bt!=oggs.end();++bt){
			ov_clear(bt->second.oggFile);
			delete bt->second.oggFile;
			//free(bt->second.pInfo);
			//delete bt->second.pInfo;
		}
		oggs.clear();

		//map<string,ALuint>::iterator it=musicplayers.begin();
		//for(;it!=musicplayers.end();++it){
		//	ALuint t = it->second;
		//	alDeleteBuffers(1, &t);
		//}
		//musicplayers.clear();
	}

	/* 开始播放 */
	bool PlayOgg(const char* name,bool bloop = true){
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
			sc->schedule(schedule_selector(ALSingle::Update),0.3);
		}
		return true;
	}

	void StopUpdate(){		
		if(musicplayers.size()<1 && !bgm_playing) return;				//StopAll 必须清除过
		sc->unschedule(schedule_selector(ALSingle::Update));
	}

	/*停止ogg播放*/
	void StopOgg(const char* name){
		ALuint d = musicplayers[name];
		musicplayers.erase(name);
		alSourceStop(d);
	}

	/*暂停ogg播放*/
	void PauseOgg(const char* name){
		ALuint d = musicplayers[name];
		alSourcePause(d);
	}

	/*恢复ogg播放*/
	void ResumeOgg(const char* name){
		ALuint d = musicplayers[name];
		alSourcePlay(d);
	}

	/* 更新缓存 */
	bool UpdateOgg(const char* name,ALuint uiSource){
		
		ALint	iBuffersProcessed, iQueuedBuffers;
		ALuint	uiBuffer;
		char pData[BUFFER_SIZE];
		

		iBuffersProcessed = 0;
		alGetSourcei(uiSource, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
		while (iBuffersProcessed)
		{
			uiBuffer = 0;
			

			ALenum format; 
			ALfloat freq;
			long ulBytesWritten = ReadOgg(name,pData,format,freq);
			if (ulBytesWritten>0)
			{
				alSourceUnqueueBuffers(uiSource, 1, &uiBuffer);
				alBufferData(uiBuffer, format, pData, ulBytesWritten, freq);
				alSourceQueueBuffers(uiSource, 1, &uiBuffer);
				iBuffersProcessed--;
			}else if(!ulBytesWritten){
				Ogg_F t = oggs[name];
				ov_raw_seek(t.oggFile,0);		//重置
				if(t.loop){
					continue;
				}else{							//缓存载入完毕则不用再update
					t.idle = true;
					alDeleteSources(1,&uiSource);
					musicplayers.erase(name);
					if(musicplayers.size()<1) StopUpdate();
					break;
				}


			}else{
				exit(1);	//AT:跳出注意，ogg文件有误。
			}
			
		}

		ALint	iState;
		alGetSourcei(uiSource, AL_SOURCE_STATE, &iState);
		if (iState == AL_STOPPED)
		{
			// If there are Buffers in the Source Queue then the Source was starved of audio
			// data, so needs to be restarted (because there is more audio data to play)
			alGetSourcei(uiSource, AL_BUFFERS_QUEUED, &iQueuedBuffers);
			if (iQueuedBuffers)
			{
				alSourcePlay(uiSource);
			}
			else
			{
				oggs[name].idle = true;
				musicplayers.erase(name);
				return false;
			}
		}
		return true;
	}

	/* 将ogg缓存至oggs */
	bool OpenOgg(const char *name){
		Ogg_F ogg;
		FILE *f;
		f= NULL;

		f =  fopen(name, "rb");  
		if (f == NULL)  
			return false;   

		//vorbis_info *pInfo;  
		OggVorbis_File *oggFile = new OggVorbis_File();  

		// Try opening the given file  
		int err = 0;
		if ((err = ov_open(f, oggFile, NULL, 0)) != 0)  
			return false;   

		//pInfo = ov_info(oggFile, -1);  

		ogg.oggFile = oggFile;
		ogg.pInfo = ov_info(oggFile, -1);
		ogg.idle = true;
		ogg.loop = true;	//loop是 默认行为

		oggs[name] = ogg;
		return true;
	}

	/* 读取内存 */ 
	long ReadOgg(const char *name, char* data, ALenum &format, ALfloat &freq){
		int endian = 0;                         // 0 for Little-Endian, 1 for Big-Endian  
		int bitStream;  
		long bytes;  
		//char pData[BUFFER_SIZE];
		Ogg_F tmp = oggs[name];
		if(tmp.oggFile == NULL){
			OpenOgg(name);
			tmp = oggs[name];
			CCLOG(">>>>PS:Read Ogg Not Buffered. Check the script.");
		}


		vorbis_info *pInfo;  
		OggVorbis_File *oggFile;  

		pInfo = tmp.pInfo;  
		oggFile = tmp.oggFile;

		if (pInfo->channels == 1)  
			format = AL_FORMAT_MONO16;  
		else  
			format = AL_FORMAT_STEREO16;  

		freq = pInfo->rate;  
		
		bytes = ov_read(oggFile, data, BUFFER_SIZE, endian, 2, 1, &bitStream);
		//bytes = ov_read(&oggFile, pData, BUFFER_SIZE, endian, 2, 1, &bitStream);
		if (bytes < 0)  
		{  
			ov_clear(oggFile);  
			exit(-1);  
		}  
		return bytes;
	}

	void Update_2(){
		if(bgm_playing){
			UpdateOgg(bgpsz.c_str(),bgm_player);
		}
		if(musicplayers.size() < 1) return;
		map<string,ALuint>::iterator it=musicplayers.begin();
		for(;it!=musicplayers.end();++it)
			UpdateOgg((it->first).c_str(),it->second);
	}

	void Update(float dt){
		sharedALSingle()->Update_2();
	}
	
	//////////////////////////////////////////////////////////////////////////
	// EffectList 使用
	bool LoadOGG(const char *name, vector<char> &buffer, ALenum &format, ALfloat &freq)  
	{  
		int endian = 0;                         // 0 for Little-Endian, 1 for Big-Endian  
		int bitStream;  
		long bytes;  
		char array[BUFFER_SIZE];                // Local fixed size array  
		FILE *f;  

		f= NULL;

		f = fopen(name, "rb");  

		if (f == NULL)  
			return false;   

		vorbis_info *pInfo;  
		OggVorbis_File oggFile;  

		// Try opening the given file  
		int err = 0;
		if ((err = ov_open(f, &oggFile, NULL, 0)) != 0)  
			return false;   

		pInfo = ov_info(&oggFile, -1);  

		if (pInfo->channels == 1)  
			format = AL_FORMAT_MONO16;  
		else  
			format = AL_FORMAT_STEREO16;  

		freq = pInfo->rate;  

		do  
		{   
			bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);  

			if (bytes < 0)  
			{  
				ov_clear(&oggFile);  
				//cerr << "Error decoding " << name << "..." << endl;  
				exit(-1);  
			}  

			buffer.insert(buffer.end(), array, array + bytes);  
		}  
		while (bytes > 0);  


		
		ov_clear(&oggFile);  
		return true;   
	}  

	//////////////////////////////////////////////////////////////////////////
	// 音量Refresh
	//	bgm_player msicpalyers[] //EffectList由于太短将不做音量刷新
	float f_gBgm, f_gMusics, f_gEffect;
	MoviePlayer* movplay;	//Moive的音量交给ALSingle管理

	void SetMov(MoviePlayer* mp){
		movplay = mp;
	}

	void RefreshBgmV(float df){
		if(df == f_gBgm) return;
		f_gBgm = df;
		alSourcef(bgm_player,AL_GAIN,f_gBgm);
	}

	void RefreshPlayer(float df){
		if(df == f_gMusics) return;
		if(movplay) movplay->setvolume(df*100);
		f_gMusics = df;
		
		map<string,ALuint>::iterator it=musicplayers.begin();
		for(;it!=musicplayers.end();++it){
			alSourcef(it->second, AL_GAIN, f_gMusics);
		}
	}

	void RefreshEffect(float df){
		if(df == f_gEffect) return;
		f_gEffect = df;
		//不做刷新。
	}

	/* 停止所有正在播放的音乐并释放必要的sources。 */
	void StopAll(){
		//ALuint uibuffer;
		StopUpdate();

		for (vector<ALuint>::iterator iter = Sources.begin(); iter != Sources.end(); ++iter)
		{
			ALuint tmp = *iter;
			alDeleteSources(1,&tmp);
		}
		Sources.clear();
		EffectList.clear();
		ListedEffect.clear();

		if(bgm_playing){
			alSourceStop(bgm_player);
			ov_raw_seek(oggs[bgpsz].oggFile,0);
			alDeleteSources(1,&bgm_player);
			alGenSources(1,&bgm_player);
			//alSourceUnqueueBuffers(bgm_player,BUFFER_NUM,&uibuffer);
		}


		map<string,ALuint>::iterator it=musicplayers.begin();
		for(;it!=musicplayers.end();++it){
			ov_raw_seek(oggs[it->first].oggFile,0);
			alSourceStop(it->second);
			alDeleteSources(1,&(it->second));
			//alSourceUnqueueBuffers(it->second,BUFFER_NUM,&uibuffer);
		}
		musicplayers.clear();
		//oggs.clear();

		bgm_playing = false;
		bgpsz.clear();
	}


protected:
	ALSingle();
	~ALSingle();

	bool init();

private:
	static ALSingle *mSharedALSingle;

	vector<string> LoadedFiles; // 文件路径
	vector<ALuint> Buffers; // 缓冲区.
	vector<ALuint> Sources; // 源. 
	map<string,ALuint> EffectList;//正在播放的Effecct对应表
	map<ALuint,string> ListedEffect;

	string GetALErrorString(ALenum err);

	ALuint LoadALBuffer(string path)
	{
		// Variables to store data which defines the buffer.
		ALenum format;
		ALsizei size;
		ALvoid* data;
		ALfloat freq;

		vector<char> bufferData;                // The sound buffer data from file 

		// 缓冲区ID和错误检测变量
		ALuint buffer;
		ALenum result,l; 
		l = alGetError();
		alGenBuffers(1, &buffer); 
		if ((result = alGetError()) != AL_NO_ERROR)
			throw GetALErrorString(result); 

		data = alutLoadMemoryFromFile ((ALbyte*) path.c_str(), &format, &size, &freq);		
		if ((result = alGetError()) != AL_NO_ERROR)
			GetALErrorString(result); 


		if(!data){
			LoadOGG(path.c_str(),bufferData,format,freq);
			alBufferData(buffer, format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), freq);

		}else{
			alBufferData(buffer, format, data, size, freq); 
		}

		
		if ((result = alGetError()) != AL_NO_ERROR)
			throw GetALErrorString(result); 


		//do { if(data) { free(data); (data) = 0; } } while(0);


		//返回缓冲区ID
		return buffer;
	} 

	ALuint GetLoadedALBuffer(string path)
	{
		int count = 0; 
		ALuint buffer; // 用于导入缓冲区的缓冲区ID 

		for(vector<string>::iterator iter = LoadedFiles.begin(); iter != LoadedFiles.end(); ++iter, count++)
		{
			if(!(*iter).compare(path))
				return Buffers[count];
		} 
		buffer = LoadALBuffer(path); 


		// 添加缓冲区到列表，记录他已添加.
		Buffers.push_back(buffer); 
		LoadedFiles.push_back(path); 


		return buffer;
	} 

	ALuint LoadALSample(string path, bool loop)
	{
		ALuint source;
		ALuint buffer;
		ALenum result; 

		// 得到文件缓冲区ID 
		buffer = GetLoadedALBuffer(path); 
		alGenSources(1 ,&source); 


		if ((result = alGetError()) != AL_NO_ERROR)
			throw GetALErrorString(result); 

		alSourcei (source, AL_BUFFER, buffer );
		alSourcef (source, AL_PITCH, 1.0 );
		alSourcef (source, AL_GAIN, 1.0 );
		alSourcefv(source, AL_POSITION, SourcePos);
		alSourcefv(source, AL_VELOCITY, SourceVel);
		alSourcei (source, AL_LOOPING, loop ); 

		Sources.push_back(source); 
		return source;
	}
public:
	void KillALLoadedData()
	{
		LoadedFiles.clear();
		EffectList.clear();
		ListedEffect.clear();
	}

};

#endif
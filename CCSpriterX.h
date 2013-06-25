//------------------------------------------------------------------------
//
//	SCMLAnimator : KickStarter project Spriter renderer for cocos2d-x.
//
//	Spriter website : http://www.kickstarter.com/projects/539087245/spriter
//
//	Licensed under the BSD license, see LICENSE in root for details.
// 
//	Copyright (c) 2012 James Hui (a.k.a. Dr.Watson)
// 
//	For latest updates, please visit http://jameshui.com
//
//------------------------------------------------------------------------


#ifndef _CC_SPRITER_X_H_
#define _CC_SPRITER_X_H_

#include <vector>
#include <string>

#include "cocos2d.h"

#include "tinyxml/tinyxml.h"

using namespace std;

class CCSpriterX;

namespace SCMLHelper
{
	struct File
	{
		File();
		~File();

		void Init(TiXmlNode *node);

		int id;
		std::string name;
		float width;
		float height;

		cocos2d::CCSprite *sprite;

	};

	class Folder
	{
	public:
		Folder();
		~Folder();
		
		void Init(TiXmlNode *node);
		
		int GetFileCount();
		File *GetFile(int index);

	private:
		int mId;
		std::string mName;

		std::vector <File *> mFiles;

	};

	struct ObjectRef
	{
		void Init(TiXmlNode *node);

		int id;
		int timeline;
		int key;
		int z_index;
	};

	struct Object
	{
		void Init(TiXmlNode *node, CCSpriterX *animator);

		int folder;
		int file;
		float x;
		float y;
		float angle;
		float pivot_x;
		float pivot_y;
		int z_index;

		cocos2d::CCSprite *sprite;
		CCSpriterX *animat;

	};

	class Key
	{
	public:
		Key();
		~Key();
		
		void Init(TiXmlNode *node, CCSpriterX *animator);
		
		int GetObjectRefCount();
		ObjectRef *GetObjectRef(int index);
		
		int GetObjectCount();
		Object *GetObject(int index);

		float GetTime();

		bool IsSpinCounterClockwise();

	private:
		int mId;
		float mTime;
		bool mSpinCounterClockwise;

		std::vector <Object *> mObjects;
		std::vector <ObjectRef *> mObjectRefs;
		// will have bones later

	};

	class Timeline
	{
	public:
		Timeline();
		~Timeline();

		void Init(TiXmlNode *node, CCSpriterX *animator);
		int GetKeyframeCount();
		Key *GetKeyframe(int index);

	private:
		int mId;
		std::vector <Key *> mKeyframes;
	};


	class Animation
	{
	public:
		Animation();
		~Animation();

		void Update(float dt);
		void Init(TiXmlNode *node, CCSpriterX *animator);

		//void Render();

		bool IsDone();

		void Restart();
		CCSpriterX *animato;
		std::string mName;
	private:
		int mId;
		
		float mLength;
		bool mLooping;
		bool mDone;

		Timeline *mMainline;
		int mCurrKeyframe;

		std::vector <Timeline *> mTimelines;

		float mTimer;

		cocos2d::CCPoint mPosition;

	};


	class Entity
	{
	public:
		Entity();
		~Entity();
		
		void Update(float dt);
		//void Render();

		void SetId(int id);
		void SetName(const char *name);
		void AddAnimation(Animation *animation);

		void NextAnimation();
		void PlayTarget(const char *name);


	private:
		
		int mId;
		std::string mName;

		std::vector <Animation *> mAnimations;
		std::map<std::string,int> mAniNames;

		int mCurrAnimation;

	};

}


class CCSpriterX : public cocos2d::CCSprite
{
public:
	CCSpriterX();
	~CCSpriterX();

	bool initWithFile(const char *filename);

	//virtual void draw(void);
	virtual void update(float dt);
		
	static CCSpriterX* create(const char *filename);

	cocos2d::CCSprite *getSprite(int folderId, int fileId);	//TODO:tricks aruond here

	void PlayNext();		//OVER:使用name来调用动画
	void PlayAnim(const char* name, int aiTimes = -1,const char* alast = "");
	void PlayLast();
	
	string msLast;
	string msCur;
	int miPlayTimes;
private:

	std::vector <SCMLHelper::Folder *> mFolders;
	std::vector <SCMLHelper::Entity *> mEntities;

	int mCurrEntity;

};



#endif

//------------------------------------------------------------------------
//
//	CCSpriterX : KickStarter project Spriter renderer for cocos2d-x.
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

#include "CCSpriterX.h"

USING_NS_CC;


namespace SCMLHelper
{

	///////////////////////////////////////////////////////////////////////////////////

	File::File()
		: sprite(NULL)
	{
	}


	File::~File()
	{
		if (sprite)
			sprite->release();

	}

	//TODO:添加函数通过配置修改sprite，达到局部更换的特效

	void File::Init(TiXmlNode *node)
	{
		TiXmlElement *element = node->ToElement();
		if (element)
		{
			int intValue;
			float floatValue;

			if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
				id = intValue;
			else
				id = 0;

			name = element->Attribute("name");

			if (element->QueryFloatAttribute("width", &floatValue) == TIXML_SUCCESS)
				width = floatValue;
			else
				width = 0;

			if (element->QueryFloatAttribute("height", &floatValue) == TIXML_SUCCESS)
				height = floatValue;
			else
				height = 0;
			CCLog("[Spx_file_init]%s",name.c_str());
			if (name.size()>0)
			{
				int i = name.find_last_of("/");
				name = name.substr(i+1,name.length());									//TODO:修改scml或者plist中的一个
				sprite = CCSprite::createWithSpriteFrameName(name.c_str());				//FN:读取sprite
				if(!sprite) CCLog("[Spx_file_init]%s",name.c_str());
				sprite->retain();
			}

		}

	}

	///////////////////////////////////////////////////////////////////////////////////

	Folder::Folder()
		: mId(0)
	{ 
		mFiles.reserve(50); 
	}


	Folder::~Folder()
	{
		int count = mFiles.size();
		for (int i=0;i<count;i++)
			CC_SAFE_DELETE(mFiles[i]);

		mFiles.clear();

	}


	int Folder::GetFileCount()
	{
		return mFiles.size();

	}

	
	File *Folder::GetFile(int index)
	{
		if (index < (int)mFiles.size())
			return mFiles[index];

		return NULL;

	}


	void Folder::Init(TiXmlNode *node)
	{
		TiXmlElement *element = node->ToElement();
		if (element)
		{
			int intValue;

			if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
				mId= intValue;


			mName = element->Attribute("name");

			for (TiXmlNode* fileNode = node->FirstChild(); fileNode; fileNode = fileNode->NextSibling())
			{
				File *file = new File();
				file->Init(fileNode);

				mFiles.push_back(file);
			}

		}

	}

	///////////////////////////////////////////////////////////////////////////////////

	void ObjectRef::Init(TiXmlNode *node)
	{
		TiXmlElement *element = node->ToElement();
		if (element)
		{
			int intValue;

			if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
				id = intValue;
			else
				id = 0;

			if (element->QueryIntAttribute("timeline", &intValue) == TIXML_SUCCESS)
				timeline = intValue;
			else
				timeline = 0;

			if (element->QueryIntAttribute("key", &intValue) == TIXML_SUCCESS)
				key = intValue;
			else
				key = 0;

			if (element->QueryIntAttribute("z_index", &intValue) == TIXML_SUCCESS)
				z_index = intValue;
			else
				z_index = 0;

		}
	}


	///////////////////////////////////////////////////////////////////////////////////

	void Object::Init(TiXmlNode *node, CCSpriterX *animator)
	{
		sprite = NULL;
		animat = animator;
        float scaleFactor = CCDirector::sharedDirector()->getContentScaleFactor();
        
		TiXmlElement *element = node->ToElement();
		if (element)
		{
			int intValue;
			float floatValue;

			if (element->QueryIntAttribute("folder", &intValue) == TIXML_SUCCESS)
				folder = intValue;
			else
				folder = 0;

			if (element->QueryIntAttribute("file", &intValue) == TIXML_SUCCESS)
				file = intValue;
			else
				file = 0;

			if (element->QueryFloatAttribute("x", &floatValue) == TIXML_SUCCESS)
				x = floatValue/scaleFactor;
			else
				x = 0;

			if (element->QueryFloatAttribute("y", &floatValue) == TIXML_SUCCESS)
				y = floatValue/scaleFactor;
			else
				y = 0;

			if (element->QueryFloatAttribute("angle", &floatValue) == TIXML_SUCCESS)
				angle = floatValue;
			else
				angle = 0;

			if (element->QueryFloatAttribute("pivot_x", &floatValue) == TIXML_SUCCESS)
				pivot_x = floatValue;
			else
				pivot_x = 0;

			if (element->QueryFloatAttribute("pivot_y", &floatValue) == TIXML_SUCCESS)
				pivot_y = floatValue;
			else
				pivot_y = 1;

			if (element->QueryIntAttribute("z_index", &intValue) == TIXML_SUCCESS)
				z_index = intValue;
			else
				z_index = 0;

			sprite = animator->getSprite(folder, file);
//			animator->sheet->addChild(sprite);

		}
	}


	///////////////////////////////////////////////////////////////////////////////////

	Key::Key()
		: mId(0)
		, mTime(0)
		, mSpinCounterClockwise(true)
	{ 
		mObjects.reserve(50);
		mObjectRefs.reserve(50); 
	}


	Key::~Key()
	{
		int count = mObjects.size();
		for (int i=0;i<count;i++)
			CC_SAFE_DELETE(mObjects[i]);

		mObjects.clear();

		count = mObjectRefs.size();
		for (int i=0;i<count;i++)
			CC_SAFE_DELETE(mObjectRefs[i]);

		mObjectRefs.clear();

	}


	int Key::GetObjectRefCount()
	{
		return mObjectRefs.size();

	}


	ObjectRef *Key::GetObjectRef(int index)
	{
		if (index < (int)mObjectRefs.size())
			return mObjectRefs[index];

		return NULL;

	}


	int Key::GetObjectCount()
	{
		return mObjects.size();

	}


	Object *Key::GetObject(int index)
	{
		if (index < (int)mObjects.size())
			return mObjects[index];

		return NULL;

	}


	float Key::GetTime()
	{
		return mTime;
	}


	bool Key::IsSpinCounterClockwise()
	{
		return mSpinCounterClockwise;

	}


	void Key::Init(TiXmlNode *node, CCSpriterX *animator)
	{
		TiXmlElement *element = node->ToElement();
		if (element)
		{
			int intValue;
			float floatValue;
			if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
				mId = intValue;

			float time = 0;
			if (element->QueryFloatAttribute("time", &floatValue) == TIXML_SUCCESS)		// was in milliseconds, convert to seconds instead
				time = floatValue/1000.0f;
			mTime = time;

			if (element->QueryIntAttribute("spin", &intValue) == TIXML_SUCCESS)
				mSpinCounterClockwise = !(intValue == -1);

			for (TiXmlNode* objNode = node->FirstChild(); objNode; objNode = objNode->NextSibling())
			{
				element = objNode->ToElement();
				const char *tabObj = element->Value();

				if (strcmp(tabObj, "object_ref")==0)
				{
					ObjectRef *ref = new ObjectRef();
					ref->Init(objNode);

					mObjectRefs.push_back(ref);

				}
				else if (strcmp(tabObj, "object")==0)
				{
					Object *obj = new Object();
					obj->Init(objNode, animator);

					mObjects.push_back(obj);
				}
			}
		}

	}


	///////////////////////////////////////////////////////////////////////////////////
	
	Timeline::Timeline()
		: mId(0)
	{
		mKeyframes.reserve(50);
	}


	Timeline::~Timeline()
	{
		int count = mKeyframes.size();
		for (int i=0;i<count;i++)
		{
			CC_SAFE_DELETE(mKeyframes[i]);

		}

	}


	int Timeline::GetKeyframeCount()
	{
		return mKeyframes.size();

	}


	Key *Timeline::GetKeyframe(int index)
	{
		if (index < (int)mKeyframes.size())
		{
			return mKeyframes[index];
		}

		return NULL;

	}

	void Timeline::Init(TiXmlNode *node, CCSpriterX *animator)
	{
		int intValue;

		TiXmlElement *element = node->ToElement();

		if (element)
		{
			if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
				mId = intValue;

			for (TiXmlNode* keyNode = node->FirstChild(); keyNode; keyNode = keyNode->NextSibling())
			{
				element = keyNode->ToElement();
				if (element)
				{
					Key *keyframe = new Key();

					keyframe->Init(keyNode, animator);

					mKeyframes.push_back(keyframe);
				}
			}

		}

	}

	///////////////////////////////////////////////////////////////////////////////////

	Animation::Animation()
		: mId(0)
		, mCurrKeyframe(0)
		, mMainline(NULL)
		, mDone(false)
		, mTimer(0)
	{
		mTimelines.reserve(50);

	}


	Animation::~Animation()
	{
		int count = mTimelines.size();
		for (int i=0;i<count;i++)
			CC_SAFE_DELETE(mTimelines[i]);

		mTimelines.clear();
		delete mMainline;

	}


	void Animation::Init(TiXmlNode *node, CCSpriterX *animator)
	{
		animato = animator;
		int intValue;
		float floatValue;


		TiXmlElement *element = node->ToElement();

		if (element)
		{
			if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
				mId = intValue;

			mName = element->Attribute("name");

			if (element->QueryFloatAttribute("length", &floatValue) == TIXML_SUCCESS)
				mLength = floatValue/1000.0f;							// was in milliseconds, convert to seconds instead

			const char *looping = element->Attribute("looping");		// was set to "false" in alpha, but in fact looping all the time
			mLooping = true;

			for (TiXmlNode* lineNode = node->FirstChild(); lineNode; lineNode = lineNode->NextSibling())
			{
				element = lineNode->ToElement();

				const char *tabLine = element->Value();
				if (strcmp(tabLine, "mainline")==0)						// 1 mainline only
				{
					mMainline = new Timeline();
					mMainline->Init(lineNode, animator);

				}
				else if (strcmp(tabLine, "timeline")==0)
				{
					Timeline *timeline = new Timeline();
					timeline->Init(lineNode, animator);

					mTimelines.push_back(timeline);

				}

			}
		}

	}


	bool Animation::IsDone()
	{
		return mDone;

	}


	void Animation::Restart()
	{
		mDone = false;
		mTimer = mfBegin;
		mCurrKeyframe = 0;

	}


	void Animation::Update(float dt)
	{

		animato->removeAllChildren();
		mTimer += dt;
		if (mTimer >= mfEnd)
		{
			mDone = true;
			////[0803]
			//[PLAYSTATEDEBUG]CCLog(">[spx]miPlayTimes:%s||>%s,%d",animato->msCur.c_str(),animato->msLast.c_str(),animato->miPlayTimes);
			if(animato->miPlayTimes < 0){
				Restart();
			}else if(animato->miPlayTimes == 0){
				if(!animato->mbNoLast){
					animato->PlayLast();
					return;
				}
			}else{
				--animato->miPlayTimes;
				Restart();			// always looping for now
			}
		}
        
		int count = mMainline->GetKeyframeCount();
		Key *keyframe = mMainline->GetKeyframe(mCurrKeyframe);
		float currTime = keyframe->GetTime();

		Key *keyframeNext = NULL;

		int next = mCurrKeyframe+1;

		if (next > count-1)		// looping
			next = 0;
	
		keyframeNext = mMainline->GetKeyframe(next);
		
		if (keyframeNext)
		{
			float nextTime = keyframeNext->GetTime();
			if (next == 0)
				nextTime = mLength;

			if (mTimer >= nextTime)
			{

				mCurrKeyframe = next;

				keyframe = keyframeNext;
				currTime = keyframe->GetTime();

				next = mCurrKeyframe+1;
				if (next > count-1)				// looping
					next = 0;
					
				keyframeNext = mMainline->GetKeyframe(next);
				if (keyframeNext == NULL)
					return;

				nextTime = keyframeNext->GetTime();
				if (next == 0)
					nextTime = mLength;
					
			}


			float t = (mTimer-currTime)/(nextTime-currTime);

			int count = keyframe->GetObjectRefCount();
			for (int i=0;i<count;i++)
			{
				ObjectRef *ref = keyframe->GetObjectRef(i);
			
				ObjectRef *refNext = keyframeNext->GetObjectRef(i);

				if (ref && refNext)
				{

					Key *keyRef = mTimelines[ref->timeline]->GetKeyframe(ref->key);
					Object *obj = keyRef->GetObject(0);									// should be only 1 object

					Key *keyRefNext = mTimelines[refNext->timeline]->GetKeyframe(refNext->key);
					Object *objNext = keyRefNext->GetObject(0);

					float x = obj->x+(objNext->x-obj->x)*t;
					float y = obj->y+(objNext->y-obj->y)*t;
					float angle = objNext->angle-obj->angle;
					if (keyRef->IsSpinCounterClockwise())
					{
						if (angle < 0)
							angle = (objNext->angle+360)-obj->angle;
					}
					else
					{
						if (angle > 0)
						{
							angle = (objNext->angle-360)-obj->angle;
						}

					}

					if (ref->timeline != refNext->timeline)	
						t = 0;

					angle = obj->angle+(angle)*t;

					if (angle >= 360)
						angle -= 360;

					float px = obj->pivot_x+(objNext->pivot_x-obj->pivot_x)*t;
					float py = obj->pivot_y+(objNext->pivot_y-obj->pivot_y)*t;

					CCPoint newPos = ccp(x, y);
					obj->sprite->setPosition(newPos);
					obj->sprite->setRotation(-angle);
					obj->sprite->setAnchorPoint(ccp(px, py));
						
					animato->addChild(obj->sprite);
				}
				

			}
		}
		

	}

	void Animation::setDec( float afBegin /*= 0.0*/, float afEnd /*= -1*/ )
	{
		mfBegin = afBegin;
		if(afEnd >= 0) mfEnd = afEnd;
		else mfEnd = mLength;
	}

	//void Animation::Render()		//TODO:某些函数已经没有用了，需要被移除
	//{
	//	//sheet->removeAllChildren();
	//	
	//	Key *keyframe = mMainline->GetKeyframe(mCurrKeyframe);

	//	int count = keyframe->GetObjectRefCount();
	//	for (int i=0;i<count;i++)
	//	{
	//		ObjectRef *ref = keyframe->GetObjectRef(i);

	//		if (ref)
	//		{

	//			Key *keyRef = mTimelines[ref->timeline]->GetKeyframe(ref->key);
	//			Object *obj = keyRef->GetObject(0);									// should be only 1 object
	//			//obj->sprite->visit();
	//					
	//		}

	//	}


	//}


	///////////////////////////////////////////////////////////////////////////////////

	Entity::Entity()
		: mCurrAnimation(0)
		, mId(0)
	{
		mAnimations.reserve(50);
	};


	Entity::~Entity()
	{
		int count = mAnimations.size();
		for (int i=0;i<count;i++)
			CC_SAFE_DELETE(mAnimations[i]);

		mAnimations.clear();
		mAniNames.clear();
	}


	void Entity::Update(float dt)
	{
		
		Animation *animation = mAnimations[mCurrAnimation];
		animation->Update(dt);

	}


	//void Entity::Render()
	//{
	//	Animation *animation = mAnimations[mCurrAnimation];
	//	animation->Render();

	//}

	void Entity::PlayTarget(const char *name, float afBegin /*= 0.0*/, float afEnd/* = -1*/)
	{
		mCurrAnimation = mAniNames[name];
		Animation *animation = mAnimations[mCurrAnimation];
		animation->setDec(afBegin,afEnd);
		animation->Restart();
	}

	void Entity::NextAnimation()
	{
		mCurrAnimation++;
		if (mCurrAnimation >= (int)mAnimations.size())
			mCurrAnimation = 0;

		Animation *animation = mAnimations[mCurrAnimation];
		animation->Restart();

	}


	void Entity::SetId(int id)
	{
		mId = id;

	}


	void Entity::SetName(const char *name)
	{
		mName = name;

	}


	void Entity::AddAnimation(Animation *animation)
	{
		mAnimations.push_back(animation);
		int i =mAnimations.size();
		mAniNames[animation->mName] = i - 1;//= animation;

	}

}

///////////////////////////////////////////////////////////////////////////////////

using namespace SCMLHelper;
using namespace std;


CCSpriterX::CCSpriterX()
{
	mFolders.reserve(50);
	mEntities.reserve(50);
	miPlayTimes = -1;
	//[SPX]CCLOG("<Spx init.");
}


CCSpriterX::~CCSpriterX()
{
	for(vector<Entity*>::iterator it = mEntities.begin(); it != mEntities.end(); ++it){
		delete *it;
	}

	for(vector<Folder*>::iterator it = mFolders.begin(); it != mFolders.end(); ++it){
		delete *it;
	}

	mEntities.clear();
	mFolders.clear();
	//[SPX]CCLOG("<Spx dec");
	//mEntities::iterator
}


CCSpriterX *CCSpriterX::create(const char *filename)
{
	CCSpriterX *animator = new CCSpriterX();
	//[SPX]CCLOG("<new spx");
	if (animator && animator->initWithFile(filename))
	{
		animator->autorelease();

		return animator;
	}
	CC_SAFE_DELETE(animator);
	return NULL;
}


void CCSpriterX::update(float dt)
{
	if (dt > 0.0167f)
		dt = 0.0167f;			//DN:帧率控制

	Entity *entity = mEntities[mCurrEntity];
	entity->Update(dt);

}


//void CCSpriterX::draw(void)
//{
//	Entity *entity = mEntities[mCurrEntity];
//	entity->Render();
//
//}


void CCSpriterX::PlayAnim(const char* name, int aiTimes, const char* alast, float afBegin, float afEnd){
	// <再次变更，使用同一参数的动画将不会被播放
	static float ca = -0.8;
	static float cb = -0.8;

	if(strcmp(msCur.c_str(),name) == 0 && afBegin == ca && afEnd == cb) {
		////[SPX]//[PLAYSTATEDEBUG]CCLog(">[spx]Duplicate name:%s", name);
		return;
	}

	ca = afBegin;
	cb = afEnd;

	//////////////////////////////////////////////////////////////////////////
	//

	miPlayTimes = aiTimes - 1;
	Entity *entity = mEntities[mCurrEntity];		//当前4a版只有单entity
	msCur = name;
	entity->PlayTarget(name, afBegin, afEnd);
	msLast = alast;
	mbNoLast = false;
	//[PLAYSTATEDEBUG]CCLog(">[spx]Play Animation:%s. %d times",name,aiTimes);
	//[0803]CCLog(">Play %s&Coding Last:%s",name,msLast.c_str());

}

void CCSpriterX::PlayNext()
{
	//[SPX]//[PLAYSTATEDEBUG]CCLog(">[spx]Play Next()");
	Entity *entity = mEntities[mCurrEntity];
	entity->NextAnimation();

}

CCSprite *CCSpriterX::getSprite(int folderId, int fileId)		//FN:通过文件夹和文件ID获得sprite
{
	if (folderId < (int)mFolders.size())
	{
		Folder *folder = mFolders[folderId];

		if (folder)
		{
			File *file = folder->GetFile(fileId);

			if (file)
				return file->sprite;
		}
	}

	return NULL;
}


bool CCSpriterX::initWithFile(const char *filename)
{
	initWithSpriteFrameName("gb-up-stand-1.png");
	setOpacity(0);

	mCurrEntity = 0;

	unsigned long filesize;
	string path = CCFileUtils::sharedFileUtils()->fullPathForFilename(filename);
	char *buffer = (char *)CCFileUtils::sharedFileUtils()->getFileData(path.c_str(), "rb", &filesize);

	if (buffer == NULL)
		return false;


	TiXmlDocument doc;

	doc.Parse(buffer);

	TiXmlNode *root = doc.FirstChild("spriter_data"); 
	if (root)
	{
		
		TiXmlElement *element = root->ToElement();

		const char *version = element->Attribute("scml_version");
		const char *generator = element->Attribute("generator");
		const char *generatorVersion = element->Attribute("generator_version");

			

		for (TiXmlNode* entityNode = root->FirstChild(); entityNode; entityNode = entityNode->NextSibling())
		{
			element = entityNode->ToElement();

			if (element)
			{
				const char *tab = element->Value();

				if (strcmp(tab, "folder")==0)
				{
					Folder *folder = new Folder();

					folder->Init(entityNode);

					mFolders.push_back(folder);

				}
				else if (strcmp(tab, "entity")==0)
				{
					int intValue;
					Entity *entity = new Entity();

					if (element->QueryIntAttribute("id", &intValue) == TIXML_SUCCESS)
						entity->SetId(intValue);

					entity->SetName(element->Attribute("name"));

					for (TiXmlNode* animationNode = entityNode->FirstChild(); animationNode; animationNode = animationNode->NextSibling())
					{
						Animation *animation = new Animation();
						animation->Init(animationNode, this);

						entity->AddAnimation(animation);

					}

					mEntities.push_back(entity);

				}

			}

		}

	}

	CC_SAFE_DELETE_ARRAY(buffer);

	mbPaused = false;
	this->scheduleUpdate();

	return true;

}

void CCSpriterX::PlayLast()
{
	//[0803]
	//[PLAYSTATEDEBUG]CCLog(">[spx]Play Last.Las:%s,Cur:%s",msLast.c_str(),msCur.c_str());
	msCur.clear();
	if(msLast.length()>0){
		PlayAnim(msLast.c_str());
	}else{
		msLast = "stand_left";
		PlayLast();

		//[SPX]
		//[PLAYSTATEDEBUG]
		CCLog(">[spx]:No Last.");
	}
}

void CCSpriterX::SetNoLast( bool anl )
{
	mbNoLast = anl;
}

void CCSpriterX::PauseMe()
{
	mbPaused = true;
	unscheduleUpdate();
}

void CCSpriterX::ResuMe()
{
	mbPaused = false;
	scheduleUpdate();
}




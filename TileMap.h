#ifndef __TILEMAP_H__
#define __TILEMAP_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "utils/Scriptor.h"
#include "utils/Entiles.h"
#include "utils/ETile.h"
#include "utils/EPhsics.h"
#include "utils/mSpotLight.h"
#include "utils/Controller.h"
#include "byeven/BYCocos.h"

#include "GLES-Render.h"
//#define DEBUGDRAW				//�����������

/*
|	�ƶ���ͼ���Ʋ�  -- MapLayer����
|	
|
*/


//#include "GameManager.h"

USING_NS_CC;

#define kTagMap 1
#define PTM_RATIO 32  //�����һ�㶨��Ϊ�� 32.0����box ������ ���� �� Ϊ��λ�ģ������ǽ�����һ�Ϊbox�����е��ף������� PTM_RATIO
#define MapdDepth -10000	//��֤��ͼ�㹻�ĵ�


class CollideInfo : public CCObject
{
public:
	b2Fixture *f1;
	b2Fixture *f2;
	int type;

	CollideInfo(b2Fixture* fi1, b2Fixture* fi2, int t){
		f1 = fi1;
		f2 = fi2;
		type = t;
	}
};


class MyQueryCallback : public b2QueryCallback {		//���ڴ��������������
public:
	std::vector<b2Fixture*> foundBodies;

	bool ReportFixture(b2Fixture* fixture) {
		foundBodies.push_back( fixture );
		return true;//keep going to find all fixtures in the query area
	}
};



class MyContactListener : public b2ContactListener		//b2world �Ӵ�������
{     
public:	
	std::vector<CollideInfo*> bc;


	/// Called when two fixtures begin to touch.
	void BeginContact(b2Contact* contact) {
		
		
		Entiles* e1 = (Entiles*) contact->GetFixtureA()->GetUserData();
		Entiles* e2 = (Entiles*) contact->GetFixtureB()->GetUserData();
		if(e1) CCLOG(">body a:%s",e1->name.c_str());
		if(e2) CCLOG(">body b:%s",e2->name.c_str());

		bc.push_back(new CollideInfo(contact->GetFixtureA(),contact->GetFixtureB(),1));
	}

	/// Called when two fixtures cease to touch.
	void EndContact(b2Contact* contact) {
		bc.push_back(new CollideInfo(contact->GetFixtureA(),contact->GetFixtureB(),2));
	}

	/// This is called after a contact is updated. This allows you to inspect a
	/// contact before it goes to the solver. If you are careful, you can modify the
	/// contact manifold (e.g. disable contact).
	/// A copy of the old manifold is provided so that you can detect changes.
	/// Note: this is called only for awake bodies.
	/// Note: this is called even when the number of contact points is zero.
	/// Note: this is not called for sensors.
	/// Note: if you set the number of contact points to zero, you will not
	/// get an EndContact callback. However, you may get a BeginContact callback
	/// the next step.
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		B2_NOT_USED(oldManifold);
		B2_NOT_USED(contact);
	}

	/// This lets you inspect a contact after the solver is finished. This is useful
	/// for inspecting impulses.
	/// Note: the contact manifold does not include time of impact impulses, which can be
	/// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
	/// in a separate data structure.
	/// Note: this is only called for contacts that are touching, solid, and awake.
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}
};

class TileMap : public BYLayerDescendant, public Controller
{
public:
	~TileMap();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
	void onEnter();

	CCSpriteBatchNode *sheet;


	mSpotLight* spotLight;

	b2World *_world; 
	b2Body *_body; 

	float vx;
	float vy;
	float ax;
	float ay;


	void w_press(bool up = false);
	void s_press(bool up = false);
	void a_press(bool up = false);
	void d_press(bool up = false);
	void z_press(bool up = false);
	virtual void b_click();

    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
	CCArray* m_ea;								//TODO: m_ea���𱣴�ÿһ��ʵ���������ȷ��������ȷ���ҵ���;�������һ����m_itemlist���ظ������;�����
	virtual void update(float dt);
	virtual void ELoad();
	virtual void draw();

	b2Draw *m_debugDraw;

	TileMap(const char* a, const char* b);
	TileMap(const char* a, const char* b, Script* asp);
	TileMap(){
		sp = NULL;
		m_ea = NULL;
//		autorelease();
	};

	 Scriptor* sp;
	 Entiles* m_controller;				//���ƽ���ָ�룬�����Ͽ��Կ����κε�λ��

	 CCDictionary* m_elist;				//��ͼsp
	 CCDictionary* m_itemlist;			//Entile�������ڵ�����б��У�ʹ�� group_name ��Ϊ������ ʹ��group �� mask id�Է���������֡�

	 string m_sNaCon, m_sNaCam, m_sNaScp;
	 int m_iNaScp;


	 CCTMXTiledMap* m_tilemap;
	 float x0,y0,dx,dy,dtx,dty,rw,rh;
	 


	 void f_setcontroller(Entiles* e);
	 void f_setcamara(Entiles* e);


	 CCPoint m_getTilec(CCPoint c);
	 CCPoint m_getViewc(CCPoint c);
	 CCPoint m_checkPoint(CCPoint c);
	 Entiles* m_getEntile(const char* group,const char* name);
	 Entiles* m_getEntile(const char* gn);

	 void f_worldsearch(b2AABB* aabb);
	 MyContactListener* colse;
	 Script* f_scrfind(string na, string nb, int t);
	 virtual Script* f_scrfind(string gn, int t);

	 string getfname(b2Fixture* f);		// Return the data name. If fixture is sensor then return the sensor name. Or it will give back the data in Parent Object.

	 b2Fixture* fa;
	 b2Fixture* fb;	//�������

	 void DerMapAct(Script* s);
	 bool cancontrol;
	 void clearcontrol();


	 float round(float r);
	 virtual void update_b2world(float dt);
	 void update_collide();
	 void test_disable_all_entiles();

	string map_path;
	string scr_path;
	bool m_isLoad;
	Script* lsp;

	CCRenderTexture* renderLayer;
};

#endif	// __TILEMAP_H__
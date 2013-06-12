#ifndef __ENTILES_H__
#define __ENTILES_H__

#include "cocos2d.h"

#include "utils/SimControl.h"
#include "Macros.h"

USING_NS_CC;

#define PTM_RATIO 32 
#define S_WID 0.5
#define S_HEI 0.25
 
typedef enum{
	MS_LEFT,
	MS_RIGHT,
	MS_UP,
	MS_DOWN,
	MS_STOP
} MoveDirect;

//���е�ͼʵ��Ļ���
//��֤ʵ����ĵ����ԣ�ʵ���಻�������HUD��ʾ������HUD���������ML��TM����
class Entiles : public CCNode, public SimControl
{
public:
	b2Body *m_body;
	string name,group,psz;
	CCSprite *m_sprite;

	bool b_CanMove;
	MoveDirect direc;
	MoveDirect stand;
	bool b_Dirty,b_IsControler,b_Re;
	string m_sTarget;
	int state;
	/*
	| b_dirty λ�÷����˱������Ҫ�ػ�
	| b_iscontroler ��ǰentiles����˿���
	| b_re �Ƿ��ǿ��ƶ������壬��Ҫ�����¼���zorder
	*/
	Entiles();
	~Entiles();

	virtual void initFiles(const char *pszFileName, const CCRect& rect);			//Do not use this.It now calls initwithfile() and is not compatible with CCSpriteFrameCache. In fact you do not need an additional rect with SpriteFrameCache.
	virtual void initFiles(const char *pszFileName);

	virtual void lin();
	virtual b2AABB* f_search();

	virtual void SCTarget(SimControl* tar);
	virtual void DecidePath(SimControl* tar);
	virtual bool SCTouch(b2Fixture* self, b2Fixture* tar,bool bLeave = false);			//����ֱ�ӷ����Ӵ���Ŀǰ������֧�ֶඨ�������������Ϊ����������ײ�����ѡ�
	virtual bool SCAttacted(SimControl* src);
	virtual bool SCSearched(SimControl* src);
	virtual bool SCContact(b2Fixture* self, b2Fixture* tar,bool bLeave = false);		//�������봫������������ײ
	virtual bool SCContacted(b2Fixture* self,b2Fixture* tar,bool bLeave = false);						//������Ĵ�������������ײ

	virtual void SCGoto(CCPoint c);
	virtual void SCMoveto(CCPoint c);
	virtual CCPoint SCGetPosition();

	virtual void ELoad();
	string GetFullName();
	//////////////////////////////////////////////////////////////////////////



};

#endif
#ifndef __AB_SCENE_H__
#define __AB_SCENE_H__

/*
	ABScene
	< Avg-Battle Scene| ��������
*/

#include "cocos2d.h"
#include "utils\States.h"
#include "ABUI\Ab_asicLayer.h"
#include "json\reader.h"

USING_NS_CC;

class ABScene : public CCScene, public StatesManager
{
public:
	ABScene(int zid);
	~ABScene();

protected:
	virtual bool	 init();
	void			 ReadConfig();
	int				 mZBId;			/* <����ID */

	Json::Value		mjvConfig;		/* <���� */

private:
	ABasicLayer*	 mlBasic;		/* <���ײ� */

public:
	void			 GotoNext();	/* <ǰ�� */ 
};

#endif
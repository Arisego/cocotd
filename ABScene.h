#ifndef __AB_SCENE_H__
#define __AB_SCENE_H__

/*
	ABScene
	< Avg-Battle Scene| ��������
*/

#include "cocos2d.h"
#include "utils\States.h"
#include "ABUI\Ab_asicLayer.h"

USING_NS_CC;

class ABScene : public CCScene, public StatesManager
{
public:
	CREATE_FUNC(ABScene);
	~ABScene();

protected:
	virtual bool init();

private:
	ABasicLayer*	mlBasic;		/* <���ײ� */
};

#endif
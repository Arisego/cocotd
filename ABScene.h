#ifndef __AB_SCENE_H__
#define __AB_SCENE_H__

/*
	ABScene
	< Avg-Battle Scene| 整备界面
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
	int				 mZBId;			/* <整备ID */

	Json::Value		mjvConfig;		/* <配置 */

private:
	ABasicLayer*	 mlBasic;		/* <基底层 */

public:
	void			 GotoNext();	/* <前进 */ 
};

#endif
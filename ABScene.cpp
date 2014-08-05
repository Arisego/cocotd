#include "ABScene.h"

ABScene::~ABScene()
{

}

bool ABScene::init()
{
	do 
	{
		mlBasic = ABasicLayer::create();
		addChild(mlBasic,0);
		AddState(mlBasic);

		e_State = 1;
		return true;
	} while (false);
	return false;

}


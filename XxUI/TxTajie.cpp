#include "TxTajie.h"

void TxTajie::f_SetGrey(bool aGrey)
{
	if (mbGrey == aGrey) return;

	if (aGrey){
		setColor(ccc3(mcOrigColor.r - 100, mcOrigColor.g - 100, mcOrigColor.b - 100));
	}else{
		setColor(mcOrigColor);
	}

	mbGrey = aGrey;
}

TxTajie* TxTajie::create(const char *pszFileName)
{
	TxTajie *pobSprite = new TxTajie();
	if (pobSprite && pobSprite->initWithFile(pszFileName))
	{
		pobSprite->autorelease();
		pobSprite->f_Init();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}

void TxTajie::f_Init()
{
	mcOrigColor = getColor();
}


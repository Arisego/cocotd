#ifndef __TX_TAJIE_H__
#define __TX_TAJIE_H__

#include "cocos2d.h"

USING_NS_CC;

class TxTajie : public CCSprite{
private:
	bool		mbGrey;
	ccColor3B	mcOrigColor;

public:
	void		f_SetGrey(bool aGrey);
	static		TxTajie* create(const char *pszFileName);

protected:
	void		f_Init();


};

#endif
#ifndef __LINE_PRIOER_H__
#define __LINE_PRIOER_H__

#include "packui/Container.h"
#include "packui/CharaS.h"

class LinePin : public CCSprite
{
public:
	string	name;
	int		m_iStep;
	int		m_iFlag;
	float	m_fFactor;

	void f_init()
	{
		initWithFile("Images/chara_pin.png");
	}

	void replace_pin(){					//TODO: decide ti by attrs.
		m_iStep = 160 + rand()%30;
	}
};

//LinePrioer -- For BattleMap to decid the next Controlling Entile.
class LinePrior : public BYLayerDescendant
{
public:
	~LinePrior();
	bool f_init();
	
	void addPin(Chara* ,string);
	void removePin(string);
	
	void tick();

	CCDictionary* m_cdPins;
	void refresh_pin();			//Refresh and replace the pins.
	CCArray*	  m_caPinSteped;

	void setactivator(CCObject* target, SEL_MenuHandler selector);
	void activate();
private:
	//Activate Group
	CCObject*       m_pListener;
	SEL_MenuHandler    m_pfnSelector;
	bool m_bIsEnabled;

};


#endif
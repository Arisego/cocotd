#ifndef __B_EQUIP_H__
#define __B_EQUIP_H__

using namespace std;
USING_NS_CC;

struct Equip : cocos2d::CCObject
{
	int id;
	string effect;
	string name;
	string discript;
	int position;

	int lock,sum;
};

#endif
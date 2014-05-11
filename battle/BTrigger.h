#ifndef __B_TRIGGER_H__
#define __B_TRIGGER_H__

#include "cocos2d.h"
USING_NS_CC;

class BTRigger: public CCObject
{
public:
	BTRigger(CCObject* abf);
	~BTRigger();

	virtual void Trig(int ai);

private:
	CCObject* m_buffer;	//BBuffer
};


class TriggerCa
{
public:
	TriggerCa();
	~TriggerCa();

private:
	CCDictionary *m_tcd;									// <内部Trigger索引 | dict [ string - array [ trigger ] ]
	virtual void ClearTriggers();							// <Trig完成之后 | 调用删除 |

public:
	void addTrig(int ai, BTRigger* atri);		// <添加Trigger到指定的类目
	void removeTrig(int ai, BTRigger* atri);	// <移除指定的Trigger
	void Trig(int ai);							// <触发该类别下所有Trigger
};


#endif
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
	CCDictionary *m_tcd;									// <�ڲ�Trigger���� | dict [ string - array [ trigger ] ]
	virtual void ClearTriggers();							// <Trig���֮�� | ����ɾ�� |

public:
	void addTrig(int ai, BTRigger* atri);		// <���Trigger��ָ������Ŀ
	void removeTrig(int ai, BTRigger* atri);	// <�Ƴ�ָ����Trigger
	void Trig(int ai);							// <���������������Trigger
};


#endif
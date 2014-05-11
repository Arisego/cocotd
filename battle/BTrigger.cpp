#include "BTrigger.h"
#include "BBuffer.h"



BTRigger::BTRigger(CCObject* abf)
{
	m_buffer = abf;
}

BTRigger::~BTRigger()
{
}

void BTRigger::Trig(int ai)
{
	((BBuffer*) m_buffer)->Trig(ai);
}

//////////////////////////////////////////////////////////////////////////

TriggerCa::TriggerCa()
{
	m_tcd = new CCDictionary();
}

TriggerCa::~TriggerCa()
{
	CC_SAFE_RELEASE_NULL(m_tcd);
}

void TriggerCa::addTrig(int ai, BTRigger* atri)
{
	CCArray* tca;
	tca = (CCArray*) m_tcd->objectForKey(ai);
	if(!tca){
		tca = new CCArray();
		m_tcd->setObject(tca, ai);
		tca->autorelease();
	}
	tca->addObject(atri);

}

void TriggerCa::removeTrig(int ai, BTRigger* atri)
{
	CCArray* tca;
	tca = (CCArray*) m_tcd->objectForKey(ai);
	if(!tca){
		return;
	}
	tca->removeObject(atri);
	atri->release();
}

void TriggerCa::Trig(int ai)
{
	CCArray* tca = (CCArray*) m_tcd->objectForKey(ai);
	if(!tca){
		CCLog(">[BTriggerCa] Asking for tringger not registed. %d", ai);
		return;
	}
	
	CCObject* tco;
	CCARRAY_FOREACH(tca,tco){
		((BTRigger*) tco)->Trig(ai);
	}
	ClearTriggers();
}

void TriggerCa::ClearTriggers()
{
	CCLog(">[TriggerCa] ClearTriggers() | Over Ride Me");
}

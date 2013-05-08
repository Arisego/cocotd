#ifndef __SCRIPTOR_H__
#define __SCRIPTOR_H__


/** 
xml-->script
*/

#include "cocos2d.h"
#include "utils/FileIO.h"
#include "tinyxml/tinyxml.h"

USING_NS_CC;
using namespace std;

typedef enum
{
	sUnknown = 0,
	sFilePack,
	sReadFile,
	sScriptPack,
	sAct,
	sShowText,
	sSelect,
	sChoice,
	sLoadImg,
	sAction,
	sFinal,
	sLock,
	sSilent,
	sList,
	sItem,
	sMapscp,
	sNameNode,
	sTypeNode,
	sChange,
	sMapAction
}ScriptType;

typedef enum
{
	vInt = 0,
	vFloat
}ValueType;

class NFloat : public CCObject
{
public:
	float f;

	NFloat(float fv){
		f = fv;
	}
};



class Script : public CCObject{
public:
	ScriptType type;	
	string content;

	int m_anum, m_snum;

	CCDictionary* attributes;
	CCArray* scriptnodes; 
	CCDictionary* mapscpnodes;

	//virtual void release(){
	//	//CCObject::release();
	//	if(m_uReference < 3)
	//		delete this;
	//	else
	//		CCObject::release();
	//}

	//virtual CCObject* autorelease(){
	//	return CCObject::autorelease();
	//}

	Script(){
		type = sUnknown;
		m_snum = 0;
		m_anum = 0;
		attributes = NULL;
		scriptnodes = NULL;
		mapscpnodes = NULL;
		//autorelease();
	}

	~Script(){
		//if(attributes) {
		//	//CCDictElement* tcde;
		//	//CCDICT_FOREACH(attributes,tcde){
		//	//	if(tcde->getObject()->retainCount()>1)
		//	//		tcde->getObject()->release();
		//	//}
		//	//attributes->removeAllObjects();
		//	attributes->release();
		//}


		CC_SAFE_RELEASE_NULL(attributes);

		//if(scriptnodes) scriptnodes->removeAllObjects();
		CC_SAFE_RELEASE_NULL(scriptnodes);

		//if(mapscpnodes) mapscpnodes->removeAllObjects();
		CC_SAFE_RELEASE_NULL(mapscpnodes);
	}

	const char* getstring(const char* tag){

		CCString* t_cs = (CCString*) attributes->objectForKey(tag);
		if(!t_cs) return "";
		
		return t_cs->getCString();
	}

	int getint(const char* tag){
		if(!attributes) return 0;
		CCInteger* t_cs = (CCInteger*) attributes->objectForKey(tag);
		if(!t_cs) return 0;
		return t_cs->getValue();
	}

	float getfloat(const char* tag){
		NFloat* nf = (NFloat*) attributes->objectForKey(tag);
		if(!nf) return 0;
		return nf->f;
	}

	
	
};



const unsigned int NUM_INDENTS_PER_SPACE=2;

class Scriptor {

private:
	FileIO* m_curfi;
	Script* m_scWhole;
	

public:
	CCArray* initcs;
	CCArray* m_caScript;
	CCDictionary* mapscps;
	int sn,in;

	Scriptor();

	void re_init();

	bool parse_string(string line);

	// isRaw and password is for the case that the xml is packed in zip.
	bool parse_file(const char* fname, const char* password = "1",bool isRaw = false);

	CCDictionary* dump_attribs_to_nodes(TiXmlElement* pElement, unsigned int indent, ScriptType st = sUnknown);
	Script* dump_to_nodes( TiXmlNode* pParent, unsigned int indent = 0, ScriptType st = sUnknown);

	~Scriptor();

};

#endif


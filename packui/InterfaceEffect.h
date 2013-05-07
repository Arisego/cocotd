#ifndef __INTERFACE_EFFECT_H__
#define __INTERFACE_EFFECT_H__

using namespace std;

//作为发生效果的接口，目前作为物品使用和技能使用的最终接口。
class InterfaceEffect
{
public:
	CCArray* m_caTList;				// member CcArray Target List.

	virtual void get_target() = 0;
	virtual void effect_over() = 0;

	virtual void show_text(string s) = 0;	//Interface的实现者必然知道应该如何显示文本，否则它就不应该实现这个Interface
	virtual void refresh_view() = 0;		//刷新显示。
	virtual void handlesp(Script* sp){
		
	};		//handle it if you want.

	InterfaceEffect(){
		m_caTList = NULL;
	}

	~InterfaceEffect(){
		CC_SAFE_DELETE(m_caTList);
	}
};

#endif
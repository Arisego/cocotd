#ifndef __INTERFACE_EFFECT_H__
#define __INTERFACE_EFFECT_H__

using namespace std;

//��Ϊ����Ч���Ľӿڣ�Ŀǰ��Ϊ��Ʒʹ�úͼ���ʹ�õ����սӿڡ�
class InterfaceEffect
{
public:
	CCArray* m_caTList;				// member CcArray Target List.

	virtual void get_target() = 0;
	virtual void effect_over() = 0;

	virtual void show_text(string s) = 0;	//Interface��ʵ���߱�Ȼ֪��Ӧ�������ʾ�ı����������Ͳ�Ӧ��ʵ�����Interface
	virtual void refresh_view() = 0;		//ˢ����ʾ��
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
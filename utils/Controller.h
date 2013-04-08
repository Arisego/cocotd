#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

/*
|	����������
|
|
*/

class Controller
{
public:
	virtual void w_press(bool up = false) = 0;	//�����ƶ�
	virtual void s_press(bool up = false) = 0;	//�����ƶ�
	virtual void a_press(bool up = false) = 0;	//�����ƶ�
	virtual void d_press(bool up = false) = 0;	//�����ƶ�
	virtual void z_press(bool up = false) = 0;	//���鰴ť
	virtual void b_click(){};					//Test Only.
	//virtual void esc_press() = 0;				//�˵���ť		-->	�˵�������EventCenterͳһ����
};

class Scroller
{
public: 
	virtual void scroll_in(WPARAM wParam, LPARAM lParam) = 0;
};

class Selector
{
public:	
	virtual void q_press() = 0;				//��ҳ
	virtual void e_press() = 0;				//�ҷ�ҳ
	virtual void c_press() = 0;				//����
};

class Cake
{
public:
	virtual void right_click() = 0;		//����Ҽ�

};

#endif
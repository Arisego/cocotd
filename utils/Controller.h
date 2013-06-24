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

/*
	Cake
	<[δʵ��]ͬʱӵ�ж�㴦����¼�������

*/
class Cake
{
public:
	virtual void right_click() = 0;		//����Ҽ�

};

/*
	CharaTab
	<��ɫ�������ý���
*/
class CharaTab
{
public:
	virtual void i_press() = 0;
	virtual void j_press() = 0;
	virtual void k_press() = 0;
	virtual void l_press() = 0;

	virtual void w_press() = 0;
	virtual void s_press() = 0;
	virtual void a_press() = 0;
	virtual void d_press() = 0;
	virtual void z_press() = 0;
	virtual void x_press() = 0;
};

#endif
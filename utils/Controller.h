#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

/*
|	控制器基类
|
|
*/

class Controller
{
public:
	virtual void w_press(bool up = false) = 0;	//向上移动
	virtual void s_press(bool up = false) = 0;	//向下移动
	virtual void a_press(bool up = false) = 0;	//向左移动
	virtual void d_press(bool up = false) = 0;	//向右移动
	virtual void z_press(bool up = false) = 0;	//调查按钮
	virtual void b_click(){};					//Test Only.
	//virtual void esc_press() = 0;				//菜单按钮		-->	菜单弹出由EventCenter统一控制
};

class Scroller 
{
public: 
	virtual void scroll_in(WPARAM wParam, LPARAM lParam) = 0;
};

class Selector
{
public:	
	virtual void q_press() = 0;				//左翻页
	virtual void e_press() = 0;				//右翻页
	virtual void c_press() = 0;				//调用
};

/*
	Cake
	<[未实现]同时拥有多层处理的事件管理类

*/
class Cake
{
public:
	virtual void right_click() = 0;		//鼠标右键

};

/*
	CharaTab
	<角色属性设置界面
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
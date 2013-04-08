#ifndef __STATES_H__
#define __STATES_H__

#include <vector>
using namespace std;

/*
|	控制状态管理 -- 不负责进行状态内容的清理
|
|
*/

class StateMachine
{
	
public:
	bool b_StateRunning;
	virtual void Close() = 0;	//关闭显示
	virtual void Pause() = 0;	//暂停事件流
	virtual void Resume() = 0;

};

class StatesManager
{
private:
	vector<StateMachine*> states;
public:
	int las_state;
	int e_State;
	StateMachine* last;
	int miType,miFlag,miTab;
	bool m_bCanSnap,m_bScrLock;

	StatesManager(){
		m_bCanSnap = true;
		m_bScrLock = false;
		e_State = -1;
		las_state = 0;
		miType = 0x001f;
		miFlag = 0x0001;
		miTab = 0;
	}

	virtual void resumestate(){
		e_update(las_state);
		e_gonext();
	}

	virtual void e_update(int new_state){
		las_state = e_State;
		e_State = new_state;
	}

	virtual void e_gonext(){
		;
	}

	virtual void NoQuit(){
		m_bCanSnap = true;
		last->Resume();
	}

	virtual void PreQuit(){
		m_bCanSnap = false;
		vector<StateMachine*>::iterator it = states.begin();
		for(;it != states.end();++it){
			(*it)->Pause();
		}
	}

	virtual void AddState(StateMachine* as){
		last = as;
		states.push_back(as);
	}

	virtual void ActiveOnly(StateMachine* as){
		vector<StateMachine*>::iterator it = states.begin();
		last = as;
		for(;it != states.end();++it){
			(*it)->Pause();
		}
		as->Resume();
	};

	virtual void OpenOnly(StateMachine* as){
		last = as;
		vector<StateMachine*>::iterator it = states.begin();
		for(;it != states.end();++it){
			(*it)->Close();
		}
		as->Resume();
	};
};

#endif
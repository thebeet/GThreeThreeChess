/*
 * Player_AI.h
 *
 *  Created on: Mar 3, 2009
 *      Author: yonghu
 */

#ifndef PLAYER_AI_H_
#define PLAYER_AI_H_

#include "Player.h"
#include "common.h"
#include "Board_Base.h"

#include <algorithm>

#include <vector>

#include <glibmm/thread.h>
#include <glibmm/main.h>

class Player_AI_Base : public Player
{
public:
	Player_AI_Base();
	virtual ~Player_AI_Base();

protected:
	virtual void GameAction(const ACTION action);
	virtual void GameActionEnd();
	virtual void TimeOver();
	virtual void OtherAction(const ACTION action);
	virtual void Stop();

	void Make_Action(const int p0);
	bool Do_Action(const int p0, const int p1);
	virtual void Busy();
	void Stop_Busy();

private:
	bool _Busy();
	void Main();
	void Quit(const bool wait = true);
	void Connect_Busy();
	Glib::RefPtr<Glib::MainContext> m_maincontext;
	Glib::RefPtr<Glib::MainLoop> m_mainloop;
	sigc::connection m_conn_busy;
	sigc::connection m_conn_action;
	Glib::Mutex m_runing_mutex;
	bool isstart;
};

class Player_Random : public Player_AI_Base
{
public:
	Player_Random();
	virtual ~Player_Random();
protected:
	virtual void Busy();
};

class Player_AI : public Player_AI_Base
{
public:
	Player_AI();
	~Player_AI();
protected:
	virtual void Busy();
private:
	int m_action_tmp;
};

#endif /* PLAYER_AI_H_ */

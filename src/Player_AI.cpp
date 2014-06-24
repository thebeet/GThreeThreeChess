/*
 * Player_AI.cpp
 *
 *  Created on: Mar 3, 2009
 *      Author: yonghu
 */

#include "Player_AI.h"

#include <time.h>

#include "Userful_Board_Bits.h"
#include "Board_AI.h"
#include "Lock.h"

Player_AI_Base::Player_AI_Base() :
	isstart(false)
{
	m_type = PlayerComputer;
	DBG("Player_AI_Base::Player_AI_Base()");
	srand(time(NULL));
	Glib::Mutex::Lock lock(signal_slot_mutex);
	m_maincontext = Glib::MainContext::create();
	m_mainloop = Glib::MainLoop::create(m_maincontext);
	Glib::Thread::create(sigc::mem_fun(*this, &Player_AI_Base::Main), false);
	while (isstart == false)
	{
		Glib::Thread::yield();
	}
	DBG("Player_AI_Base::Player_AI_Base() END");
}

Player_AI_Base::~Player_AI_Base()
{
	DBG("Player_AI_Base::~Player_AI_Base(%d)", GetColor());
	Quit(true);
}

void Player_AI_Base::Main()
{
	DBG("Player_AI_Base::Main");
	Glib::Mutex::Lock lock(m_runing_mutex);
	isstart = true;
	m_mainloop->run();
	DBG("m_mainloop->run() return");
}

void Player_AI_Base::Quit(const bool wait)
{
	DBG("Player_AI_Base::Quit()");
	GameActionEnd();
	//Glib::Mutex::Lock lock(signal_slot_mutex);
	if (m_mainloop->is_running())
	{
		m_mainloop->quit();
		//lock.release();
		if (wait)
		{
			Glib::Mutex::Lock lock_runing(m_runing_mutex);
		}
	}
}

void Player_AI_Base::Busy()
{
	DBG("Player_AI_Base::Busy()");
}

bool Player_AI_Base::_Busy()
{
	DBG("Player_AI_Base(%d)::_Busy()", GetColor());
	Glib::Mutex::Lock busy_lock(busy_mutex);
	Set_Busy_Signal_Start();
	StartTimer();
	Busy();
	CloseTimer();
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_conn_busy.disconnect();
	}
	DBG("Player_AI_Base(%d)::_Busy() ret", GetColor());
	return false;
}

void Player_AI_Base::Connect_Busy()
{
	DBG("Player_AI_Base::Connect_Busy");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_conn_busy = m_maincontext->signal_idle().connect(sigc::mem_fun(*this, &Player_AI_Base::_Busy), Glib::PRIORITY_DEFAULT);
	}
}

void Player_AI_Base::GameAction(const ACTION action)
{
	DBG("Player_AI_Base(%d)::GameAction(%d)", GetColor(), action);
	Player::GameAction(action);
	Connect_Busy();
}

void Player_AI_Base::GameActionEnd()
{
	DBG("Player_AI_Base::GameActionEnd()");
	if (GetActive())
	{
		Player::GameActionEnd();
		Stop_Busy();
		{
			Glib::Mutex::Lock lock(signal_slot_mutex);
			if (m_conn_action.connected())
			{
				m_conn_action.disconnect();
			}
		}
	}
}

void Player_AI_Base::TimeOver()
{
	DBG("PLayer_AI_Base::TimeOver()");
	Stop_Busy();
}

void Player_AI_Base::OtherAction(const ACTION action)
{
	DBG("Player_AI_Base(%d)::OtherAction(%d)", GetColor(), action);
}

void Player_AI_Base::Stop()
{
	DBG("Player_AI_Base::Stop()");
	Player::Stop();
	Quit(true);
}

void Player_AI_Base::Stop_Busy()
{
	DBG("Player_AI_Base::Stop_Busy()");
	Set_Busy_Signal_Stop();
	Glib::Mutex::Lock busy_lock(busy_mutex);
}

void Player_AI_Base::Make_Action(const int p0)
{
	DBG("Player_AI_Base::Make_Action(%d)", p0);
	if (GetActive())
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_conn_action = Glib::signal_timeout().connect(sigc::bind<int, int>(sigc::mem_fun(*this, &Player_AI_Base::Do_Action), p0 & 0xff, p0 >> 8), 0);
	}
}

bool Player_AI_Base::Do_Action(const int p0, const int p1)
{
	DBG("Player_AI_Base::Do_Action");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		if (m_conn_action.connected())
		{
			m_conn_action.disconnect();
		}
		else
		{
			DBG("WARNING!!! Conn_Action Disconnect!");
		}
	}
	Player::Do_Action(p0, p1);
	return false;
}

Player_Random::Player_Random()
{

}

Player_Random::~Player_Random()
{

}

void Player_Random::Busy()
{
	DBG("Player_Random::Busy()");
	DBG("Current Action is %d", m_current_action);
	if (m_current_action == Action_Move)
	{
		Make_Action(((rand() % PMax) << 8) | (rand() % PMax));
	}
	else
	{
		Make_Action(rand() % PMax);
	}
}

Player_AI::Player_AI()
{
}

Player_AI::~Player_AI()
{
}

void Player_AI::Busy()
{
	DBG("Player_AI::Busy()");
	DBG("Current Action is %d", m_current_action);
	Board_AI ai_board(*m_board);
	Make_Action(ai_board.CalcAction(GetColor(), m_current_action));
}

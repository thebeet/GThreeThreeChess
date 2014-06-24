/*
 * Player_Control.cpp
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#include "Player.h"

#include "Lock.h"

#include <glibmm/main.h>

const char *PlayerTypeName(const PlayerType type)
{
	switch (type)
	{
		case PlayerNone:
		{
			return "None";
			break;
		}
		case PlayerHuman:
		{
			return "Human";
			break;
		}
		case PlayerComputer:
		{
			return "Computer";
			break;
		}
		default:
		{
			return "ERROR_TYPE";
			break;
		}
	}
}

Player::Player() :
	m_board(NULL), m_type(PlayerNone), m_use_time(5000), m_active(false), m_started(false)
{
	Glib::Mutex::Lock lock(signal_slot_mutex);
	SignalAction().connect(sigc::mem_fun(*this, &Player::_Action));
}

Player::~Player()
{
	Stop();
}

PlayerType Player::GetType() const
{
	return m_type;
}

void Player::SetColor(const Color color)
{
	DBG("Player::SetColor(%d)", color);
	m_color = color;
}

Color Player::GetColor() const
{
	return m_color;
}

void Player::Attend(Board &board)
{
	DBG("Player::Attend");
	m_board = &board;
}

void Player::SetTime(const int use_time)
{
	DBG("Player::SetTime(%d)", use_time);
	m_use_time = use_time;
}

int Player::GetTime() const
{
	return m_use_time;
}

bool Player::GetActive() const
{
	return m_active;
}

bool Player::GetStarted() const
{
	return m_started;
}

const Board &Player::GetBoard() const
{
	if (m_board == NULL)
	{
		DBG("ERROR Get Null Object");
	}
	return *m_board;
}

Board &Player::GetBoard()
{
	if (m_board == NULL)
	{
		DBG("ERROR Get Null Object");
	}
	return *m_board;
}


void Player::_Action(const ACTION action)
{
	DBG("Player(%d)::_GameAction(%d)", GetColor(), action);
	if (GetStarted() == false)
	{
		if (action == Action_Start)
		{
			Start();
		}
		return;
	}
	switch (action)
	{
		case Action_Move:
		case Action_Put:
		case Action_Take:
		case Action_Cover:
		{
			GameAction(action);
			break;
		}
		case Action_GameActionEnd:
		{
			GameActionEnd();
			break;
		}
		case Action_TimeOver:
		{
			TimeOver();
			break;
		}
		case Action_Retract:
		{
			Retract();
			break;
		}
		case Action_Stop:
		{
			Stop();
			break;
		}
		default:
		{
			OtherAction(action);
			break;
		}
	}
}

bool Player::_TimeOver()
{
	DBG("Player::_TimeOver");
	SignalAction().emit(Action_TimeOver);
	CloseTimer();
	return false;
}

void Player::GameAction(const ACTION action)
{
	DBG("Player(%d)::GameAction(%d)", GetColor(), action);
	m_current_action = action;
	m_active = true;
}

void Player::GameActionEnd()
{
	DBG("Player(%d)::GameActionEnd()", GetColor());
	if (m_active)
	{
		m_active = false;
	}
	else
	{
		DBG("WARNING! GameActionEnd in active == false");
	}
}

void Player::Retract()
{
	DBG("Player::Retract()");
	m_board->SignalAction().emit(Action(Action_Retract, GetColor()));
}

void Player::TimeOver()
{
	DBG("Player::TimeOver");
}

void Player::Start()
{
	DBG("Player(%d)::Start", m_color);
	m_started = true;
}

void Player::Stop()
{
	DBG("Player(%d)::Stop()", m_color);
	if (GetActive())
	{
		SignalAction().emit(Action_GameActionEnd);
	}
	m_started = false;
}

void Player::OtherAction(const ACTION action)
{

}

void Player::StartTimer()
{
	DBG("Player::StartTimer()");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_timer_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Player::_TimeOver), GetTime());
	}
}

void Player::CloseTimer()
{
	DBG("Player::CloseTimer()");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		if (m_timer_conn.connected())
		{
			m_timer_conn.disconnect();
		}
	}
}

sigc::signal1<void, ACTION> &Player::SignalAction()
{
	return m_signal_action;
}

void Player::Do_Action(const int p0, const int p1)
{
	DBG("Player(%d)::Do_Action(%d, %d)", this->GetColor(), p0, p1);
	Action newaction(m_current_action, this->GetColor(), p0, p1);
	SignalAction().emit(Action_GameActionEnd);
	m_board->SignalAction().emit(newaction);
}

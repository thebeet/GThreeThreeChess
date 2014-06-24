/*
 * Player_Local.cpp
 *
 *  Created on: Mar 3, 2009
 *      Author: yonghu
 */

#include "Player_Local.h"

#include "Board_AI.h"

Player_Local::Player_Local() :
	m_select_pos(0)
{
	m_type = PlayerHuman;
}

Player_Local::~Player_Local()
{
}

void Player_Local::Click(const int p)
{
	DBG("Player_Local::Click(%d)", p);
	switch (m_current_action)
	{
		case Action_Move:
		{
			if (m_select_pos == -1)
			{
				if (m_board->Bit(GetColor()).isSet(p))
				{
					m_select_pos = p;
					//m_board-Slot_Select(p);
					m_board->SignalAction().emit(Action(Action_SlotSelect, GetColor(), p));
				}
			}
			else
			{
				m_board->SignalAction().emit(Action(Action_SlotUnSelect, GetColor(), m_select_pos));
				if ((Do_Action(m_select_pos, p) == Error_Move) &&
						m_board->Bit(GetColor()).isSet(p))
				{
					m_select_pos = p;
					//m_board->Slot_Select(p);
					m_board->SignalAction().emit(Action(Action_SlotSelect, GetColor(), p));
				}
			}
			break;
		}
		case Action_Put: case Action_Take: case Action_Cover:
		{
			Do_Action(p);
			break;
		}
		default:
		{
			DBG("ERROR");
			return;
		}
	}
}

void Player_Local::GameAction(const ACTION action)
{
	DBG("Player_Local::GameAction(%d)", action);
	Player::GameAction(action);
	m_board->SignalAction().emit(Action(Action_RequestControl, GetColor()));
}

void Player_Local::GameActionEnd()
{
	DBG("Player_Local::GameActionEnd()");
	Player::GameActionEnd();
	if (m_select_pos >= 0)
	{
		m_board->SignalAction().emit(Action(Action_SlotUnSelect, GetColor(), m_select_pos));
		m_select_pos = -1;
	}
	m_board->SignalAction().emit(Action(Action_CloseControl, GetColor()));
}

void Player_Local::Stop()
{
	DBG("Player_Local::Stop()");
	if (GetActive())
	{
		GameActionEnd();
	}
	Player::Stop();

}

int Player_Local::Do_Action(const int p0, const int p1)
{
	DBG("Player_Local(%d)::Action(%d, %d)", GetColor(), p0, p1);
	Board_Base boardbase(*m_board);
	if (boardbase.Do_Action(m_current_action, GetColor(), p0, p1) == Error_Move)
	{
		return Error_Move;
	}
	Player::Do_Action(p0, p1);
	return OK;
}

/*
Player_Local_with_AI::Player_Local_with_AI()
{
}

Player_Local_with_AI::~Player_Local_with_AI()
{
}

void Player_Local_with_AI::Turn(const ACTION action)
{
	Board_AI ai_board(*m_board);
	Glib::Thread::create(sigc::bind<Board_AI, ACTION>(sigc::mem_fun(*this, &Player_Local_with_AI::AI_Hint), ai_board, action), false);
	Player_Local::Turn(action);
}

void Player_Local_with_AI::AI_Hint(Board_AI &ai_board, const ACTION action)
{
	int pos = ai_board.CalcAction(GetColor(), action);
	if (action == Action_Move)
	{
		DBG("Hint: %d to %d", pos & 0xff, pos >> 8);
	}
	else
	{
		DBG("Hint: %d", pos);
	}
}
*/

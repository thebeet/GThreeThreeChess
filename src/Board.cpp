/*
 * Board.cpp
 *
 *  Created on: Feb 18, 2009
 *      Author: yonghu
 */

#include "Board.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include "Userful_Board_Bits.h"
#include "Lock.h"

#include <libxml++/libxml++.h>

Board_Stat::Board_Stat(const Board_Base &board, const ACTION action, const Color color) :
	m_board(board), m_action(action), m_color(color)
{
}

Board::Board() :
	m_players(2),
	m_current_player(-1),
	m_current_action(Action_Stop)
{
	DBG("Board::Board()");
	Userful_Board_Bits_Init();
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		SignalAction().connect(sigc::mem_fun(*this, &Board::_Action));
	}
}

Board::~Board()
{
}

void Board::Start(Player &player1, Player &player2)
{
	DBG("Board::Start");
	if (m_current_action != Action_Stop)
	{
		DBG("ERROR Start Again!");
		return;
	}
	Clear();
	m_actions.clear();
	ClearHistory();

	m_players[0] = &player1;
	m_players[1] = &player2;
	m_current_action = Action_Put;
	m_players[0]->Attend(*this);
	m_players[1]->Attend(*this);
	m_players[0]->SetColor(Black);
	m_players[1]->SetColor(White);
	m_current_player = 0;
	m_players[0]->SignalAction().emit(Action_Start);
	m_players[1]->SignalAction().emit(Action_Start);
	DBG("Color of Player1 is %d", m_players[0]->GetColor());
	DBG("Color of Player2 is %d", m_players[1]->GetColor());

	AddHistory();
	EmitCurrentPlayerAction();
}

void Board::ChangePlayer(Player &player, Color color)
{
	DBG("Board::ChangePlayer(..., %d)", color);
	if (m_current_action != Action_Stop)
	{
		int id = 0;
		DBG("Color of Player1 is %d", m_players[0]->GetColor());
		DBG("Color of Player2 is %d", m_players[1]->GetColor());
		id = ((m_players[0]->GetColor() == color) ? 0 : 1);
		DBG("id is %d", id);
		bool active_flag(false);
		if (m_players[id]->GetActive())
		{
			active_flag = true;
			DBG("Changing an Active Player");
			m_players[id]->SignalAction().emit(Action_GameActionEnd);
		}
		else
		{
			DBG("Changing a non-Active Player");
		}
		delete m_players[id];
		m_players[id] = &player;
		m_players[id]->Attend(*this);
		m_players[id]->SignalAction().emit(Action_Start);
		if (active_flag)
		{
			EmitCurrentPlayerAction();
		}
	}
	else
	{
		DBG("Error, ChangePlayer in Stopped Game");
	}
}

sigc::signal1<void, Action> &Board::SignalAction()
{
	return m_signal_action;
}

const std::vector<Action> &Board::GetActionList() const
{
	return m_actions;
}

bool Board::isStoped() const
{
	return (m_current_action == Action_Stop);
}

void Board::_Action(const Action action)
{
	DBG("Board::_Action(%d, %d)", action.m_action, action.m_color);
	switch (action.m_action)
	{
		case Action_Move:
		case Action_Put:
		case Action_Take:
		case Action_Cover:
		{
			GameAction(action);
			break;
		}
		case Action_GameEnd:
		case Action_Stop:
		{
			Stop();
			break;
		}
		case Action_Retract:
		{
			Retract(action.m_color);
			break;
		}
		default:
		{
			OtherAction(action);
			break;
		}
	}
}

void Board::LoadHistory()
{
	m_current_player = ((m_players[0]->GetColor() == m_history.rbegin()->m_color) ? 0 : 1);
	m_current_action = m_history.rbegin()->m_action;
	RetractTo(m_history.rbegin()->m_board);
	for (int i = 0; i < PMax; ++i)
	{
		Slot_Change(i, GetSlot(i));
	}
}

void Board::ClearHistory()
{
	m_history_hash.clear();
	m_history.clear();
}

void Board::AddHistory()
{
	++m_history_hash[Hash()];
	m_history.push_back(Board_Stat(*this, m_current_action, m_players[m_current_player]->GetColor()));
}

void Board::CancelHistory(const int n)
{
	for (int i = 0; i < n; ++i)
	{
		--m_history_hash[m_history.rbegin()->m_board.Hash(m_history.rbegin()->m_color, m_history.rbegin()->m_action)];
		m_history.erase(m_history.end() - 1);
	}
}


void Board::EmitCurrentPlayerAction()
{
	m_players[m_current_player]->SignalAction().emit(m_current_action);
}

int Board::GameAction(const Action &action)
{
	DBG("Board::GameAction(Action(%d, %d, %d, %d))",
		action.m_action, action.m_color, action.m_p0, action.m_p1);
	if ((action.m_color != m_players[m_current_player]->GetColor()) || (action.m_action != m_current_action))
	{
		return Error_Move;
	}
	int ret = Board_Base::Do_Action(action.m_action, action.m_color, action.m_p0, action.m_p1);
	DBG("Board_Base::Do_Action = %d", ret);
	if (ret == Error_Move)
	{
		EmitCurrentPlayerAction();
		return Error_Move;
	}
	m_actions.push_back(action);
	switch (m_current_action)
	{
		case Action_Move:
		{
			if (ret == action.m_color)
			{
				m_current_action = Action_Take;
			}
			else
			{
				SwitchPlayer();
			}
			break;
		}
		case Action_Put:
		{
			if (ret == action.m_color)
			{
				m_current_action = Action_Cover;
			}
			else
			{
				if (Get_Chessman_Num(None) == 0)
				{
					Erase_X();
					m_current_action = Action_Move;
				}
				else
				{
					SwitchPlayer();
				}
			}
			break;
		}
		case Action_Take:
		{
			SwitchPlayer();
			m_current_action = Action_Move;
			break;
		}
		case Action_Cover:
		{
			m_current_action = Action_Put;
			if (Get_Chessman_Num(None) == 0)
			{
				Erase_X();
				m_current_action = Action_Move;
			}
			else
			{
				SwitchPlayer();
			}
			break;
		}
		default:
		{
			return Error_Move;
		}
	}

	AddHistory();
	if (m_history_hash[Hash()] >= 3)
	{
		DBG("Draw");
		GameEnd(None);
		return OK;
	}

	int ret_winner = CheckIsWin();
	if (ret_winner != OK)
	{
		DBG("Winner is %d", ret_winner);
		GameEnd((Color)ret_winner);
		return OK;
	}
	else
	{
		EmitCurrentPlayerAction();
	}
	return OK;
}

void Board::Stop()
{
	DBG("Board::Stop()");
	if (m_current_action != Action_Stop)
	{
		m_current_action = Action_Stop;
		m_players[0]->SignalAction().emit(Action_Stop);
		m_players[1]->SignalAction().emit(Action_Stop);
	}
}

void Board::Retract(const Color color)
{
	DBG("Board::Retract(%d)", color);
	m_players[m_current_player]->SignalAction().emit(Action_GameActionEnd);
	int k(1);
	bool flagfind(false);
	for (std::vector<Action>::reverse_iterator rit = m_actions.rbegin(); rit != m_actions.rend(); ++rit, ++k)
	{
		if (rit->m_color == color)
		{
			flagfind = true;
			break;
		}
	}
	if (flagfind == false)
	{
		DBG("Board::Retract -- NotFind");
		k = 0;
	}
	DBG("k is %d", k);
	for (int i = 0; i < k; ++i)
	{
		m_actions.erase(m_actions.end() - 1);
	}
	CancelHistory(k);

	LoadHistory();

	EmitCurrentPlayerAction();
}

void Board::OtherAction(const Action &action)
{
	DBG("Board::OtherAction");
}

int Board::CheckIsWin() const
{
	if (m_current_action == Action_Move)
	{
		if (Get_Chessman_Num(Black) < 3)
		{
			return White;
		}
		if (Get_Chessman_Num(White) < 3)
		{
			return Black;
		}
		return Check_HasMove(m_players[m_current_player]->GetColor());
	}
	return OK;
}

void Board::SwitchPlayer()
{
	DBG("Board::Switch_Player()");
	m_current_player ^= 1;
}

void Board::Slot_Change(const int p, const Color color)
{
}

void Board::SetSolt(const int p, const Color color)
{
	Board_Base::SetSolt(p, color);
	Slot_Change(p, color);
}

Player &Board::GetPlayer(const Color color)
{
	if (m_players[0]->GetColor() == color)
	{
		return *m_players[0];
	}
	else
	{
		return *m_players[1];
	}
}

Player &Board::GetCurrentPlayer()
{
	return *m_players[m_current_player];
}

Board_Base::Hash_Type Board::Hash() const
{
	return Board_Base::Hash(m_players[m_current_player]->GetColor(), m_current_action);
}

void Board::Erase_X()
{
	DBG("Board::Erase_X()");
	for (int i = 0; i < PMax; ++i)
	{
		if (Bit(XXX).isSet(i))
		{
			SetSolt(i, None);
		}
	}
}

void Board::GameEnd(const Color color)
{
	DBG("Board::GameEnd()");
	//m_current_action = Action_Stop;
	//SignalAction().emit(Action(Action_GameEnd, color));
	if (color == None)
	{
		m_players[0]->SignalAction().emit(Action_Draw);
		m_players[1]->SignalAction().emit(Action_Draw);
	}
	else
	{
		GetPlayer(color).SignalAction().emit(Action_Win);
		GetPlayer(Opp_Color(color)).SignalAction().emit(Action_Lose);
	}
//	Save();
}

void Board::Save(const Glib::ustring &filename) const
{
	DBG("Board::Save()");
	char buf[64];
	memset(buf, 0, sizeof(buf));
	xmlpp::Document document;
	xmlpp::Element *noderoot = document.create_root_node("ThreeThreeChess");
	xmlpp::Element *players = noderoot->add_child("Players");
	xmlpp::Element *actionlist = noderoot->add_child("ActionList");
	for (int i = 0; i < 2; ++i)
	{
		xmlpp::Element *player = players->add_child("Player");
		player->set_attribute("id", ((i == 0) ? "1" : "2"));
		xmlpp::Element *color = player->add_child("Color");
		color->set_child_text(ColorName(m_players[i]->GetColor()));
		xmlpp::Element *type = player->add_child("Type");
		type->set_child_text(PlayerTypeName(m_players[i]->GetType()));
		xmlpp::Element *time = player->add_child("Time");
		snprintf(buf, 64, "%d", m_players[i]->GetTime());
		time->set_child_text(buf);
	}
	int id(0);
	for (std::vector<Action>::const_iterator it = m_actions.begin(); it != m_actions.end(); ++it)
	{
		++id;
		xmlpp::Element *action = actionlist->add_child("Action");
		snprintf(buf, 64, "%d", id);
		action->set_attribute("id", buf);
		xmlpp::Element *op = action->add_child("Operator");
		op->set_child_text(ActionName(it->m_action));
		xmlpp::Element *color = action->add_child("Color");
		color->set_child_text(ColorName(it->m_color));
		xmlpp::Element *pos = action->add_child("Pos");
		snprintf(buf, 64, "%d", it->m_p0);
		pos->set_child_text(buf);
		if (it->m_action == Action_Move)
		{
			xmlpp::Element *expos = action->add_child("ExPos");
			snprintf(buf, 64, "%d", it->m_p1);
			expos->set_child_text(buf);
		}
	}
	DBG("	Save to %s", filename.c_str());
	document.write_to_file_formatted(filename.c_str());
}


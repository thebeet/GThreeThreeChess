/*
 * Board_AI.cpp
 *
 *  Created on: Mar 27, 2009
 *      Author: yonghu
 */

#include "Board_AI.h"

#include <map>
#include <vector>
#include <algorithm>

#include <cstring>

#include "Lock.h"

static int busy_signal(THREAD_START);
static Glib::StaticMutex mutex_busy_stop_signal;

#define If_Signal_Stop if (busy_signal == THREAD_STOP)
#define If_Signal_Not_Stop if (busy_signal != THREAD_STOP)

void Set_Busy_Signal_Start()
{
	Glib::Mutex::Lock signal_lock(mutex_busy_stop_signal);
	busy_signal = THREAD_START;
}

void Set_Busy_Signal_Stop()
{
	Glib::Mutex::Lock signal_lock(mutex_busy_stop_signal);
	busy_signal = THREAD_STOP;
}

static const int MagicNumber(27720);

bool comp_score(const int a, const int b)
{
	return (a >> 8) > (b >> 8);
}

Board_AI::Board_AI()
{
}

Board_AI::Board_AI(const Board_Base &board)
{
	memcpy(m_chessmans, board.m_chessmans, sizeof(m_chessmans));
}

Board_AI::Board_AI(const Board_AI &board)
{
	memcpy(m_chessmans, board.m_chessmans, sizeof(m_chessmans));
}

Board_AI::~Board_AI()
{

}

int Board_AI::CalcAction(const Color color, const ACTION action) const
{
	int prepos(-1);
	int pos(-1);
	{
		Glib::Mutex::Lock signal_lock(mutex_busy_stop_signal);
		int busy_signal_bak = busy_signal;
		busy_signal = THREAD_START;
		switch (action)
		{
			case Action_Move:
			{
				_CalcMove(color, 1, &prepos);
				break;
			}
			case Action_Put:
			{
				_CalcPut(color, 1, &prepos);
				break;
			}
			case Action_Take:
			{
				_CalcTake(color, 1, &prepos);
				break;
			}
			case Action_Cover:
			{
				_CalcTake_X(color, 1, &prepos);
				break;
			}
			default:
			{
				break;
			}
		}
		busy_signal = busy_signal_bak;
	}
	int preret(0);
	for (int trydepth = 1; ; ++trydepth)
	{
		int ret(0);
		DBG("TryDepth is %d", trydepth);
		int a(-HIGH), b(HIGH);
		if (trydepth > 1)
		{
			switch (action)
			{
				case Action_Move:
				case Action_Take:
				{
					a = (((trydepth & 1) == 1) ? preret - 1 : preret - MagicNumber - 1);
					b = (((trydepth & 1) == 0) ? preret + 1 : preret + MagicNumber + 1);
					break;
				}
				case Action_Put:
				case Action_Cover:
				{
					a = ((((trydepth & 1) == 0) ^ (color == White)) ? preret - 1 : preret - MagicNumber - 1);
					b = ((((trydepth & 1) == 1) ^ (color == White)) ? preret + 1 : preret + MagicNumber + 1);
					break;
				}
				default:
				{
					break;
				}
			}
		}
		for(;;)
		{
			DBG("Window is %d to %d", a, b);
			switch (action)
			{
				case Action_Move:
				{
					ret = _CalcMove(color, trydepth, &pos, a, b);
					break;
				}
				case Action_Put:
				{
					ret = _CalcPut(color, Get_Chessman_Num(None) + trydepth, &pos, a, b);
					break;
				}
				case Action_Take:
				{
					ret = _CalcTake(color, trydepth, &pos, a, b);
					break;
				}
				case Action_Cover:
				{
					ret = _CalcTake_X(color, Get_Chessman_Num(None) + 1 + trydepth, &pos, a, b);
					break;
				}
				default:
				{
					break;
				}
			}
			If_Signal_Stop
			{
				break;
			}
			if ((ret >= b) || (ret <= a))
			{
				DBG("Value is %d", ret);
				DBG("Reset Window");
				if (ret >= b)
				{
					a = b;
					b = HIGH;
				}
				else
				{
					b = a;
					a = -HIGH;
				}
			}
			else
			{
				break;
			}
		}
		preret = ret;
		DBG("Value is %d", ret);
		If_Signal_Stop
		{
			DBG("Return %d", prepos);
			return prepos;
		}
		else if (((ret >= b) || (ret <= a)) || ((ret >= WIN) || (ret <= LOSE)))
		{
			DBG("Got WIN or LOSE");
			return pos;
		}
		prepos = pos;
	}
}

int Board_AI::StaticValue(const Color color) const
{
	return (m_chessmans[color].Count() - m_chessmans[Opp_Color(color)].Count()) * MagicNumber;
}

int Board_AI::_CalcTake(const Color color, const int n, int *pos,
						const int alpha, const int beta) const
{
	int a(alpha), b(beta);
	int take_pos;
	int next_n(n - 1);
	Color opp_color(Opp_Color(color));
	Board_Bit opp_board(m_chessmans[opp_color]);
	while (opp_board.FindEach(take_pos))
	{
		Board_AI next(*this);
		next.Do_Action(Action_Take, color, take_pos);
		int nextscore = -next._CalcMove(opp_color, next_n, NULL, -b, -a);
		If_Signal_Stop return a;
		if (nextscore >= b)
		{
			return b;
		}
		if (nextscore > a)
		{
			if (pos != NULL)
			{
				*pos = take_pos;
			}
			a = nextscore;
		}
	}
	return a;
}

int Board_AI::_CalcMove(const Color color, const int n, int *pos,
						const int alpha, const int beta) const
{
	if (m_chessmans[color].Count() < 3)
		return LOSE - n;
	if (n == 0)
	{
		return StaticValue(color);
	}
	else
	{
		int next_n(n - 1);
		Color opp_color = Opp_Color(color);
		int a(alpha), b(beta);
		bool flag(false);
		std::vector< std::pair<Board_AI, int> > board_store;
		Board_Bit tar_board(m_chessmans[None]);
		int tar_pos;
		while (tar_board.FindEach(tar_pos))
		{
			Board_Bit from_board(Adj_Board_Bits[tar_pos] & m_chessmans[color]);
			int from_pos;
			while (from_board.FindEach(from_pos))
			{
				flag = true;
				Board_AI next(*this);
				int ret = next.Do_Action(Action_Move, color, from_pos, tar_pos);
				if (ret == color)
				{
					int nextscore = next._CalcTake(color, n, NULL, a, b);
					If_Signal_Stop return a;
					if (nextscore >= b)
					{
						return b;
					}
					if (nextscore > a)
					{
						if (pos != NULL)
						{
							*pos = (tar_pos << 8) | from_pos;
						}
						a = nextscore;
					}
				}
				else
				{
					board_store.push_back(std::make_pair(next, (tar_pos << 8) | from_pos));
				}
			}
		}
		for (std::vector<std::pair<Board_AI, int> >::iterator it = board_store.begin(); it != board_store.end(); ++it)
		{
			int nextscore = -(it->first._CalcMove(opp_color, next_n, NULL, -b, -a));
			If_Signal_Stop return a;
			if (nextscore >= b)
			{
				return b;
			}
			if (nextscore > a)
			{
				if (pos != NULL)
				{
					*pos = it->second;
				}
				a = nextscore;
			}
		}
		if (flag == false)
		{
			return LOSE - n;
		}
		return a;
	}
}

int Board_AI::_CalcPut(const Color color, const int n, int *put_pos,
					   const int alpha, const int beta) const
{
	if (m_chessmans[None].Count() == 0)
	{
		Board_AI next(*this);
		next.Erase_X();
		int ret = -next._CalcMove(Opp_Color(color), n, NULL, -beta, -alpha);
		return ret;
	}
	if (n == 0)
	{
		return StaticValue(color);
	}
	else
	{
		int score[PMax];
		memset(score, 0, sizeof(score));
		if (m_chessmans[None].Count() > 2)
		{
			/*
			 * Calc Score Begin
			 */
			int board[PMax];
			for (int i = 0; i < PMax; ++i)
			{
				board[i] = GetSlot(i);
			}
			for (int i = 0; i < PMax; ++i)
			{
				if (m_chessmans[None].isSet(i))
				{
					score[i] += 1;
					if ((i % 3) == 1)
					{
						score[i] += 1;
					}
				}
				else if (m_chessmans[XXX].isSet(i))
				{
					Board_Bit bonus(Adj_Board_Bits[i] & m_chessmans[None]);
					int p;
					while (bonus.FindEach(p))
					{
						score[p] += 10;
					}
				}
			}
			for (int i = 0; i < TotalLine; ++i)
			{
				int kind[4];
				memset(kind, 0, sizeof(kind));
				++kind[board[Lines_Point[i][0]]];
				++kind[board[Lines_Point[i][1]]];
				++kind[board[Lines_Point[i][2]]];
				int s = CalcPut_Line(color, kind);
				Board_Bit bonus(Lines_Board_Bits[i] & m_chessmans[None]);
				int p;
				while (bonus.FindEach(p))
				{
					score[p] += s;
				}
			}
			/*
			 * Calc Score End
			 */
		}
		else
		{
			Board_Bit none(m_chessmans[None]);
			int p;
			while (none.FindEach(p))
			{
				score[p] = 1;
			}
		}

		Color opp_color = Opp_Color(color);
		std::vector<int> scores;
		for (int i = 0; i < PMax; ++i)
		{
			if (score[i] > 0)
			{
				scores.push_back((score[i] << 8) | i);
			}
		}
		std::sort(scores.begin(), scores.end(), comp_score);

		int next_n(n - 1);
		int a(alpha), b(beta);
		for (std::vector<int>::iterator it = scores.begin(); it != scores.end(); ++it)
		{
			Board_AI next(*this);
			int pos = (*it) & 0xff;
			int ret = next.Do_Action(Action_Put, color, pos);
			if (ret == color)
			{
				int nextscore = next._CalcTake_X(color, n, NULL, a, b);
				If_Signal_Stop return a;
				if (nextscore >= b)
				{
					return b;
				}
				if (nextscore > a)
				{
					if (put_pos != NULL)
					{
						*put_pos = pos;
					}
					a = nextscore;
				}
			}
			else
			{
				int nextscore = -next._CalcPut(opp_color, next_n, NULL, -b, -a);
				If_Signal_Stop return a;
				if (nextscore >= b)
				{
					return b;
				}
				if (nextscore > a)
				{
					if (put_pos != NULL)
					{
						*put_pos = pos;
					}
					a = nextscore;
				}
			}
		}
		return a;
	}
}

int Board_AI::CalcPut_Line(const Color color, int kind[4])
{
	if (kind[0] == 3) return 5;
	if ((kind[0] == 1) && (kind[Opp_Color(color)] == 1) && (kind[XXX] == 1)) return 10;
	if ((kind[0] == 1) && (kind[color] == 1) && (kind[XXX] == 1)) return 15;
	if ((kind[0] == 2) && (kind[Opp_Color(color)] == 1)) return 10;
	if ((kind[0] == 2) && (kind[XXX] == 1)) return 5;
	if ((kind[0] == 2) && (kind[color] == 1)) return 30;
	if ((kind[0] == 1) && (kind[Opp_Color(color)] == 2)) return 50;
	if ((kind[0] == 1) && (kind[color] == 2)) return 150;
	return 0;
}

int Board_AI::_CalcTake_X(const Color color, const int n, int *take_pos,
					   const int alpha, const int beta) const
{
	/*
	 * Calc Score Begin
	 */
	const Color oppcolor(Opp_Color(color));
	int board[PMax];
	int adj_score[4];
	adj_score[None] = -1;
	adj_score[color] = 10;
	adj_score[Opp_Color(color)] = -10;
	adj_score[XXX] = 0;
	for (int i = 0; i < PMax; ++i)
	{
		board[i] = GetSlot(i);
	}
	int score[PMax];
	memset(score, 0, sizeof(score));
	for (int i = 0; i < PMax; ++i)
	{
		if (m_chessmans[oppcolor].isSet(i))
		{
			score[i] += 100;
			Board_Bit bonus(Adj_Board_Bits[i]);
			int p;
			while (bonus.FindEach(p))
			{
				score[i] += adj_score[board[p]];
			}
		}
	}
	for (int i = 0; i < TotalLine; ++i)
	{
		int kind[4];
		memset(kind, 0, sizeof(kind));
		++kind[board[Lines_Point[i][0]]];
		++kind[board[Lines_Point[i][1]]];
		++kind[board[Lines_Point[i][2]]];
		int s = CalcTake_X_Line(color, kind);
		Board_Bit bonus(Lines_Board_Bits[i] & m_chessmans[oppcolor]);
		int p;
		while (bonus.FindEach(p))
		{
			score[p] += s;
		}
	}
	/*
	 * Calc Score End
	 */
	Color opp_color = Opp_Color(color);
	std::vector<int> scores;
	for (int i = 0; i < PMax; ++i)
	{
		if (score[i] > 0)
		{
			scores.push_back((score[i] << 8) | i);
		}
	}
	std::sort(scores.begin(), scores.end(), comp_score);

	int next_n(n - 1);
	int a(alpha), b(beta);
	for (std::vector<int>::iterator it = scores.begin(); it != scores.end(); ++it)
	{
		Board_AI next(*this);
		int pos = (*it) & 0xff;
		next.Do_Action(Action_Cover, color, pos);
		int nextscore = -next._CalcPut(opp_color, next_n, NULL, -b, -a);
		If_Signal_Stop return a;
		if (nextscore >= b)
		{
			return b;
		}
		if (nextscore > a)
		{
			if (take_pos != NULL)
			{
				*take_pos = pos;
			}
			a = nextscore;
		}
	}
	return a;
}

int Board_AI::CalcTake_X_Line(const Color color, int kind[4])
{
	if ((kind[0] == 2) && (kind[Opp_Color(color)] == 1)) return 5;
	if ((kind[0] == 1) && (kind[color] == 1) && (kind[Opp_Color(color)] == 1)) return 5;
	if ((kind[0] == 0) && (kind[color] == 1) && (kind[Opp_Color(color)] == 2)) return 2;
	if ((kind[0] == 1) && (kind[Opp_Color(color)] == 2)) return 35;
	if ((kind[0] == 0) && (kind[color] == 2) && (kind[Opp_Color(color)] == 1)) return 20;
	if (kind[Opp_Color(color)] == 3) return -5;
	return 0;
}

int Board_AI::Find_Max_Score(const int score[PMax])
{
	int high(-100);
	int pt(0);
	for (int i = 0; i < PMax; ++i)
	{
		if (high <= score[i])
		{
			if (high == score[i])
			{
				int r = rand() % (i + 5);
				if (r != 0)
				{
					continue;
				}
			}
			high = score[i];
			pt = i;
		}
	}
	return pt;
}

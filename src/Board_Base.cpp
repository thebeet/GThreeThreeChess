#include "Board_Base.h"

#include "Userful_Board_Bits.h"

#include <cstring>

Board_Base::Board_Base()
{
	memset(m_chessmans, 0, sizeof(m_chessmans));
	m_chessmans[None] = ~m_chessmans[None];
}

Board_Base::Board_Base(const Board_Base& board)
{
	m_chessmans[None] = board.m_chessmans[None];
	m_chessmans[Black] = board.m_chessmans[Black];
	m_chessmans[White] = board.m_chessmans[White];
	m_chessmans[XXX] = board.m_chessmans[XXX];
}

Board_Base::~Board_Base()
{
}

void Board_Base::Clear()
{
	for (int i = 0; i < PMax; ++i)
	{
		SetSolt(i, None);
	}
}

Board_Base::Hash_Type Board_Base::Hash(const Color m_color, const ACTION action) const
{
	return (((Hash_Type)m_chessmans[Black].m_context | (Hash_Type)m_chessmans[XXX].m_context) |
			(((Hash_Type)m_chessmans[White].m_context | (Hash_Type)m_chessmans[XXX].m_context) << 24) |
			((Hash_Type)m_color << 48) |
			((Hash_Type)action << 50));
}

int Board_Base::Do_Action(const int action, const Color color, const int p0, const int p1)
{
	if ((p0 >= PMax) || (p0 < 0))
	{
		return Error_Move;
	}
	switch (action)
	{
		case Action_Move:
		{
			if ((p1 >= PMax) || (p1 < 0))
			{
				return Error_Move;
			}
			return Move(p0, p1, color);
		}
		case Action_Put:
		{
			return Put(p0, color);
		}
		case Action_Take:
		{
			return Take(p0, color);
		}
		case Action_Cover:
		{
			return Take_X(p0, color);
		}
		default:
		{
			return Error_Move;
		}
	}
}

Color Board_Base::GetSlot(const int p) const
{
	if (m_chessmans[None].isSet(p)) return None;
	if (m_chessmans[Black].isSet(p)) return Black;
	if (m_chessmans[White].isSet(p)) return White;
	return XXX;
}

const Board_Bit &Board_Base::Bit(const Color color) const
{
	return m_chessmans[color];
}

void Board_Base::SetSolt(const int p, const Color color)
{
	m_chessmans[None].UnSet(p);
	m_chessmans[Black].UnSet(p);
	m_chessmans[White].UnSet(p);
	m_chessmans[XXX].UnSet(p);
	m_chessmans[color].Set(p);
}

void Board_Base::Erase_X()
{
	m_chessmans[None] |= m_chessmans[XXX];
	m_chessmans[XXX].m_context = 0;
}

void Board_Base::RetractTo(const Board_Base& board)
{
	m_chessmans[None] = board.m_chessmans[None];
	m_chessmans[Black] = board.m_chessmans[Black];
	m_chessmans[White] = board.m_chessmans[White];
	m_chessmans[XXX] = board.m_chessmans[XXX];
}

int Board_Base::Get_Chessman_Num(const Color color) const
{
	return m_chessmans[color].Count();
}

void Board_Base::Slot_Change(const int p, const Color color)
{
}

int Board_Base::Check_IsThree(const int p, const Color color) const
{
	for (int i = 0; i < Point_Line_Num[p]; ++i)
	{
		if (m_chessmans[color].Cover(Point_Lines_Board_Bits[p][i]))
		{
			return color;
		}
	}
	return OK;
}

int Board_Base::Check_HasMove(const Color color) const
{
	for (int i = 0; i < PMax; ++i)
	{
		if (m_chessmans[color].isSet(i))
		{
			if ((m_chessmans[None] & Adj_Board_Bits[i]) > 0)
			{
				return OK;
			}
		}
	}
	return Opp_Color(color);
}

int Board_Base::Move(const int p0, const int p1, const Color color)
{
	if (m_chessmans[color].isSet(p0) && m_chessmans[None].isSet(p1)
		&& Adj_Board_Bits[p0].isSet(p1))
	{
		SetSolt(p0, None);
		SetSolt(p1, color);
		return Check_IsThree(p1, color);
	}
	else
	{
		return Error_Move;
	}
}

int Board_Base::Put(const int p, const Color color)
{
	if (m_chessmans[None].isSet(p))
	{
		SetSolt(p, color);
		return Check_IsThree(p, color);
	}
	else
	{
		return Error_Move;
	}
}

int Board_Base::Take(const int p, const Color color)
{
	if (m_chessmans[Opp_Color(color)].isSet(p))
	{
		SetSolt(p, None);
		return OK;
	}
	else
	{
		return Error_Move;
	}
}

int Board_Base::Take_X(const int p, const Color color)
{
	if (m_chessmans[Opp_Color(color)].isSet(p))
	{
		SetSolt(p, XXX);
		return OK;
	}
	else
	{
		return Error_Move;
	}
}

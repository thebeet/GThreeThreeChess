/*
 * Board_Bit.cpp
 *
 *  Created on: Mar 18, 2009
 *      Author: yonghu
 */

#include "Board_Bit.h"

#include "Userful_Board_Bits.h"

Board_Bit::Board_Bit() :
	m_context(0)
{
}

Board_Bit::Board_Bit(const int context) :
	m_context(context)
{

}

Board_Bit::Board_Bit(const Board_Bit &board) :
	m_context(board.m_context)
{

}

Board_Bit::~Board_Bit()
{
}

bool Board_Bit::isSet(const int p) const
{
	return (m_context & (1 << p));
}


void Board_Bit::Set(const int p)
{
	m_context |= (1 << p);
}

void Board_Bit::UnSet(const int p)
{
	m_context &= (~(1 << p));
}

void Board_Bit::Xor(const int p)
{
	m_context ^= (1 << p);
}

void Board_Bit::Move(const int p0, const int p1)
{
	Xor(p0);
	Xor(p1);
}

int Board_Bit::Count() const
{
	return Board_Count[m_context & 0xFFF] + Board_Count[m_context >> 12];
}

int Board_Bit::Count_2() const
{
	int ret;
	ret = (m_context & 0x55555555L) + ((m_context >> 1) & 0x55555555L);
	ret = (ret & 0x33333333L) + ((ret >> 2) & 0x33333333L);
	ret = (ret & 0x0F0F0F0FL) + ((ret >> 4) & 0x0F0F0F0FL);
	ret = (ret & 0x00FF00FFL) + ((ret >> 8) & 0x00FF00FFL);
	ret = (ret & 0x0000FFFFL) + ((ret >> 16) & 0x0000FFFFL);
	return ret;
}

bool Board_Bit::Cover(const Board_Bit &board) const
{
	return ((m_context & board.m_context) == board.m_context);
}

bool Board_Bit::FindEach(int &i)
{
	if (m_context)
	{
		int last = m_context & (-m_context);
		//i = Pos.at(last);
		i = Pos[last];
		m_context ^= last;
		return true;
	}
	else
	{
		return false;
	}
}

bool Board_Bit::ForEach(int &i)
{
	while (isSet(i) == false)
	{
		if ((++i) == PMax) break;
	}
	if (i < PMax)
	{
		UnSet(i);
		return true;
	}
	return false;
}


Board_Bit &Board_Bit::operator ^=(const Board_Bit &board)
{
	m_context ^= board.m_context;
	return *this;
}

Board_Bit &Board_Bit::operator &=(const Board_Bit &board)
{
	m_context &= board.m_context;
	return *this;
}

Board_Bit &Board_Bit::operator |=(const Board_Bit &board)
{
	m_context |= board.m_context;
	return *this;
}

Board_Bit Board_Bit::operator ^(const Board_Bit &board) const
{
	Board_Bit ret(*this);
	ret ^= board;
	return ret;
}

Board_Bit Board_Bit::operator &(const Board_Bit &board) const
{
	Board_Bit ret(*this);
	ret &= board;
	return ret;
}

Board_Bit Board_Bit::operator |(const Board_Bit &board) const
{
	Board_Bit ret(*this);
	ret |= board;
	return ret;
}


Board_Bit Board_Bit::operator ~() const
{
	Board_Bit ret(*this);
	ret.m_context = (~ret.m_context) & MASK;
	return ret;
}

Board_Bit::operator int() const
{
	return m_context;
}

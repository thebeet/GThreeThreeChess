/*
 * Board_Bit.h
 *
 *  Created on: Mar 18, 2009
 *      Author: yonghu
 */

#ifndef BOARD_BIT_H_
#define BOARD_BIT_H_

#include "common.h"
#include "Color.h"
#include "Action.h"

class Board_Base;
class Board_AI;

class Board_Bit
{
	friend class Board_Base;
	friend class Board_AI;
public:
	Board_Bit();
	Board_Bit(const int context);
	Board_Bit(const Board_Bit &board);
	~Board_Bit();

	bool isSet(const int p) const;

	void Set(const int p);
	void UnSet(const int p);
	void Xor(const int p);
	void Move(const int p0, const int p1);

	int Count() const;
	int Count_2() const;

	bool Cover(const Board_Bit &board) const;

	bool FindEach(int &i);
	bool ForEach(int &i);

	Board_Bit &operator ^=(const Board_Bit &board);
	Board_Bit &operator &=(const Board_Bit &board);
	Board_Bit &operator |=(const Board_Bit &board);
	Board_Bit operator ^(const Board_Bit &board) const;
	Board_Bit operator &(const Board_Bit &board) const;
	Board_Bit operator |(const Board_Bit &board) const;
	Board_Bit operator ~() const;

	operator int() const;

private:
	int m_context;
	static const int MASK = 0xffffff;
};

#endif /* BOARD_BIT_H_ */

#ifndef BOARD_BASE_H_
#define BOARD_BASE_H_

#include "Color.h"
#include "Action.h"

#include "common.h"

#include "Board_Bit.h"

class Board_AI;

class Board_Base
{
	friend class Board_AI;
public:
	typedef long long Hash_Type;
	Board_Base();
	Board_Base(const Board_Base& board);
	virtual ~Board_Base();
	void Clear();
	Color GetSlot(const int p) const;
	const Board_Bit &Bit(const Color color) const;
	Hash_Type Hash(const Color color = None, const ACTION action = Action_None) const;
	int Do_Action(const int action, const Color color, const int p0, const int p1 = 0);

protected:
	void Erase_X();
	void RetractTo(const Board_Base& board);
	int Get_Chessman_Num(const Color color) const;
	virtual void Slot_Change(const int p, const Color color);
	int Check_HasMove(const Color color) const;
	virtual void SetSolt(const int p, const Color color);

private:
	int Move(const int p0, const int p1, const Color color);
	int Put(const int p, const Color color);
	int Take(const int p, const Color Color);
	int Take_X(const int p, const Color Color);

	int Check_IsThree(const int p, const Color color) const;

	Board_Bit m_chessmans[4];

};


#endif /* BOARD_BASE_H_ */

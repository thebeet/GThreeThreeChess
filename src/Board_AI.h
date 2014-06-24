/*
 * Board_AI.h
 *
 *  Created on: Mar 27, 2009
 *      Author: yonghu
 */

#ifndef BOARD_AI_H_
#define BOARD_AI_H_

#include "Color.h"
#include "common.h"
#include "Board_Base.h"
#include "Userful_Board_Bits.h"

void Set_Busy_Signal_Start();
void Set_Busy_Signal_Stop();

class Board_AI : public Board_Base
{
public:
	Board_AI();
	Board_AI(const Board_AI &board);
	Board_AI(const Board_Base &board);
	virtual ~Board_AI();
	int CalcAction(const Color color, const ACTION action) const;
	std::pair<int, int> CalcMove(const Color color, const int depth) const;
	int CalcPut(const Color color) const;
	int CalcTake_X(const Color color) const;

	int StaticValue(const Color color) const;

private:
	int _CalcMove(const Color color, const int n, int *pos = NULL,
				  const int alpha = -HIGH, const int beta = HIGH) const;
	int _CalcTake(const Color color, const int n, int *pos = NULL,
				  const int alpha = -HIGH, const int beta = HIGH) const;
	int _CalcTake_X(const Color color, const int n, int *pos = NULL,
				  const int alpha = -HIGH, const int beta = HIGH) const;
	int _CalcPut(const Color color, const int n, int *pos = NULL,
				  const int alpha = -HIGH, const int beta = HIGH) const;
	static int Find_Max_Score(const int score[PMax]);
	static int CalcPut_Line(const Color color, int kind[4]);
	static int CalcTake_X_Line(const Color color, int kind[4]);

	static const int WIN = 100000000;
	static const int LOSE = -WIN;
	static const int HIGH = WIN * 4;
};


#endif /* BOARD_AI_H_ */

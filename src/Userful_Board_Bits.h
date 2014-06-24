/*
 * Userful_Board_Bits.h
 *
 *  Created on: Mar 19, 2009
 *      Author: yonghu
 */

#ifndef USERFUL_BOARD_BITS_H_
#define USERFUL_BOARD_BITS_H_

#include <map>

extern bool isInit;
extern Board_Bit Adj_Board_Bits[PMax];
extern Board_Bit Lines_Board_Bits[TotalLine];
extern int Lines_Point[TotalLine][4];
extern Board_Bit Point_Lines_Board_Bits[PMax][4];
extern int Point_Line_Num[PMax];
extern std::map<int, int> Pos;

extern int Board_Count[1 << (PMax / 2)];

extern void Userful_Board_Bits_Init();


#endif /* USERFUL_BOARD_BITS_H_ */

/*
 * Userful_Board_Bits.cpp
 *
 *  Created on: Mar 19, 2009
 *      Author: yonghu
 */

#include "common.h"
#include "Board_Bit.h"

#include <map>

bool isInit(false);
Board_Bit Adj_Board_Bits[PMax];
Board_Bit Lines_Board_Bits[TotalLine];
int Lines_Point[TotalLine][4];
Board_Bit Point_Lines_Board_Bits[PMax][4];
int Point_Line_Num[PMax];
std::map<int, int> Pos;
int Board_Count[1 << (PMax / 2)];

static void Board_Count_Init()
{
	int total = (1 << (PMax / 2));
	for (int i = 0; i < total; ++i)
	{
		Board_Count[i] = Board_Bit(i).Count_2();
	}
}

static int NextRow(const int p)
{
	return (p + nyy) % PMax;
}

static int PreRow(const int p)
{
	return (p + PMax - nyy) % PMax;
}

static void Adj_Board_Bits_Init()
{
	for (int i = 0; i < PMax; ++i)
	{
		Adj_Board_Bits[i].Set(NextRow(i));
		Adj_Board_Bits[i].Set(PreRow(i));
		if ((i % nyy) != 2)
		{
			Adj_Board_Bits[i].Set(i + 1);
		}
		if ((i % nyy) != 0)
		{
			Adj_Board_Bits[i].Set(i - 1);
		}
	}
}

static void Lines_Board_Bits_Init()
{
	int k(0);
	for (int i = 0; i < PMax; i += 3)
	{
		for (int j = i; j < i + 3; ++j)
		{
			Lines_Board_Bits[k].Set(j);
			Lines_Point[k][j - i] = j;
		}
		for (int j = i; j < i + 3; ++j)
		{
			Point_Lines_Board_Bits[j][Point_Line_Num[j]++] = Lines_Board_Bits[k];
		}
		++k;
	}
	for (int i = 0; i < nxx; i += 2)
	{
		for (int j = 0; j < 3; ++j)
		{
			int p = i * nyy + j;
			Lines_Board_Bits[k].Set(PreRow(p));
			Lines_Board_Bits[k].Set(p);
			Lines_Board_Bits[k].Set(NextRow(p));
			Lines_Point[k][0] = PreRow(p);
			Lines_Point[k][1] = p;
			Lines_Point[k][2] = NextRow(p);
			Point_Lines_Board_Bits[PreRow(p)][Point_Line_Num[PreRow(p)]++] = Lines_Board_Bits[k];
			Point_Lines_Board_Bits[p][Point_Line_Num[p]++] = Lines_Board_Bits[k];
			Point_Lines_Board_Bits[NextRow(p)][Point_Line_Num[NextRow(p)]++] = Lines_Board_Bits[k];
			++k;
		}
	}
}

static void Userful_Pos_Init()
{
	Pos.clear();
	for (int i = 0; i < PMax; ++i)
	{
		Pos[1 << i] = i;
	}
}

void Userful_Board_Bits_Init()
{
	if (isInit == false)
	{
		Adj_Board_Bits_Init();
		Lines_Board_Bits_Init();
		Userful_Pos_Init();
		Board_Count_Init();
		isInit = true;
	}
}

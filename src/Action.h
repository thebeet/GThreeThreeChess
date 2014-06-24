/*
 * Action.h
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#ifndef ACTION_H_
#define ACTION_H_

#include "Color.h"

enum ACTION
{
	Action_None = 0,
	Action_Move = 1,
	Action_Put = 2,
	Action_Take = 3,
	Action_Cover = 4,
	Action_GameActionEnd = 5,

	Action_RequestControl = 6,
	Action_CloseControl = 7,
	Action_SlotSelect = 8,
	Action_SlotUnSelect = 9,
	Action_Retract = 10,

	Action_TimeOver = 32,
	Action_Win = 64,
	Action_Lose = 65,
	Action_Draw = 66,
	Action_GameEnd = 67,

	Action_Start = 125,
	Action_Stop = 126,
	Action_Exit = 127,
};

const char *ActionName(const ACTION action);
ACTION ActionReflect(const char *actionname);

class Action
{
public:
	Action();
	Action(const ACTION action, const Color color, const int p0 = -1, const int p1 = -1);
	~Action();
	ACTION m_action;
	Color m_color;
	int m_p0;
	int m_p1;
};

#endif /* ACTION_H_ */

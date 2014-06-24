/*
 * Action.cpp
 *
 *  Created on: 2009-3-5
 *      Author: Administrator
 */

#include "Action.h"

#include <cstring>

const char *ActionName(const ACTION action)
{
	switch (action)
	{
		case Action_None:
		{
			return "None";
			break;
		}
		case Action_Move:
		{
			return "Move";
			break;
		}
		case Action_Put:
		{
			return "Put";
			break;
		}
		case Action_Take:
		{
			return "Take";
			break;
		}
		case Action_Cover:
		{
			return "Cover";
			break;
		}
		case Action_GameActionEnd:
		{
			return "GameActionEnd";
			break;
		}
		case Action_RequestControl:
		{
			return "RequestControl";
			break;
		}
		case Action_CloseControl:
		{
			return "CloseControl";
			break;
		}
		case Action_SlotSelect:
		{
			return "SlotSelect";
			break;
		}
		case Action_SlotUnSelect:
		{
			return "SlotUnSelect";
			break;
		}
		case Action_Retract:
		{
			return "Retract";
			break;
		}
		case Action_TimeOver:
		{
			return "TimeOver";
			break;
		}
		case Action_Win:
		{
			return "Win";
			break;
		}
		case Action_Lose:
		{
			return "Lose";
			break;
		}
		case Action_Draw:
		{
			return "Draw";
			break;
		}
		case Action_GameEnd:
		{
			return "GameEnd";
			break;
		}
		case Action_Start:
		{
			return "Start";
			break;
		}
		case Action_Stop:
		{
			return "Stop";
			break;
		}
		case Action_Exit:
		{
			return "Exit";
			break;
		}
		default:
		{
			return "";
			break;
		}
	}
}

ACTION ActionReflect(const char *actionname)
{
	if (strncmp(actionname, "Move", 16) == 0)
	{
		return Action_Move;
	}
	if (strncmp(actionname, "Put", 16) == 0)
	{
		return Action_Put;
	}
	if (strncmp(actionname, "Take", 16) == 0)
	{
		return Action_Take;
	}
	if (strncmp(actionname, "Cover", 16) == 0)
	{
		return Action_Cover;
	}
	return Action_None;
}

Action::Action(const ACTION action, const Color color,
			   const int p0, const int p1) :
	m_action(action), m_color(color),
	m_p0(p0), m_p1(p1)
{
}

Action::Action()
{

}

Action::~Action()
{
}

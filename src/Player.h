/*
 * Player_Control.h
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "Board.h"
#include "Action.h"

#include <sigc++/signal.h>
#include <sigc++/connection.h>

enum PlayerType
{
	PlayerNone = 0,
	PlayerHuman = 1,
	PlayerComputer = 2,
};

const char *PlayerTypeName(const PlayerType type);

class Board;

class Player
{
public:
	Player();
	virtual ~Player();
	PlayerType GetType() const;
	void SetColor(const Color color);
	Color GetColor() const;

	void Attend(Board &board);
	void SetTime(const int use_time);
	int GetTime() const;
	bool GetActive() const;
	bool GetStarted() const;
	const Board &GetBoard() const;
	Board &GetBoard();

	sigc::signal1<void, ACTION> &SignalAction();


protected:
	virtual void GameAction(const ACTION action);
	virtual void GameActionEnd();
	virtual void Retract();
	virtual void TimeOver();
	virtual void Start();
	virtual void Stop();
	virtual void OtherAction(const ACTION action);

	void StartTimer();
	void CloseTimer();

	void Do_Action(const int p0, const int p1);
	ACTION m_current_action;
	Board *m_board;
	sigc::signal1<void, ACTION> m_signal_action;
	PlayerType m_type;

private:
	void _Action(const ACTION action);
	bool _TimeOver();

	sigc::connection m_timer_conn;

	int m_use_time;
	bool m_active;
	bool m_started;
	Color m_color;
};

#endif /* PLAYER_H_ */

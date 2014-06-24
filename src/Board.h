/*
 * Board.h
 *
 *  Created on: Feb 18, 2009
 *      Author: yonghu
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include <map>

#include "Board_Base.h"
#include "Player.h"
#include "Action.h"
#include <glibmm/thread.h>

class Player;

class Board_Stat
{
public:
	Board_Stat(const Board_Base &board, const ACTION action, const Color color);
	Board_Base m_board;
	ACTION m_action;
	Color m_color;
};

class Board : public Board_Base
{
public:
	Board();
	virtual ~Board();

	void Start(Player &player1, Player &player2);

	void ChangePlayer(Player &player, Color color);

	void Save(const Glib::ustring &filename) const;

	sigc::signal1<void, Action> &SignalAction();

	const std::vector<Action> &GetActionList() const;

	bool isStoped() const;

protected:
	virtual void OtherAction(const Action &action);
	virtual void Slot_Change(const int p, const Color color);
	void SetSolt(const int p, const Color color);
	Player &GetPlayer(const Color color);
	Player &GetCurrentPlayer();
	Board_Base::Hash_Type Hash() const;

private:
	void _Action(const Action action);
	int GameAction(const Action &action);
	void Stop();
	void Retract(const Color color);
	void EmitCurrentPlayerAction();
	int CheckIsWin() const;
	void SwitchPlayer();
	void Erase_X();
	void GameEnd(const Color color);

	void LoadHistory();
	void ClearHistory();
	void AddHistory();
	void CancelHistory(const int n);

	sigc::signal1<void, Action> m_signal_action;

//Game Stat
	std::vector< Player* > m_players;
	int m_current_player;
	ACTION m_current_action;
	std::vector<Action> m_actions;
	std::vector< Board_Stat > m_history;
	std::map<Board_Base::Hash_Type, int> m_history_hash;
};

#endif /* BOARD_H_ */

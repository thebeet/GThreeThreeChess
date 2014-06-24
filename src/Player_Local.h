/*
 * Player_Local.h
 *
 *  Created on: Mar 3, 2009
 *      Author: yonghu
 */

#ifndef PLAYER_LOCAL_H_
#define PLAYER_LOCAL_H_

#include "Player.h"

class Player_Local : public Player
{
public:
	Player_Local();
	virtual ~Player_Local();
	void Click(const int p);

protected:
	virtual void Stop();
	virtual void GameActionEnd();
	virtual void GameAction(const ACTION action);

private:
	int Do_Action(const int p0, const int p1 = -1);
	int m_select_pos;
};

/*
class Player_Local_with_AI : public Player_Local
{
public:
	Player_Local_with_AI();
	virtual ~Player_Local_with_AI();
	virtual void Turn(const ACTION action);
private:
	virtual void AI_Hint(Board_AI &ai_board, const ACTION action);
	int m_action_tmp;
};
*/
#endif /* PLAYER_LOCAL_H_ */

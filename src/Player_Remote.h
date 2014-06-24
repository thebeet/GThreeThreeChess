/*
 * Player_Remote.h
 *
 *  Created on: 2009-5-14
 *      Author: TheBeet
 */

#ifndef PLAYER_REMOTE_H_
#define PLAYER_REMOTE_H_

#include "Player.h"

class Player_Remote : public Player
{
public:
	Player_Remote();
	virtual ~Player_Remote();
	virtual void Stop();
};

#endif /* PLAYER_REMOTE_H_ */

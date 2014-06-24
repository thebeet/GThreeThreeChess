/*
 * Slot.h
 *
 *  Created on: Feb 20, 2009
 *      Author: yonghu
 */

#ifndef BOARD_SLOT_WIDGET_H_
#define BOARD_SLOT_WIDGET_H_

#include <gtkmm/image.h>
#include <gtkmm/box.h>

#include "common.h"

class Board_Slot_Widget : public Gtk::Image
{
public:
	Board_Slot_Widget();
	Board_Slot_Widget(const Board_Slot_Widget &slot);
	virtual ~Board_Slot_Widget();

	void Change_Status(const int new_status);
	void High_Light();

private:
	void Init();
	int m_status;
};

#endif /* BOARD_SLOT_WIDGET_H_ */

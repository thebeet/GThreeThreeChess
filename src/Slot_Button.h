/*
 * Slot.h
 *
 *  Created on: Feb 20, 2009
 *      Author: yonghu
 */

#ifndef SLOT_BUTTON_H_
#define SLOT_BUTTON_H_

#include <gtkmm.h>

#include "common.h"

class Slot_Button : public Gtk::Button
{
public:
	Slot_Button();
	Slot_Button(const Slot_Button &slot);
	virtual ~Slot_Button();

	void Change_Status(const int new_status);
	void High_Light();

private:
	void Init();
	Gtk::VBox m_vbox;
	Gtk::Image m_image;
	int m_status;
};

#endif /* SLOT_BUTTON_H_ */

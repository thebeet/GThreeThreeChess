/*
 * Board_Gui.h
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#ifndef BOARD_WIDGET_H_
#define BOARD_WIDGET_H_

#include <gtkmm/box.h>
#include <gtkmm/table.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#include <vector>

#include "common.h"

#include "Board.h"
#include "Board_Slot_Widget.h"

class Board_BackGround : public Gtk::Image
{
public:
	Board_BackGround();
protected:
	virtual bool on_expose_event(GdkEventExpose *event);
private:
	Glib::RefPtr<Gdk::Pixbuf> m_image;
};

class Board_Widget : public Gtk::HBox, public Board
{
public:
	Board_Widget();
	virtual ~Board_Widget();

protected:
	virtual void OtherAction(const Action &action);
	virtual void Slot_Change(const int p, const Color color);

private:
	void Hand_Control();
	void Close_Control();
	void Slot_Select(const int p);
	void Slot_UnSelect(const int p);

	bool EventBoxPress(GdkEventButton *event);
	sigc::signal1<void, int> &signal_clicked();

	sigc::signal1<void, int> m_signal_clicked;

	Gtk::Table m_table;
	Gtk::EventBox m_eventbox;
	std::vector<Board_Slot_Widget> m_slots;
	sigc::connection m_slot_conn;
	Board_BackGround m_background;
	bool m_conn_flag;
};

#endif /* BOARD_GUI_H_ */

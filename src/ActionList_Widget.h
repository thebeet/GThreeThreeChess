/*
 * ActionList_Widget.h
 *
 *  Created on: 2009-5-31
 *      Author: Administrator
 */

#ifndef ACTIONLIST_WIDGET_H_
#define ACTIONLIST_WIDGET_H_

#include "Board.h"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listviewtext.h>

class ActionList_Widget : public Gtk::ScrolledWindow
{
public:
	ActionList_Widget(Board &board);
	virtual ~ActionList_Widget();

private:
	void Synchronizing(Action action);
	Board *m_board;
	Gtk::ListViewText m_actionlist;
	//Gtk::ListStore m_actionlist;
};

#endif /* ACTIONLIST_WIDGET_H_ */

/*
 * ActionList_Widget.cpp
 *
 *  Created on: 2009-5-31
 *      Author: Administrator
 */

#include "ActionList_Widget.h"
#include "Action.h"

#include <gtkmm/scrollbar.h>
#include <gtkmm/main.h>

#include <vector>

ActionList_Widget::ActionList_Widget(Board &board) :
	m_actionlist(1)
{
	m_board = &board;
	m_actionlist.set_size_request(160, 0);
	m_board->SignalAction().connect(sigc::mem_fun(*this, &ActionList_Widget::Synchronizing));
	add(m_actionlist);
	set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
	m_actionlist.set_column_title(0, "Actions");
	m_actionlist.show();
}

ActionList_Widget::~ActionList_Widget()
{
}

void ActionList_Widget::Synchronizing(Action action)
{
	//Low Speed But Easy For Code
	const std::vector<Action> &actionlist = m_board->GetActionList();
	char buf[128];
	m_actionlist.clear_items();
	for (std::vector<Action>::const_iterator it = actionlist.begin(); it != actionlist.end(); ++it)
	{
		if (it->m_action == Action_Move)
		{
			snprintf(buf, 128, "%s %s(%d,%d)to(%d,%d)", ColorName(it->m_color), ActionName(it->m_action), it->m_p0 / 8, it->m_p0 & 7, it->m_p1 / 8, it->m_p1 & 7);
		}
		else
		{
			snprintf(buf, 128, "%s %s(%d,%d)", ColorName(it->m_color), ActionName(it->m_action), it->m_p0 / 8, it->m_p0 & 7);
		}
		m_actionlist.prepend_text(buf);
	}
}

/*
 * Board_Gui.cpp
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#include "Board_Widget.h"

#include "Player_Local.h"

extern Glib::StaticMutex signal_slot_mutex;

static int dxx[8] =
{
	0, 1, 1, 1, 0, -1, -1, -1
};

static int dyy[8] =
{
	-1, -1, 0, 1, 1, 1, 0, -1
};

Board_BackGround::Board_BackGround()
{
	m_image = Gdk::Pixbuf::create_from_file(IMAGE_BACKGROUND);
	//m_image->scale_simple(400, 300,  Gdk::INTERP_HYPER);
	set_alignment(0.0, 0.0);
	this->set_size_request(0, 0);
	//set_double_buffered(true);
}

bool Board_BackGround::on_expose_event(GdkEventExpose *event)
{
	DBG("Board_BackGround::on_expose_event");
	bool ret = Gtk::Image::on_expose_event(event);
	Glib::RefPtr<Gdk::Window> window = get_window();
	if (window)
	{
		Gtk::Allocation allocation = get_allocation();
		const double left = allocation.get_x();
		const double top = allocation.get_y();
		const double width = allocation.get_width();
		const double height = allocation.get_height();
		const double half_cell_width = width / 13 / 2;
		const double half_cell_height = height / 13 / 2;
		Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
		static double prewidth = -1.0;
		static double preheight = -1.0;
		if ((prewidth != width) || (preheight != height))
		{
			this->set(m_image->scale_simple(allocation.get_width(), allocation.get_height(), Gdk::INTERP_HYPER));
			prewidth = width;
			preheight = height;
		}

	    if(event)
	    {
	        // clip to the area indicated by the expose event so that we only
	        // redraw the portion of the window that needs to be redrawn
	        cr->rectangle(event->area.x, event->area.y,
	                event->area.width, event->area.height);
	        cr->clip();
	    }

		cr->set_fill_rule(Cairo::FILL_RULE_EVEN_ODD);
		cr->set_source_rgba(.4, .2, .2, 0.4);
		cr->set_line_width(5.0);
		cr->set_line_join(Cairo::LINE_JOIN_ROUND);
		//cr->set_line_cap(Cairo::LINE_CAP_ROUND);
/*		cr->save();
		cr->set_source_rgba(.2, .1, .1, 0.8);
		cr->rectangle(left, top, width, height);
		cr->fill();
		cr->restore();*/

		cr->rectangle(left + half_cell_width, top + half_cell_height,
				half_cell_width * 24, half_cell_height * 24);
		cr->rectangle(left + half_cell_width * 5, top + half_cell_height * 5,
				half_cell_width * 16, half_cell_height * 16);
		cr->rectangle(left + half_cell_width * 9, top + half_cell_height * 9,
				half_cell_width * 8, half_cell_height * 8);

		//h_line
		cr->move_to(left + half_cell_width, top + half_cell_height * 13);
		cr->line_to(left + half_cell_width * 9, top + half_cell_height * 13);

		cr->move_to(left + width - half_cell_width * 9, top + half_cell_height * 13);
		cr->line_to(left + width - half_cell_width, top + half_cell_height * 13);

		//v_line

		cr->move_to(left + half_cell_width * 13, top + half_cell_height);
		cr->line_to(left + half_cell_width * 13, top + half_cell_height * 9);

		cr->move_to(left + half_cell_width * 13, top + height - half_cell_height * 9);
		cr->line_to(left + half_cell_width * 13, top + height - half_cell_height);

		//slash
		cr->move_to(left + half_cell_width, top + half_cell_height);
		cr->line_to(left + half_cell_width * 9, top + half_cell_height * 9);
		cr->move_to(left + width - half_cell_width, top + half_cell_height);
		cr->line_to(left + width - half_cell_width * 9, top + half_cell_height * 9);
		cr->move_to(left + half_cell_width, top + height - half_cell_height);
		cr->line_to(left + half_cell_width * 9, top + height - half_cell_height * 9);
		cr->move_to(left + width - half_cell_width, top + height - half_cell_height);
		cr->line_to(left + width - half_cell_width * 9, top + height - half_cell_height * 9);

		cr->stroke();
	    cr->clip();
	}
	return ret;
}

Board_Widget::Board_Widget() :
	m_table(13, 13),
	m_slots(PMax, Board_Slot_Widget()),
	m_conn_flag(false)
{
	DBG("Board_Widget::Board_Widget()");
	add(m_eventbox);
	m_eventbox.add(m_table);
	m_table.set_homogeneous(true);
	for (int i = 0; i < nxx; ++i)
	{
		for (int j = 0; j < nyy; ++j)
		{
			int c = i * nyy + j;
			m_table.attach(m_slots[i * nyy + j], 6 + 2 * dxx[i] * (j + 1), 6 + 2 * dxx[i] * (j + 1) + 1, 6 + 2 * dyy[i] * (j + 1), 6 + 2 * dyy[i] * (j + 1) + 1);
			char buf[16];
			snprintf(buf, 16, "%d", c);
			m_slots[i * nyy + j].show();
		}
	}
	m_table.attach(m_background, 0, 13, 0, 13);
	m_background.show();
	m_table.show();
	m_eventbox.set_events(Gdk::BUTTON_PRESS_MASK);
	m_eventbox.signal_button_press_event().connect(sigc::mem_fun(*this, &Board_Widget::EventBoxPress));
	m_eventbox.show();
}

Board_Widget::~Board_Widget()
{
}

void Board_Widget::Slot_Change(const int p, const Color color)
{
	DBG("Board_Widget::Slot_Change(%d, %d)", p, color);
	m_slots[p].Change_Status(color);
	Board::Slot_Change(p, color);
}

void Board_Widget::OtherAction(const Action &action)
{
	DBG("Board_Widget::OtherAction(%d)", action.m_action);
	switch (action.m_action)
	{
		case Action_RequestControl:
		{
			if (GetCurrentPlayer().GetColor() == action.m_color)
			{
				Hand_Control();
			}
			break;
		}
		case Action_CloseControl:
		{
			if (GetCurrentPlayer().GetColor() == action.m_color)
			{
				Close_Control();
			}
			break;
		}
		case Action_SlotSelect:
		{
			Slot_Select(action.m_p0);
			break;
		}
		case Action_SlotUnSelect:
		{
			Slot_UnSelect(action.m_p0);
			break;
		}
		default:
		{
			break;
		}
	}
}

void Board_Widget::Hand_Control()
{
	DBG("Board_Widget::Hand_Control()");
	Glib::Mutex::Lock lock(signal_slot_mutex);
	Player_Local *player = (Player_Local *)(&GetCurrentPlayer());
	assert(m_conn_flag == false);
	m_conn_flag = true;
	m_slot_conn = signal_clicked().connect(sigc::mem_fun(*player, &Player_Local::Click));
}

void Board_Widget::Close_Control()
{
	DBG("Board_Widget::Close_Control()");
	if (m_conn_flag)
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_conn_flag = false;
		m_slot_conn.disconnect();
	}
}

void Board_Widget::Slot_Select(const int p)
{
	DBG("Board_Widget::Slot_Select(%d)", p);
	m_slots[p].High_Light();
}

void Board_Widget::Slot_UnSelect(const int p)
{
	DBG("Board_Widget::Slot_UnSelect(%d)", p);
	m_slots[p].Change_Status(GetSlot(p));
}

bool Board_Widget::EventBoxPress(GdkEventButton *event)
{
	DBG("Board_Widget::EventBoxPress");
	if (m_conn_flag)
	{
		DBG("x, y %lf %lf", event->x, event->y);
		for (int i = 0; i < PMax; ++i)
		{
			int h = m_slots[i].get_height();
			int w = m_slots[i].get_width();
			int x = m_slots[i].get_allocation().get_x();
			int y = m_slots[i].get_allocation().get_y();
			if ((event->x >= x) && (event->y >= y) &&
				(event->x <= x + w) && (event->y <= y + h))
			{
				signal_clicked().emit(i);
				return 0;
			}
		}
	}
	return false;
}

sigc::signal1<void, int> &Board_Widget::signal_clicked()
{
	return m_signal_clicked;
}

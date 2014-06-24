/*
 * Player_Widget.cpp
 *
 *  Created on: 2009-5-20
 *      Author: Administrator
 */

#include "Player_Widget.h"

#include "Player_Local.h"
#include "Player_AI.h"

#include "Lock.h"

Player_Widget::Player_Widget(const Color color, const Glib::ustring &name) :
	Gtk::Frame(name), m_color(color), m_player(NULL),
	m_button_time_set("Set"),
	m_group(),
	m_radiobutton_human(m_group, "Human"), m_radiobutton_computer(m_group, "Computer"),
	m_button_retract("Restract"),
	m_button_return_answer("Return Answer"),
	m_player_human_time(0), m_player_computer_time(10000)
{
	add(m_vbox);
	m_vbox.pack_start(m_hbox, false, false, 0);
		m_hbox.pack_start(m_vbox_type, false, false, 0);
			m_vbox_type.pack_start(m_radiobutton_human, false, false, 0);
			m_vbox_type.pack_start(m_radiobutton_computer, false, false, 0);
			if (m_color == Black)
			{
				m_radiobutton_human.set_active(true);
			}
			else
			{
				m_radiobutton_computer.set_active(true);
			}
			m_radiobutton_human.signal_clicked().connect(sigc::mem_fun(*this, &Player_Widget::PlayerTypeChange));
			m_radiobutton_computer.signal_clicked().connect(sigc::mem_fun(*this, &Player_Widget::PlayerTypeChange));
			m_radiobutton_human.show();
			m_radiobutton_computer.show();
			m_vbox_type.show();
		m_hbox.pack_end(m_image, false, false, 5);
		m_image.set((m_color == Black) ? (IMAGE_BLACK) : (IMAGE_WHITE));
		m_image.show();
		m_hbox.show();
	m_vbox.pack_start(m_progressbar, false, false, 5);
	m_progressbar.set_size_request(0, 18);
	m_progressbar.show();
	m_vbox.pack_start(m_hbox_time, false, false, 5);
		m_hbox_time.pack_start(m_label_time, false, false, 5);
		m_label_time.set_text("Time:");
		m_label_time.show();
		m_hbox_time.pack_start(m_text_time, true, false, 5);
		m_text_time.set_numeric(true);
		m_text_time.set_range(0, 600000);
		m_text_time.set_increments(1000, 10000);
		if (m_color == Black)
		{
			m_text_time.set_value(m_player_human_time);
		}
		else
		{
			m_text_time.set_value(m_player_computer_time);
		}
		m_text_time.show();
		m_hbox_time.pack_start(m_button_time_set, false, false, 5);
		m_button_time_set.signal_clicked().connect(sigc::mem_fun(*this, &Player_Widget::PlayerTimeSet));
		m_button_time_set.show();
	m_hbox_time.show();
	m_vbox.pack_start(m_button_retract, false, false, 5);
		m_button_retract.signal_clicked().connect(sigc::mem_fun(*this, &Player_Widget::Retract));
	m_button_retract.set_sensitive(false);
	m_button_retract.show();
	m_vbox.pack_start(m_button_return_answer, false, false, 5);
		m_button_return_answer.signal_clicked().connect(sigc::mem_fun(*this, &Player_Widget::ReturnAnswer));
	m_button_return_answer.set_sensitive(false);
	if (m_color == White)
	{
		m_button_return_answer.show();
	}
#ifndef NDEBUG
	/*
	m_vbox.pack_start(m_debug, false, false, 5);
	m_debug.show();//*/
#endif /* NDEBUG */
	m_vbox.show();
}

Player_Widget::~Player_Widget()
{
	DestroyPlayer();
}

void Player_Widget::Stop()
{
	DBG("Player_Widget::Stop()");
/*	m_radiobutton_human.set_sensitive(true);
	m_radiobutton_computer.set_sensitive(true);
	m_text_time.set_sensitive(true);*/
	m_button_retract.set_sensitive(false);
}

Player &Player_Widget::NewPlayer()
{
	PlayerTimeSet();
	if (m_radiobutton_human.get_active())
	{
		m_player = new Player_Local();
		m_player->SetTime(m_player_human_time);
	}
	else if (m_radiobutton_computer.get_active())
	{
		m_player = new Player_AI();
		m_player->SetTime(m_player_computer_time);
	}
	else
	{
		DBG("Error");
	}
	m_player->SetColor(m_color);
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_player_conn = m_player->SignalAction().connect(sigc::mem_fun(*this, &Player_Widget::Action));
	}
	m_progressbar.set_fraction(0.0);
/*	m_radiobutton_human.set_sensitive(false);
	m_radiobutton_computer.set_sensitive(false);
	m_text_time.set_sensitive(false);*/
	m_button_retract.set_sensitive(true);
	m_image.set((m_color == Black) ? (IMAGE_BLACK) : (IMAGE_WHITE));
	return *m_player;
}

void Player_Widget::ChangePlayer(Player *player)
{
	DBG("Player_Widget::ChangePlayer");
	DestroyPlayer();
	m_player = player;
	if (m_player->GetType() == PlayerHuman)
	{
		m_radiobutton_human.set_active(true);
		m_player_human_time = m_player->GetTime();
	}
	else if (m_player->GetType() == PlayerComputer)
	{
		m_radiobutton_computer.set_active(true);
		m_player_computer_time = m_player->GetTime();
	}
	else
	{
		DBG("ERROR");
	}
	PlayerTypeChange();
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_player_conn = m_player->SignalAction().connect(sigc::mem_fun(*this, &Player_Widget::Action));
	}
}

void Player_Widget::DestroyPlayer()
{
	if (m_player != NULL)
	{
		{
			Glib::Mutex::Lock lock(signal_slot_mutex);
			if (m_player_conn.connected())
			{
				m_player_conn.disconnect();
			}
		}
		delete m_player;
		m_player = NULL;
	}
}

void Player_Widget::Action(const ACTION action)
{
	DBG("Player_Widget::Action(%d)", action);
#ifndef NDEBUG
	/*
	Glib::ustring str = m_debug.get_label();
	(str += ActionName(action)) += "\n";
	m_debug.set_label(str);
	//*/
#endif /* NEDEBUG */
	switch (action)
	{
		case Action_Stop:
		{
			ActionEnd();
			Stop();
			return;
		}
		case Action_GameActionEnd:
		{
			ActionEnd();
			return;
		}
		case Action_Win:
		{
			m_image.set((m_color == Black) ? (IMAGE_BLACK_WIN) : (IMAGE_WHITE_WIN));
			m_progressbar.set_text("Winner!");
			return;
		}
		case Action_Lose:
		{
			m_image.set(IMAGE_XXX);
			m_progressbar.set_text("Loser!");
			return;
		}
		case Action_Draw:
		{
			m_progressbar.set_text("Draw.");
			return;
		}
		case Action_Move:
		{
			m_progressbar.set_text("Wait for Move...");
			break;
		}
		case Action_Take:
		{
			m_progressbar.set_text("Wait for Take...");
			break;
		}
		case Action_Cover:
		{
			m_progressbar.set_text("Wait for Cover...");
			break;
		}
		case Action_Put:
		{
			m_progressbar.set_text("Wait for Put...");
			break;
		}
		default:
		{
			return;
		}
	}
//	m_image.set((m_color == Black) ? (IMAGE_BLACK_SELECT) : (IMAGE_WHITE_SELECT));
	if (m_player->GetTime() == 0)
	{
		m_progressbar.pulse();
	}
	else
	{
		m_progressbar.set_fraction(0.0);
	}
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		if (m_progressbar_conn.connected())
		{
			DBG("WARNING ProgressBar conn dup");
			m_progressbar_conn.disconnect();
		}
		m_progressbar_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Player_Widget::AddProgressBar), ProgressBarTimeStep);
	}
	m_button_return_answer.set_sensitive(true);
}

void Player_Widget::ActionEnd()
{
	DBG("Player_Widget::ActionEnd()");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		if (m_progressbar_conn.connected())
		{
			m_progressbar_conn.disconnect();
		}
	}
	m_progressbar.set_fraction(1.0);
	m_progressbar.set_text("");
//	m_image.set((m_color == Black) ? (IMAGE_BLACK) : (IMAGE_WHITE));
	m_button_return_answer.set_sensitive(false);
}

void Player_Widget::Retract()
{
	m_player->SignalAction().emit(Action_Retract);
}

void Player_Widget::ReturnAnswer()
{
	DBG("Player_Widget::ReturnAnswer");
	m_player->SignalAction().emit(Action_TimeOver);
}

bool Player_Widget::AddProgressBar()
{
	if (m_player->GetTime() == 0)
	{
		m_progressbar.pulse();
	}
	else
	{
		m_progressbar.set_fraction(std::min(1.0, m_progressbar.get_fraction() + ProgressBarTimeStep / (double)(m_player->GetTime())));
	}
	return true;
}

void Player_Widget::PlayerTypeChange()
{
	DBG("Player_Widget(%d)::PlayerTypeChange", m_color);
	PlayerType settype(PlayerNone);
	if (m_radiobutton_human.get_active())
	{
		DBG("Human is Set");
		settype = PlayerHuman;
		m_text_time.set_value(m_player_human_time);
		m_button_return_answer.hide();
	}
	else if (m_radiobutton_computer.get_active())
	{
		DBG("Computer is Set");
		settype = PlayerComputer;
		m_text_time.set_value(m_player_computer_time);
		m_button_return_answer.show();
	}
	else
	{
		DBG("Error");
	}

	if ((m_player != NULL) && (m_player->GetStarted()))
	{
		if (m_player->GetType() != settype)
		{
			Board &board = m_player->GetBoard();
			board.ChangePlayer(NewPlayer(), m_color);
		}
	}
}

void Player_Widget::PlayerTimeSet()
{
	DBG("Player_Widget::PlayerTimeSet()");
	bool changeflag(false);
	if (m_radiobutton_human.get_active())
	{
		if (m_player_human_time != m_text_time.get_value_as_int())
		{
			DBG("Set Human Time is %d", m_player_human_time);
			m_player_human_time = m_text_time.get_value_as_int();
			changeflag = true;
		}
	}
	else if (m_radiobutton_computer.get_active())
	{
		if (m_player_computer_time != m_text_time.get_value_as_int())
		{
			DBG("Set Computer Time is %d", m_player_computer_time);
			m_player_computer_time = m_text_time.get_value_as_int();
			changeflag = true;
		}
	}
	else
	{
		DBG("Error");
	}

	if ((m_player != NULL) && (m_player->GetStarted()))
	{
		if (changeflag == true)
		{
			Board &board = m_player->GetBoard();
			board.ChangePlayer(NewPlayer(), m_color);
		}
	}
}

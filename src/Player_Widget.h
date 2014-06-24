/*
 * Player_Widget.h
 *
 *  Created on: 2009-5-20
 *      Author: Administrator
 */

#ifndef PLAYER_WIDGET_H_
#define PLAYER_WIDGET_H_

#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/image.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include <gtkmm/listviewtext.h>

#include "Player.h"

class Player_Widget : public Gtk::Frame
{
public:
	Player_Widget(const Color color, const Glib::ustring &name);
	virtual ~Player_Widget();

	Player &NewPlayer();
	void ChangePlayer(Player *player);

private:
	void DestroyPlayer();
	void Action(const ACTION action);
	void ActionEnd();
	void Retract();
	void ReturnAnswer();
	void Stop();
	bool AddProgressBar();
	void PlayerTypeChange();
	void PlayerTimeSet();

	Color m_color;
	Player *m_player;
	Gtk::Label m_label_time;
	Gtk::SpinButton m_text_time;
	Gtk::Button m_button_time_set;
	Gtk::VBox m_vbox, m_vbox_type;
	Gtk::HBox m_hbox, m_hbox_time;
	Gtk::Image m_image;
	Gtk::RadioButton::Group m_group;
	Gtk::RadioButton m_radiobutton_human, m_radiobutton_computer;
	Gtk::ProgressBar m_progressbar;
	Gtk::Button m_button_retract;
	Gtk::Button m_button_return_answer;
#ifndef NDEBUG
	Gtk::Label m_debug;
#endif /* NDEBUG */
	sigc::connection m_progressbar_conn;
	sigc::connection m_player_conn;
	int m_player_human_time;
	int m_player_computer_time;
	static const int ProgressBarTimeStep = 200;
};

#endif /* PLAYER_WIDGET_H_ */

/*
 * MainWindow.h
 *
 *  Created on: 2009-5-20
 *      Author: Administrator
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/menu_elems.h>

#include "Player_Widget.h"
#include "Board_Widget.h"
#include "ActionList_Widget.h"

#include "common.h"

class MainWindow : public Gtk::Window
{
public:
	MainWindow();
	virtual ~MainWindow();

protected:
	void StartGame();
	void StartGame_ChangeButton();
	bool StopGame();
	void StopGame_ChangeButton();
	void Quit();

	void LoadGame(const Glib::ustring &filename);

	void ButtonStop();

	void MenuStart();
	void MenuStop();
	void MenuShowSideBar();
	void MenuHelp();
	void MenuAbout();

private:
	void InitMenu();

	void BoardAction(const Action action);
	void OpenGame();
	void SaveGame() const;

	Board_Widget m_board;
	Player_Widget m_player1;
	Player_Widget m_player2;
	ActionList_Widget m_actionlist;
	Gtk::VBox m_vbox, m_vbox_player, m_vbox_buttons;
	Gtk::HBox m_hbox;
	Gtk::Button m_button_start, m_button_quit;
	sigc::connection m_button_start_conn;
	Gtk::MenuBar m_menubar;
	Gtk::Menu m_menu_game, m_menu_opinion, m_menu_help;
	bool m_showsidebar;
};

#endif /* MAINWINDOW_H_ */

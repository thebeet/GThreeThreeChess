/*
 * test.cpp
 *
 *  Created on: Feb 18, 2009
 *      Author: yonghu
 */

#include <gtkmm.h>
#include "MainWindow.h"
#include "Board_Widget.h"
#include "Player_Local.h"
#include "Player_AI.h"
#include "Board_AI.h"

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	Gtk::Main kit(argc, argv);
	if (!Glib::thread_supported())
		Glib::thread_init();
	/*Gtk::Window window;
	Board_Widget board;
	window.add(board);
	board.show();
	Player_AI player1;
	Player_Local player2;
	//player1.SetTime(3000);
	player1.SetTime(5000);
	board.Start(player1, player2);*/
	MainWindow window;
	Gtk::Main::run(window);
	//board.Save();
	return 0;
}

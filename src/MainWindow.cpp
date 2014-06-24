/*
 * MainWindow.cpp
 *
 *  Created on: 2009-5-20
 *      Author: Administrator
 */

#include "MainWindow.h"
#include "Lock.h"

#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/filechooserdialog.h>

#include <libxml++/libxml++.h>

#include "Player_Local.h"
#include "Player_AI.h"

MainWindow::MainWindow() :
	m_player1(Black, "Player1"), m_player2(White, "Player2"),
	m_actionlist(m_board),
	m_button_start("Start"), m_button_quit("Quit"),
	m_showsidebar(false)
{
	add(m_vbox);
	InitMenu();
	m_vbox.pack_start(m_menubar, false, false, 0);
	m_menubar.show();
		m_vbox.pack_start(m_hbox, true, true, 0);
			m_hbox.pack_start(m_board, true, true, 0);
			{
				Glib::Mutex::Lock lock(signal_slot_mutex);
				m_board.SignalAction().connect(sigc::mem_fun(*this, &MainWindow::BoardAction));
			}
			m_board.show();
			m_hbox.pack_start(m_vbox_player, false, false, 10);
				m_vbox_player.pack_start(m_player1, false, false, 5);
				m_player1.show();
				m_vbox_player.pack_start(m_player2, false, false, 5);
				m_player2.show();
				m_vbox_player.pack_start(m_actionlist, true, true, 5);
				m_actionlist.show();
				m_vbox_player.pack_start(m_vbox_buttons, false, false, 5);
					m_vbox_buttons.pack_start(m_button_start, false, false, 10);
					{
						Glib::Mutex::Lock lock(signal_slot_mutex);
						m_button_start_conn = m_button_start.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::StartGame));
					}
					m_button_start.show();
					m_vbox_buttons.pack_start(m_button_quit, false, false, 10);
					{
						Glib::Mutex::Lock lock(signal_slot_mutex);
						m_button_quit.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::Quit));
					}
					m_button_quit.show();
				m_vbox_buttons.show();
			//m_vbox_player.show();
		m_hbox.show();
	m_vbox.show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::StartGame()
{
	DBG("MainWindow::StartGame()");
	if (StopGame())
	{
		StartGame_ChangeButton();
		m_board.Start(m_player1.NewPlayer(), m_player2.NewPlayer());
	}
}

void MainWindow::StartGame_ChangeButton()
{
	DBG("MainWindow::StartGame_ChangeButton()");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_button_start_conn.disconnect();
		m_button_start_conn = m_button_start.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::ButtonStop));
	}
	m_button_start.set_label("Stop");
	m_menu_game.items()[2].set_sensitive(true);
	m_menu_game.items()[3].set_sensitive(true);
}

bool MainWindow::StopGame()
{
	DBG("MainWindow::StopGame()");
	if (m_board.isStoped() == false)
	{
		Gtk::MessageDialog dialog_yesorno(*this, "Stop Current Game?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
		dialog_yesorno.set_title("Stop");
		dialog_yesorno.add_button(Gtk::Stock::YES, Gtk::RESPONSE_YES);
		dialog_yesorno.add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
		int responseret = dialog_yesorno.run();
		switch (responseret)
		{
			case Gtk::RESPONSE_YES:
			{
				StopGame_ChangeButton();
				m_board.SignalAction().emit(Action(Action_Stop, None));
				break;
			}
			default:
			{
				return false;
				break;
			}
		}
	}
	return true;
}

void MainWindow::StopGame_ChangeButton()
{
	DBG("MainWindow::StopGame_ChangeButton()");
	{
		Glib::Mutex::Lock lock(signal_slot_mutex);
		m_button_start_conn.disconnect();
		m_button_start_conn = m_button_start.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::StartGame));
	}
	m_button_start.set_label("Start");
	m_menu_game.items()[3].set_sensitive(false);
}

void MainWindow::BoardAction(const Action action)
{
	DBG("MainWindow::BoardAction(%d)", action.m_action);
	switch (action.m_action)
	{
		case Action_GameEnd:
		{
			StopGame_ChangeButton();
			break;
		}
		default:
		{
			break;
		}
	}
}

void MainWindow::MenuStart()
{
	DBG("MainWindow::MenuStart()");
}

void MainWindow::ButtonStop()
{
	DBG("MainWindow::ButtonStop()");
	StopGame();
}

void MainWindow::MenuStop()
{
	DBG("MainWindow::MenuStop()");
	StopGame();
}

void MainWindow::MenuShowSideBar()
{
	DBG("MainWindow::MenuShowSideBar()");
	if (m_showsidebar)
	{
		m_showsidebar = false;
		m_vbox_player.hide();
		DBG("ToHide");
	}
	else
	{
		m_showsidebar = true;
		m_vbox_player.show();
		DBG("ToShow");
	}
}

void MainWindow::Quit()
{
	DBG("MainWindow::Quit()");
	if (StopGame())
	{
		hide();
	}
}

void MainWindow::MenuHelp()
{
	DBG("MainWindow::MenuHelp()");
	Glib::ustring helpmessage = "1.下棋分两个阶段：布棋阶段和走棋阶段。\n\
2.对局双方各执一种棋子，决定先后手，交替下子。\n\
3.布棋阶段行棋方可任意选择一个空的着棋点落子。\n\
4.如果行棋方当前的落子使己方的棋子在同一条线段上排成一线时，又称“成三”，则可以打掉对方一个子。\n\
5.布棋阶段的打子是在被打子上做一个标记，暂时不拿离棋盘，且这个棋子将失去作用，即对方不能使用这个子进行“成三”。\n\
6.当对局双方各自放完12个子后，即棋盘被放满时，将布棋阶段被打子清理出棋盘，开始走棋阶段。\n\
7.走棋阶段由布棋阶段的后手先行棋，行棋方每次可选择一个己方的棋子移动到相邻的空着棋点上。\n\
8.如果行棋方当前的走子使己方的棋子在同一条线段上排成一线时，也为“成三”，亦可以打掉对方一个子。\n\
9.走棋阶段的打子是直接将被打子移出棋盘。\n\
10.走棋阶段中当某方棋子小于3个，或者无法进行移动时，则判为该方输棋。\n\
11.走棋阶段中如果某一个局面重复出现3次，或者50步内没有打子，则为和棋。\n\
";
	Gtk::MessageDialog messagedialog(*this, helpmessage);
	messagedialog.set_title("Help");
	messagedialog.run();

}

void MainWindow::MenuAbout()
{
	DBG("MainWindow::MenuAbout()");
	Gtk::MessageDialog messagedialog(*this, "这个是项光特的毕业设计作品。\n\
指导老师：张德富\n\
");
	messagedialog.set_title("About");
	messagedialog.run();
}

void MainWindow::InitMenu()
{
	{
		using namespace Gtk::Menu_Helpers;
		m_menubar.items().push_back(MenuElem("_Game", m_menu_game));
		m_menubar.items().push_back(MenuElem("_View", m_menu_opinion));
		m_menubar.items().push_back(MenuElem("_Help", m_menu_help));

		m_menu_game.items().push_back(StockMenuElem(Gtk::Stock::NEW, sigc::mem_fun(*this, &MainWindow::StartGame)));
		m_menu_game.items().push_back(StockMenuElem(Gtk::Stock::OPEN, sigc::mem_fun(*this, &MainWindow::OpenGame)));
		m_menu_game.items().push_back(StockMenuElem(Gtk::Stock::SAVE, sigc::mem_fun(*this, &MainWindow::SaveGame)));
		m_menu_game.items().push_back(StockMenuElem(Gtk::Stock::STOP, sigc::mem_fun(*this, &MainWindow::MenuStop)));
		m_menu_game.items()[2].set_sensitive(false);
		m_menu_game.items()[3].set_sensitive(false);
		m_menu_game.items().push_back(SeparatorElem());
		m_menu_game.items().push_back(StockMenuElem(Gtk::Stock::QUIT, sigc::mem_fun(*this, &MainWindow::Quit)));

		m_menu_opinion.items().push_back(CheckMenuElem("Side Bar", sigc::mem_fun(*this, &MainWindow::MenuShowSideBar)));
		m_menu_opinion.items()[0].activate();

		m_menu_help.items().push_back(StockMenuElem(Gtk::Stock::HELP, sigc::mem_fun(*this, &MainWindow::MenuHelp)));
		m_menu_help.items().push_back(StockMenuElem(Gtk::Stock::ABOUT, sigc::mem_fun(*this, &MainWindow::MenuAbout)));
	}
}

void MainWindow::OpenGame()
{
	DBG("MainWindow::OpenGame");
	Gtk::FileChooserDialog filechooserdialog(*this, Glib::ustring("Choose A File"), Gtk::FILE_CHOOSER_ACTION_OPEN);
	Gtk::FileFilter filefilter_xml;
	filechooserdialog.set_current_folder("./Save");
	filefilter_xml.set_name("(*.xml)/XML Files");
	filefilter_xml.add_pattern("*.xml");
	Gtk::FileFilter filefilter_all;
	filefilter_all.set_name("All Files");
	filefilter_all.add_pattern("*");
	filechooserdialog.add_filter(filefilter_xml);
	filechooserdialog.add_filter(filefilter_all);
	filechooserdialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	filechooserdialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	int responseret = filechooserdialog.run();
	filechooserdialog.hide();
	switch (responseret)
	{
		case Gtk::RESPONSE_OK:
		{
			LoadGame(filechooserdialog.get_filename());
			break;
		}
		default:
		{
			break;
		}
	}
}

void MainWindow::SaveGame() const
{
	DBG("MainWindow::SaveGame");
	Gtk::FileChooserDialog filechooserdialog(*(Gtk::Window *)this, Glib::ustring("Choose A File"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	filechooserdialog.set_do_overwrite_confirmation();
	filechooserdialog.set_current_folder("./Save");
	filechooserdialog.set_current_name("*.xml");
	Gtk::FileFilter filefilter_xml;
	filefilter_xml.set_name("(*.xml)/XML Files");
	filefilter_xml.add_pattern("*.xml");
	filechooserdialog.add_filter(filefilter_xml);
	filechooserdialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
	filechooserdialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	int responseret = filechooserdialog.run();
	filechooserdialog.hide();
	switch (responseret)
	{
		case Gtk::RESPONSE_OK:
		{
			m_board.Save(filechooserdialog.get_filename());
			break;
		}
		default:
		{
			break;
		}
	}
}


void MainWindow::LoadGame(const Glib::ustring &filename)
{
	DBG("MainWindow::LoadGame()");
	xmlpp::DomParser parser;
	parser.parse_file(filename);
	if (parser)
	{
		if (StopGame())
		{
			std::vector< Player* > players(2);
			xmlpp::Element *noderoot = parser.get_document()->get_root_node();
			xmlpp::NodeSet xmlplayers = noderoot->find("/ThreeThreeChess/Players/Player");
			for (xmlpp::NodeSet::iterator it = xmlplayers.begin(); it != xmlplayers.end(); ++it)
			{
				Glib::ustring idcontent = ((xmlpp::Element *)(*it))->get_attribute_value("id");
				int id(0);
				sscanf(idcontent.c_str(), "%d", &id);
				Glib::ustring typecontent = ((xmlpp::Element *)(*(((*it)->get_children("Type")).begin())))->get_child_text()->get_content();
				if (typecontent == "Human")
				{
					players[id - 1] = new Player_Local();
				}
				else if (typecontent == "Computer")
				{
					players[id - 1] = new Player_AI();
				}
				else
				{
					DBG("ERROR");
				}
				Glib::ustring colorcontent = ((xmlpp::Element *)(*(((*it)->get_children("Color")).begin())))->get_child_text()->get_content();
				Color color = ColorReflect(colorcontent.c_str());
				players[id - 1]->SetColor(color);
				Glib::ustring timecontent = ((xmlpp::Element *)(*(((*it)->get_children("Time")).begin())))->get_child_text()->get_content();
				int time(0);
				sscanf(timecontent.c_str(), "%d", &time);
				players[id - 1]->SetTime(time);
			}
			m_player1.ChangePlayer(players[0]);
			m_player2.ChangePlayer(players[1]);
			Player *player1, *player2;
			player1 = new Player();
			player2 = new Player();
			m_board.Start(*player1, *player2);
			DBG("Board_Start OK");
			xmlpp::NodeSet nodeset = noderoot->find("/ThreeThreeChess/ActionList/Action");
			DBG("Find OK");
			for (xmlpp::NodeSet::iterator it = nodeset.begin(); it != nodeset.end(); ++it)
			{
				Glib::ustring actioncontent = ((xmlpp::Element *)(*(((*it)->get_children("Operator")).begin())))->get_child_text()->get_content();
				ACTION action = ActionReflect(actioncontent.c_str());
				Glib::ustring playercontent = ((xmlpp::Element *)(*(((*it)->get_children("Color")).begin())))->get_child_text()->get_content();
				Color color = ColorReflect(playercontent.c_str());
				Glib::ustring poscontent = ((xmlpp::Element *)(*(((*it)->get_children("Pos")).begin())))->get_child_text()->get_content();
				int pos(0), expos(0);
				DBG("Send virtual Action(%d, %d, %d, %d)", action, color, pos, expos);
				sscanf(poscontent.c_str(), "%d", &pos);
				if (action == Action_Move)
				{
					Glib::ustring exposcontent = ((xmlpp::Element *)(*(((*it)->get_children("ExPos")).begin())))->get_child_text()->get_content();
					sscanf(exposcontent.c_str(), "%d", &expos);
				}
				DBG("Send virtual Action(%d, %d, %d, %d)", action, color, pos, expos);
				m_board.SignalAction().emit(Action(action, color, pos, expos));
				if (color == Black)
				{
					player1->SignalAction().emit(Action_GameActionEnd);
				}
				else
				{
					player2->SignalAction().emit(Action_GameActionEnd);
				}
			}
			DBG("Board_Start Go OK");

			StartGame_ChangeButton();

			if (player1->GetActive())
			{
				m_board.ChangePlayer(*players[1], White);
				m_board.ChangePlayer(*players[0], Black);
			}
			else
			{
				m_board.ChangePlayer(*players[0], Black);
				m_board.ChangePlayer(*players[1], White);
			}
		}
	}
}

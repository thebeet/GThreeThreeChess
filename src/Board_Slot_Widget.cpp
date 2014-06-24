/*
 * Slot.cpp
 *
 *  Created on: Feb 20, 2009
 *      Author: yonghu
 */

#include "Board_Slot_Widget.h"

#include "Color.h"

#include <gdkmm/cursor.h>

Board_Slot_Widget::Board_Slot_Widget() :
	m_status(0)
{
	Init();
}

Board_Slot_Widget::Board_Slot_Widget(const Board_Slot_Widget &slot) :
	m_status(slot.m_status)
{
	Init();
}

Board_Slot_Widget::~Board_Slot_Widget()
{
}

void Board_Slot_Widget::Change_Status(const int new_status)
{
	DBG("Slot_Button::Change_Status(%d)", new_status);
	m_status = new_status;
	switch (m_status)
	{
		case 0:
		{
			set(IMAGE_NONE);
			break;
		}
		case 1:
		{
			set(IMAGE_BLACK);
			break;
		}
		case 2:
		{
			set(IMAGE_WHITE);
			break;
		}
		case XXX:
		{
			set(IMAGE_XXX);
			break;
		}
		default:
		{
			//set_label(Glib::ustring(std::string(1, char('0' + m_status))));
		}
	}
}

void Board_Slot_Widget::High_Light()
{
	switch (m_status)
	{
		case Black:
		{
			set(IMAGE_BLACK_SELECT);
			break;
		}
		case White:
		{
			set(IMAGE_WHITE_SELECT);
			break;
		}
		default:
		{
		}
	}
}

void Board_Slot_Widget::Init()
{
	set(IMAGE_NONE);
	set_size_request(48, 48);
	//Gdk::Cursor c;
}

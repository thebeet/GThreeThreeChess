/*
 * Slot.cpp
 *
 *  Created on: Feb 20, 2009
 *      Author: yonghu
 */

#include "Slot_Button.h"

#include "Color.h"

Slot_Button::Slot_Button() :
	m_status(0)
{
	Init();
}

Slot_Button::Slot_Button(const Slot_Button &slot) :
	m_status(slot.m_status)
{
	Init();
}

Slot_Button::~Slot_Button()
{
}

void Slot_Button::Change_Status(const int new_status)
{
	DBG("Slot_Button::Change_Status(%d)", new_status);
	m_status = new_status;
	switch (m_status)
	{
		case 0:
		{
			m_image.set(IMAGE_NONE);
			break;
		}
		case 1:
		{
			m_image.set(IMAGE_BLACK);
			break;
		}
		case 2:
		{
			m_image.set(IMAGE_WHITE);
			break;
		}
		case XXX:
		{
			m_image.set(IMAGE_XXX);
			break;
		}
		default:
		{
			set_label(Glib::ustring(std::string(1, char('0' + m_status))));
		}
	}
}

void Slot_Button::High_Light()
{
	switch (m_status)
	{
		case Black:
		{
			m_image.set(IMAGE_BLACK_SELECT);
			break;
		}
		case White:
		{
			m_image.set(IMAGE_WHITE_SELECT);
			break;
		}
		default:
		{
		}
	}
}

void Slot_Button::Init()
{
	m_image.set(IMAGE_NONE);
	m_image.show();
	add(m_image);
	this->set_size_request(48, 48);}

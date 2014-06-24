/*
 * Lock.cpp
 *
 *  Created on: 2009-5-24
 *      Author: Administrator
 */

#include "Lock.h"

Glib::StaticMutex signal_slot_mutex;

Glib::StaticMutex busy_mutex;

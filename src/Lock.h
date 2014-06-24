/*
 * Lock.h
 *
 *  Created on: 2009-5-24
 *      Author: Administrator
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <glibmm/thread.h>

extern Glib::StaticMutex signal_slot_mutex;

extern Glib::StaticMutex busy_mutex;

#endif /* LOCK_H_ */

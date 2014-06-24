/*
 * common.h
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#ifndef COMMON_H_
#define COMMON_H_

const int Error_Move = -2;
const int OK = 0;

const int nxx = 8;
const int nyy = 3;

const int PMax(24);

const int TotalLine(20);

#ifndef NDEBUG
#define DBG(fmt,x...) fprintf(stderr, "debug>" fmt "\n", ##x)
#else
#define DBG(fmt,x...) ((void)0)
#endif

#define THREAD_STOP 1
#define THREAD_START 2

#define IMAGE_BLACK "imgs/black.png"
#define IMAGE_WHITE "imgs/white.png"
#define IMAGE_NONE "imgs/none.png"
#define IMAGE_XXX "imgs/x.png"

#define IMAGE_BLACK_SELECT "imgs/black_select.png"
#define IMAGE_WHITE_SELECT "imgs/white_select.png"

#define IMAGE_BLACK_WIN "imgs/black_win.png"
#define IMAGE_WHITE_WIN "imgs/white_win.png"

#define IMAGE_BACKGROUND "imgs/back.png"

#include <cassert>

#endif /* COMMON_H_ */

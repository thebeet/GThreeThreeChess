/*
 * Color.h
 *
 *  Created on: Mar 2, 2009
 *      Author: yonghu
 */

#ifndef COLOR_H_
#define COLOR_H_

enum Color
{
	None = 0,
	Black = 1,
	White = 2,
	XXX = 3,
};

#define Opp_Color(x) (((x) == Black) ? White : Black)
#define ColorName(x) (((x) == Black) ? "Black" : "White")
#define ColorReflect(x) (((x)[0] == 'B') ? Black : White)
#endif /* COLOR_H_ */

/*
 * CC_DrawCommand.h
 * Class for how to draw
 */

/*
   Copyright (C) 1995-2011  Garrick Brian Meeker, Richard Michael Powell

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CC_DRAWCOMMAND_H_
#define _CC_DRAWCOMMAND_H_

struct CC_DrawCommand
{
	typedef enum { Ignore, Line, Arc } DrawType;
	DrawType mType;
	int x1, y1, x2, y2;
	int xc, yc;
	// nothing version
	CC_DrawCommand() : mType(Ignore) {}

	// Line version
	CC_DrawCommand(int startx, int starty, int endx, int endy) :
	mType(Line),
	x1(startx),
	y1(starty),
	x2(endx),
	y2(endy)
	{}

	// Arc version
	CC_DrawCommand(int startx, int starty, int endx, int endy, int centerx, int centery) :
	mType(Arc),
	x1(startx),
	y1(starty),
	x2(endx),
	y2(endy),
	xc(centerx),
	yc(centery)
	{}

};

#endif // _CC_DRAWCOMMAND_H_

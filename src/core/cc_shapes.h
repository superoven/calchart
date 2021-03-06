/*
 * cc_shapes.h
 * Header for calchart selector shapes
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

#ifndef _CC_SHAPES_H_
#define _CC_SHAPES_H_

#include "cc_coord.h"
#include <vector>

struct CC_DrawCommand;

class CC_shape
{
public:
	CC_shape();
	virtual ~CC_shape();

	virtual std::vector<CC_DrawCommand> GetCC_DrawCommand(float x, float y) const = 0;
	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p) = 0;
};

class CC_shape_1point: public CC_shape
{
public:
	CC_shape_1point(const CC_coord& p);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
	void MoveOrigin(const CC_coord& p);
	CC_coord GetOrigin() const;

protected:
	CC_coord origin;
};

class CC_shape_cross: public CC_shape_1point
{
public:
	CC_shape_cross(const CC_coord& p, Coord width);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
	virtual std::vector<CC_DrawCommand> GetCC_DrawCommand(float x, float y) const;

protected:
	Coord cross_width;
};

class CC_shape_2point: public CC_shape_1point
{
public:
	CC_shape_2point(const CC_coord& p);
	CC_shape_2point(const CC_coord& p1, const CC_coord& p2);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
	void MovePoint(const CC_coord& p);
	CC_coord GetPoint() const;

protected:
	CC_coord point;
};

class CC_shape_line: public CC_shape_2point
{
public:
	CC_shape_line(const CC_coord& p);
	CC_shape_line(const CC_coord& p1, const CC_coord& p2);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
	virtual std::vector<CC_DrawCommand> GetCC_DrawCommand(float x, float y) const;
};

class CC_shape_angline: public CC_shape_line
{
public:
	CC_shape_angline(const CC_coord& p, const CC_coord& refvect);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
private:
	CC_coord vect;
	float mag;
};

class CC_shape_arc: public CC_shape_2point
{
public:
	CC_shape_arc(const CC_coord& c, const CC_coord& p);
	CC_shape_arc(const CC_coord& c, const CC_coord& p1, const CC_coord& p2);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
	virtual std::vector<CC_DrawCommand> GetCC_DrawCommand(float x, float y) const;

	inline float GetAngle() const { return r-r0; }
private:
	float r,r0,d;
};

class CC_shape_rect: public CC_shape_2point
{
public:
	CC_shape_rect(const CC_coord& p);
	CC_shape_rect(const CC_coord& p1, const CC_coord& p2);

	virtual std::vector<CC_DrawCommand> GetCC_DrawCommand(float x, float y) const;
};

class CC_lasso: public CC_shape
{
public:
	CC_lasso(const CC_coord& p);
	virtual ~CC_lasso();

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
	void Clear();
	void Start(const CC_coord& p);
	void End();
	void Append(const CC_coord& p);
	bool Inside(const CC_coord& p) const;
	virtual std::vector<CC_DrawCommand> GetCC_DrawCommand(float x, float y) const;
	void Drag(const CC_coord& p);
	inline const CC_coord *FirstPoint() const
	{
		return pntlist.empty() ? NULL : &pntlist.front();
	}
private:
	bool CrossesLine(const CC_coord& start, const CC_coord& end,
		const CC_coord& p) const;
	typedef std::vector<CC_coord> PointList;
	PointList pntlist;
};

class CC_poly: public CC_lasso
{
public:
	CC_poly(const CC_coord& p);

	virtual void OnMove(const CC_coord& p, const CC_coord& snapped_p);
};

#endif

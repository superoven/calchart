/*
 * cc_continuity.h
 * Definitions for the continuity classes
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

#ifndef _CC_CONTINUITY_H_
#define _CC_CONTINUITY_H_

#include <string>

// points have a symbol index and a continuity index.  The continuity
// numbers is the way that the points know what continity they use.
// This allows multiple points to have different symbols but the same continuity.
class CC_continuity
{
public:
	CC_continuity();
	~CC_continuity();

	void SetText(const std::string& s);
	void AppendText(const std::string& s);
	const std::string& GetText() const;

private:
	std::string text;

friend bool Check_CC_continuity(const CC_continuity&, const struct CC_continuity_values&);
friend void CC_continuity_UnitTests();
};

bool Check_CC_continuity(const CC_continuity&, const struct CC_continuity_values&);

void CC_continuity_UnitTests();

#endif

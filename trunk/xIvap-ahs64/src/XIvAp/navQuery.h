/***************************************************************************
 *   Copyright (C) 2006 by Martin Domig                                    *
 *   martin@domig.net                                                      *
 *                                                                         *
 *   For license information see LICENSE.xivap in the root folder of the   *
 *   source code.                                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/

#ifndef NAV_QUERY_H
#define NAV_QUERY_H

#include "common.h"

// only show infomration of nav. database entries less than this many nm away
#define NAVQUERY_MAX_DIST 100

class NavQuery {
public:
	NavQuery();

	std::vector<string> query(const string& searchstr) const;

private:
	string navref2string(const XPLMNavRef ref, const float lat, const float lon, const float maxdist) const;

	XPLMDataRef gPlaneLat, gPlaneLon;
};

#endif
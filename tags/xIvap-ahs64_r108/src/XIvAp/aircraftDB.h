
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

#ifndef AIRCRAFTDB_H
#define AIRCRAFTDB_H

#include "common.h"

class AircraftDB
{
public:
	AircraftDB();

	class Aircraft {
	public:
		Aircraft(): icao(""), manufacturer(""), modelname(""), type(""), category('X') {};
		string icao, manufacturer, modelname, type;
		char category;
	};
	typedef std::vector<Aircraft> AircraftList;

	AircraftList retrieve(string searchstr);
	Aircraft get(string icao);

private:
	void load();

	AircraftList _database;

};

#endif

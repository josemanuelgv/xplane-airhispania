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

#include "aircraftDB.h"
#include "helpers.h"

AircraftDB::AircraftDB()
{
	load();
}

void AircraftDB::load()
{
	// load the database file
	char l[512];
	int linesize = 511;
	FILE *in;

	string filename = getXivapRessourcesDir() + "Doc8643.txt";
    in = fopen(pconst(filename), "r");

	_database.clear();
    if(in == NULL) return;

	string line = "";
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			Aircraft entry;
			if(length(line) > 0) {
				// CESSNA[tab]172 Skyhawk[tab][tab]C172[tab]L1P[tab]L
				int p = pos('\t', line); if(p < 0) continue;
				entry.manufacturer = strupcase(trim(copy(line, 0, p)));
				del(line, 0, p+1);

				// 172 Skyhawk[tab]C172[tab]L1P[tab]L
				p = pos('\t', line); if(p < 0) continue;
				entry.modelname = trim(copy(line, 0, p));
				del(line, 0, p+1);

				//C172[tab]L1P[tab]L
				p = pos('\t', line); if(p < 0) continue;
				entry.icao = trim(copy(line, 0, p));
				del(line, 0, p+1);

				//L1P[tab]L
				p = pos('\t', line); if(p < 0) continue;
				entry.type = trim(copy(line, 0, p));
				del(line, 0, p+1);

				// L
				entry.category = line[0];
				
				_database.push_back(entry);
			}
		}
	} while(length(line) > 0);
	fclose(in);
}

AircraftDB::AircraftList AircraftDB::retrieve(string searchstr)
{
	// no search string - return the whole database
	if(length(searchstr) < 1)
		return _database;

	AircraftList result;

	searchstr = strupcase(searchstr);

	for(AircraftList::const_iterator it = _database.begin(); it != _database.end(); ++it) {
		// match search string against icao, manufacturer and model
		int p = pos(searchstr, it->icao);
		if(p >= 0) { result.push_back(*it); continue; }

		p = pos(searchstr, strupcase(it->manufacturer));
		if(p >= 0) { result.push_back(*it); continue; }

		p = pos(searchstr, strupcase(it->modelname));
		if(p >= 0) result.push_back(*it);
	}
	
	return result;
}

AircraftDB::Aircraft AircraftDB::get(string icao)
{
	Aircraft result;

	for(AircraftList::const_iterator it = _database.begin(); it != _database.end(); ++it) {
		if(it->icao == icao) {
			result = *it;
			return result;
		}
	}
	return result;
}

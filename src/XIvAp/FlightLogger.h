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

#ifndef FLIGHT_LOGGER_H
#define FLIGHT_LOGGER_H

#include "common.h"

namespace FL {

class Status {
public:
	Status();
	void reset();

	Status operator-(const Status& rhs) const;

	float lat, lon, altft;
    float pitch, bank, hdg;
	float iaskt, gndkt;
	float vspeedfpm, gload;
	float totfuel_kg, totmass_kg;
	float enginepwr;
	bool onGround;
	datetime timestamp;
};

class FlightLogger {
public:
	FlightLogger(const string& filename);
	~FlightLogger();

	void reset(const string& filename);

	// call this as often as practical (~1 time a sec)
	void log(const Status& s);

private:
	Status		_lastLogged;
	logfile		*file;

	void WriteLog(const Status& s);

	/*
	class ArpPos {
	public:
		ArpPos(): icao(""), name(""), city(""), country(""), fir(""), lat(0), lon(0) {};

		std::string icao, name, city, country, fir;
		float lat, lon;
	};
	typedef std::map<std::string, ArpPos> ArpMap;
	ArpMap _airports;

	bool LoadAirports(const string& filename);
	*/
};

} // namespace

#endif

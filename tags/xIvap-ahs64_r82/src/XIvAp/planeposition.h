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

#ifndef PLANE_POSITION_H
#define PLANE_POSITION_H

#include "common.h"
//#include "XPLMMultiplayer.h"
#include "XPMPMultiplayer.h"

class PlanePosition {
public:
	PlanePosition();
	~PlanePosition();
	PlanePosition(const PlanePosition& rhs) { operator=(rhs); };
	PlanePosition& operator=(const PlanePosition& rhs);
	PlanePosition& operator+=(const PlanePosition& rhs);
	const PlanePosition operator+(const PlanePosition &other) const;
	PlanePosition& operator-=(const PlanePosition& rhs);
	const PlanePosition operator-(const PlanePosition &other) const;
	PlanePosition& operator/=(const double rhs);
	const PlanePosition operator/(const double other) const;
	PlanePosition& operator*=(const double rhs);
	PlanePosition* operator*();
	const PlanePosition operator*(const double other) const;

	XPMPPlanePosition_t pos;
	bool onground;				///< guess what that is...
	UInt32 timestamp;			///< when did we get that packet (our clock)
	UInt32 latency;				///< what was the latency to peer at that time
	UInt32 p_timestamp;			///< peers timestamp on packet (peers clock)
};

#endif

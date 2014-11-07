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

#include "planeposition.h"
#include <math.h>

PlanePosition::PlanePosition()
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat = 0;
	pos.lon = 0;
	pos.elevation = 0;
	pos.heading = 0;
	pos.pitch = 0;
	pos.roll = 0;

	onground = false;

	timestamp = 0;
	latency = 0;
	p_timestamp = 0;
}

PlanePosition::~PlanePosition()
{
}

PlanePosition& PlanePosition::operator=(const PlanePosition& rhs)
{
	if(this == &rhs)
		return *this;

	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat = rhs.pos.lat;
	pos.lon = rhs.pos.lon;
	pos.elevation = rhs.pos.elevation;
	pos.heading = rhs.pos.heading;
	pos.pitch = rhs.pos.pitch;
	pos.roll = rhs.pos.roll;

	onground = rhs.onground;

	timestamp = rhs.timestamp;
	latency = rhs.latency;
	p_timestamp = rhs.p_timestamp;

	return *this;
}

PlanePosition& PlanePosition::operator+=(const PlanePosition& rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat += rhs.pos.lat;
	pos.lon += rhs.pos.lon;
	pos.elevation += rhs.pos.elevation;
	pos.heading += rhs.pos.heading;
	// by den_rain
	// improved fix dance aircraft between 0-360 deg.
	if (pos.heading <= 0) pos.heading += 360.0;
	if (pos.heading > 360.0) pos.heading -= 360.0;
	pos.pitch += rhs.pos.pitch;
	pos.roll += rhs.pos.roll;

	timestamp += rhs.timestamp;
	latency += rhs.latency;
	p_timestamp += rhs.p_timestamp;

	return *this;
}

const PlanePosition PlanePosition::operator+(const PlanePosition &other) const {
    return PlanePosition(*this) += other;
}

PlanePosition& PlanePosition::operator-=(const PlanePosition& rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat -= rhs.pos.lat;
	pos.lon -= rhs.pos.lon;
	pos.elevation -= rhs.pos.elevation;
	pos.heading -= rhs.pos.heading;
	// by den_rain
	// improved fix dance aircraft between 0-360 deg.
	if (pos.heading <= 0) pos.heading += 360.0;
	if (pos.heading > 360.0) pos.heading -= 360.0;
	// if we turned past north, this is wrong... fix elsewhere!
	pos.pitch -= rhs.pos.pitch;
	pos.roll -= rhs.pos.roll;

	timestamp -= rhs.timestamp;
	latency -= rhs.latency;
	p_timestamp -= rhs.p_timestamp;

	return *this;
}

const PlanePosition PlanePosition::operator-(const PlanePosition &other) const {
    return PlanePosition(*this) -= other;
}

PlanePosition& PlanePosition::operator/=(const double rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat /= rhs;
	pos.lon /= rhs;
	pos.elevation /= rhs;
	pos.heading /= rhs;
	pos.pitch /= rhs;
	pos.roll /= rhs;

	timestamp /= rhs;
	latency /= rhs;
	p_timestamp /= rhs;

	return *this;
}

const PlanePosition PlanePosition::operator/(const double other) const {
    return PlanePosition(*this) /= other;
}

PlanePosition& PlanePosition::operator*=(const double rhs)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat *= rhs;
	pos.lon *= rhs;
	pos.elevation *= rhs;
	pos.heading *= rhs;
	pos.pitch *= rhs;
	pos.roll *= rhs;

	timestamp *= rhs;
	latency *= rhs;
	p_timestamp *= rhs;

	return *this;
}

PlanePosition* PlanePosition::operator*()
{
	return this;
}

const PlanePosition PlanePosition::operator*(const double other) const {
    return PlanePosition(*this) *= other;
}



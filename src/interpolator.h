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

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "common.h"
#include "planeposition.h"

#include <deque>

// for debug
//#define DEBUG_INTERP


namespace Multiplayer {

using std::deque;

class Interpolator {
public:
	Interpolator();
	~Interpolator();

	/** Process a new position.
	 *  The timestamp stored in the position object should be our best guess
	 *  of the exact time that was on our clock when this position was generated.
	 *  distance_nm is the distance from the players location to the peer in nm
	 */
	void takeNewPosition(PlanePosition* new_position, double distance_nm);

	/** Calculate position for a given timestamp (likely, now).
	 *  This function puts the computed position into the \par rendered_position
	 *  object and sets the timestamp accordingly.
	 *  Returns false if position could not be calculated, true otherwise.
	 */
	bool renderPosition(UInt32 timestamp, PlanePosition& rendered_position);

	/** Return last known position for this pilot.
	 *  This returns the latest position packet received, not the rendered position
	 */
	const PlanePosition& lastKnownPosition() const { return lastPos; };

	/** Returns true if enough data is present to calculate this aircraft position,
	 *  false otherwise
	 */
	bool usable() const;

	/** Reset the internal state */
	void clear();

	/** Returns true if the difference between the extrapolated position
	 *  (at timestamp) compared with the current known position
	 *  (truePosition) is too large */
	bool errorTooLarge(UInt32 timestamp, const PlanePosition& truePosition);

private:
	/** Double ended queue to store positions.
	 *  front() holds the latest position, back() the oldest
	 */
	deque<PlanePosition*> positions;
	// last received position
	PlanePosition lastPos;
	// change between last and previous received
	PlanePosition lastDelta;
	// where we are now
	PlanePosition Pos;
	
	double tc;
	double d;
	double distance;
};

} // namespace

#endif

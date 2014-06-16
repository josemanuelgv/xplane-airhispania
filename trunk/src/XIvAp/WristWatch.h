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

#ifndef WRIST_WATCH_H
#define WRIST_WATCH_H

#include "common.h"

#ifndef WIN32
#include <sys/time.h>
#include <time.h>
#endif

/** Class to retrieve very precise timestamps (microsecond granularity) */
class WristWatch {
public:
	WristWatch();

	/** Returns the time in microseconds since the object construction time */
	UInt32 getTime();

private:
#ifdef WIN32
	// Windows implementation following http://www.decompile.com/cpp/faq/windows_timer_api.htm
	LARGE_INTEGER _ticksPerSecond;
	LARGE_INTEGER _startTick;
#else
	struct timeval _startTime;
#endif
};

#endif

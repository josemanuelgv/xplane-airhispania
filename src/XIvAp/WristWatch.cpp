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

#include "WristWatch.h"

WristWatch::WristWatch()
{
#ifdef WIN32
	QueryPerformanceFrequency(&_ticksPerSecond);
	QueryPerformanceCounter(&_startTick);
#else
	gettimeofday(&_startTime, 0);
#endif
}

UInt32 WristWatch::getTime()
{
#ifdef WIN32
	// what time is it?
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);

	// convert the tick into the number of seconds since system start
	LARGE_INTEGER time;
	time.QuadPart = tick.QuadPart - _startTick.QuadPart;
	UInt32 usec = static_cast<UInt32>((time.QuadPart * 1000000) / _ticksPerSecond.QuadPart );
	return usec;
#else
	struct timeval curTime;
	gettimeofday(&curTime, 0);
	UInt32 usec = static_cast<UInt32>((curTime.tv_sec - _startTime.tv_sec) * 1000000
		+ (curTime.tv_usec - _startTime.tv_usec));
	return usec;
#endif
}

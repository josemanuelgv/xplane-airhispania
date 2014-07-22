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

#include "IVAOsecurity.h"

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define MAGIC1 761
#define MAGIC2 26578

#define add(id) allowed_members_.push_back(id * MAGIC1 - MAGIC2)

/**
 * Constructor
 */
IVAOSecurity::IVAOSecurity() 
{
#ifdef INTERNAL_BETA

//<<BUILD_SYSTEM_BETA_LIST>>

#endif
}

/**
 * Access Checking
 */
bool IVAOSecurity::CheckAccess(int vid) 
{
#ifdef INTERNAL_BETA
	IntList::iterator iter(std::find(allowed_members_.begin(), allowed_members_.end(), vid * MAGIC1 - MAGIC2));
	return iter != allowed_members_.end();
#else
  return true;
#endif
}

/**
 * Username transformation
 */
string IVAOSecurity::TransformUsername(const string& username) 
{
	return username + "° (" + SOFTWARE_VERSION + ")";
}

/**
 * Data checking
 */
bool IVAOSecurity::CheckDate() 
{
#ifdef BETA
	struct tm deadline_tm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	deadline_tm.tm_mday = EXPIRE_DAY;
	deadline_tm.tm_mon = EXPIRE_MONTH - 1;
	deadline_tm.tm_year = EXPIRE_YEAR - 1900;

	time_t now = time(NULL);
	time_t deadline = mktime(&deadline_tm);

	if(now > deadline) return false;
#endif
	return true;
}

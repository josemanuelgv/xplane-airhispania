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

#ifndef IVAOSECURITY_H
#define IVAOSECURITY_H

#include "common.h"

#include <list>
#include <algorithm>

class IVAOSecurity 
{
private:
  // typedef for easy coding
  typedef std::list<int> IntList;

public:
	IVAOSecurity();

	// returns true if access granted
	bool CheckAccess(int vid);

	// transforms the username
	string TransformUsername(const string& username);

	// time limit checking
	bool CheckDate();

private:
	IntList allowed_members_;
};

#endif // IVAOSECURITY_H

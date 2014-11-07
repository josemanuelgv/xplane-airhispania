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

#include "common.h"
#include "PlatformUtils.h"

/** Convert double to string */
string ftoa(double d);

#define PI 3.14159265359

string stripchar(const string& str, char c);
string stripcolons(const string& str);
double deg2dist(double lat1, double lon1, double lat2, double lon2);
double deg2crs(double lat1, double lon1, double lat2, double lon2);

// remove leading and trailing spaces
string trim(string line);

// convert string to UPPERCASE
string strupcase(string line);

string getXivapRessourcesDir(void);
string getXplaneHomeDir(void);
string getPosixPath();

#ifdef APPLE
// nasty mach-o SDK workaround
string getMachRessourcesDir();
#else
#define getMachRessourcesDir getXivapRessourcesDir
#endif


string float2coords(const float& ord);

/* Determine whether you are dealing with a 8.33 or 25 khz frequency.
 * @param frequency - frequency string as given by freq2str in xivap.h
 * @returns 
 *      true if and only if the frequency string is valid and is in 25 kHz
 *      space. false otherwise.
 */
bool is25kHzFreq(string frequency);

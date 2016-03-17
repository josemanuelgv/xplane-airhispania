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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include "common.h"
#include <list>
#include <stdio.h>

class ConfigFile {
public:
	ConfigFile();
	~ConfigFile();

	void load(string filename);
	void save(string filename);

	string readConfig(string section, string key);
	void setConfig(string section, string key, string value);

	void clearSection(string section);
	std::vector<string> listSections();

private:
	std::list<string> contents;	

	std::list<string>::iterator findSection(string section);
	std::list<string>::iterator findKey(string section, string key);
};

#endif

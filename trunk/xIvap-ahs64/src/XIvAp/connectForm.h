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

#ifndef CONNECT_FORM_
#define CONNECT_FORM_

#include "common.h"
#include "TeamSpeak.h"

class ConnectForm {
public:
	ConnectForm();

	void show();
	void hide();

	int handler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2);

	void setUserCredentials();

private:
	XPWidgetID window;
	XPWidgetID callsignTextField, realNameTextField, baseTextField, vidTextField, passwdTextField,
        hideSupCheckbox, rememberMeCheckbox, portTextField, followMeCheckbox, connectButton, cancelButton;

	XPWidgetID serverList;

	XPWidgetID voiceCheckbox;

	void create();

	void loadServers();

	typedef std::pair<string, string> ServerEntry;
	typedef std::vector<ServerEntry> ServerList;
	ServerList _servers;
	string _serverText; // string for the list field

	void _loadServerList(string filename, bool checkUpdate = false);
	
};

#endif

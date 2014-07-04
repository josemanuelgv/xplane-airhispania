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

#include "connectForm.h"
#include "systemcallbacks.h"
#include "xivap.h"
#include "configFile.h"
#include "helpers.h"

#include "XPWidgetsEx.h"

ConnectForm::ConnectForm()
{
	_serverText = ";";
	loadServers();
	create();
}

void ConnectForm::_loadServerList(string filename, bool checkUpdate)
{
	FILE *in = fopen(filename, "r");
	if(in == NULL) return;
	string line = "";
	char l[512];
	int linesize = 511;

	static bool updateChecked = false;

	// read all lines into our list
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;			// N:IVANGB:195.97.194.68:3:250
/*
			// do version check
			if(length(line) >= 14 && line[0] == '@' && checkUpdate && !updateChecked) {
				// @:X:0.1.0:0.1.0
				if(copy(line, 0, 4) == "@:X:") {
					del(line, 0, 4);
					line = trim(line);
				
					int p = pos(':', line);
					if(p < 0) continue;
					//modification to get the carry for digit 2 working. This only works for version numbers 0.x.x to 0.9.9!
					//23-09-2012 
					del(line, 0, p+3);
			
					if (atof(SOFTWARE_VERSION+2)< atof(line)) xivap.messageBox().show("A new version of X-IvAp is available at www.ivao.aero/softdev!");

					updateChecked = true;
					
				}
			}
*/
			if(l[0] != 'N') continue;
			del(line, 0, 2);	// IVANGB:195.97.194.68:3:250
			int p = pos(':', line);
			if(p < 2) continue;
			string servername = copy(line, 0, p);
			del(line, 0, p+1);	// 195.97.194.68:3:250
			p = pos(':', line);
			string ip = line;
			if(p > 0) {
				if(p < 5) continue;
				ip = copy(line, 0, p);
			}

			_serverText += servername + ";";

			_servers.push_back(ServerEntry(servername, ip));
		}
	} while(length(line) > 0);
	fclose(in);
}

void ConnectForm::loadServers()
{
	_servers.clear();
	_serverText = "";

	string filename = getXivapRessourcesDir() + SERVERS_FILENAME;
	_loadServerList(filename, true);

	// make sure we have at least one live server
	// hard coded fallback in case there is no usable server list
	if(_servers.size() < 1) {
//		_serverText = "AHS;EU2;EU3;";
		_serverText = "AHS;";
		_servers.push_back(ServerEntry("AHS.AHS", "95.211.55.201"));
//		_servers.push_back(ServerEntry("IVAO.EU2", "eu2.ivan.ivao.aero"));
//		_servers.push_back(ServerEntry("IVAO.EU3", "eu3.ivan.ivao.aero"));
	}

	filename = getXivapRessourcesDir() + OWN_SERVERS_FILENAME;
	_loadServerList(filename);
}

void ConnectForm::show()
{
	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void ConnectForm::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void ConnectForm::create()
{
	int x = 50;
	int y = 750;
	int x2 = x + 350;
//	int y2 = y - 275;
	int y2 = y - 350;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"Conexion a la red de AHS", // desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 1);

	// Callsign
	XPCreateWidget(x+10, y-25, x+70, y-47,
					1, "Indicativo:", 0, window, xpWidgetClass_Caption);
	callsignTextField = XPCreateWidget(x+70, y-25, x+70+9*8, y-47,
					1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(callsignTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(callsignTextField, xpProperty_MaxCharacters, 9);

	// Pilot Account group
	XPCreateWidget(x+10, y-50, x+70, y-62,
					1, "Identificacion", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+10, y-66, x+340, y-137,
					1, "", 0, window, xpWidgetClass_SubWindow);
	
	// Real Name
	XPCreateWidget(x+20, y-68, x+80, y-90,
					1, "Nombre real:", 0, window, xpWidgetClass_Caption);
	realNameTextField = XPCreateWidget(x+95, y-68, x+205, y-90,
					1, "", 0, window, xpWidgetClass_TextField);

	XPSetWidgetProperty(realNameTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(realNameTextField, xpProperty_MaxCharacters, 30);

	// Base Airport
	XPCreateWidget(x+20, y-90, x+80, y-112,
					1, "AD base:", 0, window, xpWidgetClass_Caption);
	baseTextField = XPCreateWidget(x+95, y-90, x+138, y-112,
					1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(baseTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(baseTextField, xpProperty_MaxCharacters, 4);

	// VID
	XPCreateWidget(x+211, y-68, x+263, y-90,
					1, "AHS:", 0, window, xpWidgetClass_Caption);
	vidTextField = XPCreateWidget(x+266, y-68, x+330, y-90,
					1, "", 0, window, xpWidgetClass_TextField);

	XPSetWidgetProperty(vidTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(vidTextField, xpProperty_MaxCharacters, 4);

	// Password
	XPCreateWidget(x+211, y-90, x+263, y-112,
					1, "Clave:", 0, window, xpWidgetClass_Caption);
	passwdTextField = XPCreateWidget(x+266, y-90, x+330, y-112,
					1, "", 0, window, xpWidgetClass_TextField);

	XPSetWidgetProperty(passwdTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(passwdTextField, xpProperty_MaxCharacters, 30);
	XPSetWidgetProperty(passwdTextField, xpProperty_PasswordMode, 1);

/* Eliminado porque no est� soportado en la red de AirHispania
	// Hide SUP/ADM rating
	hideSupCheckbox = XPCreateWidget(x+21, y-115, x+35, y-137,
					1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(hideSupCheckbox, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(hideSupCheckbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
	XPCreateWidget(x+40, y-113, x+320, y-137,
					1, "Ocultar SUP/ADM (si es aplicable)",
					0, window, xpWidgetClass_Caption);
*/

	//Remember me... or not.
	rememberMeCheckbox = XPCreateWidget(x+220, y-115, x+234, y-137,
					1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(rememberMeCheckbox, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(rememberMeCheckbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
	XPCreateWidget(x+240, y-113, x+320, y-137,
					1, "Recuerdame",
					0, window, xpWidgetClass_Caption);
	XPSetWidgetProperty(rememberMeCheckbox, xpProperty_ButtonState, 1); //enable by default



	y = y - 145;
/* Recuperado del X-IvAp anterior */
	// MTL group
	XPCreateWidget(x+10, y, x+70, y-12,
					1, "ICAO de la aeronave", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+10, y-16, x+340, y-67,
					1, "", 0, window, xpWidgetClass_SubWindow);

	// Aircraft Type
	XPCreateWidget(x+20, y-18, x+80, y-40,
					1, "ICAO:", 0, window, xpWidgetClass_Caption);
	acTypeTextField = XPCreateWidget(x+95, y-18, x+138, y-40,
					1, pconst(xivap.fpl.aircrafttype), 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(acTypeTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(acTypeTextField, xpProperty_MaxCharacters, 4);
	XPCreateWidget(x+143, y-18, x+200, y-40,
					1, "(plan de vuelo)", 0, window, xpWidgetClass_Caption);

	// MTL Model
	XPCreateWidget(x+20, y-40, x+80, y-62,
					1, "ICAO en red:", 0, window, xpWidgetClass_Caption);
	acMTLTextField = XPCreateWidget(x+95, y-40, x+138, y-62,
					1, "C182", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(acMTLTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(acMTLTextField, xpProperty_MaxCharacters, 4);
	// livery
	acLivTextField = XPCreateWidget(x+143, y-40, x+330, y-62,
					1, "", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(acLivTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(acLivTextField, xpProperty_MaxCharacters, 30);

	y = y - 75;
//	*/

	// IVAO Network Connection group
	XPCreateWidget(x+10, y, x+70, y-12,
					1, "Conexion a la red de AHS", 0, window, xpWidgetClass_Caption);
	XPCreateWidget(x+10, y-16, x+340, y-95,
					1, "", 0, window, xpWidgetClass_SubWindow);

// Quitado por no tener sentido para la red AHS
	// blabla
//	XPCreateWidget(x+20, y-18, x+80, y-30,
//					1, "For optimal performance you should connect to the",	0, window, xpWidgetClass_Caption);
//	XPCreateWidget(x+20, y-30, x+80, y-42,
//					1, "geographically closest server.",	0, window, xpWidgetClass_Caption);

	// Server
	XPCreateWidget(x+20, y-47, x+80, y-69,
				1, "Servidor:", 0, window, xpWidgetClass_Caption);
	serverList = XPCreatePopup(x+67, y-47, x+240, y-69, 1, _serverText, window);
	
	// Port
	XPCreateWidget(x+253, y-47, x+290, y-69,
					1, "Puerto:", 0, window, xpWidgetClass_Caption);
	portTextField = XPCreateWidget(x+290, y-47, x+330, y-69,
					1, "6809", 0, window, xpWidgetClass_TextField);
	XPSetWidgetProperty(portTextField, xpProperty_TextFieldType, xpTextEntryField);
	XPSetWidgetProperty(portTextField, xpProperty_MaxCharacters, 5);

	// Voice
	voiceCheckbox = XPCreateWidget(x+21, y-70, x+35, y-92,
					1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(voiceCheckbox, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(voiceCheckbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
#ifdef HAVE_TEAMSPEAK
	XPCreateWidget(x+40, y-69, x+320, y-90,
					1, "Usar Voz",
					0, window, xpWidgetClass_Caption);
#else
	XPCreateWidget(x+40, y-69, x+320, y-90,
					1, "Voz INOP",
					0, window, xpWidgetClass_Caption);
#endif

	// Follow me car
	followMeCheckbox = XPCreateWidget(x+220, y-70, x+234, y-92,
					1, "", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(followMeCheckbox, xpProperty_ButtonType, xpRadioButton);
	XPSetWidgetProperty(followMeCheckbox, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
	XPCreateWidget(x+240, y-69, x+320, y-90,
					1, "Coche 'Follow me'",
					0, window, xpWidgetClass_Caption);


	y = y - 100;
	// Connect
	connectButton = XPCreateWidget(x+210, y, x+270, y-23,
					1, "Conectar", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(connectButton, xpProperty_ButtonType, xpPushButton);

	// Cancel
	cancelButton = XPCreateWidget(x+280, y, x+340, y-23,
					1, "Cancelar", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(cancelButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, connectFormHandler);


	// load some defaults

	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);

	// load some presets
	XPSetWidgetDescriptor(callsignTextField, pconst(config.readConfig("ACCOUNT", "CALLSIGN")));
	XPSetWidgetDescriptor(vidTextField, pconst(config.readConfig("ACCOUNT", "VID")));
	XPSetWidgetDescriptor(passwdTextField, pconst(config.readConfig("ACCOUNT", "PASSWORD")));
	XPSetWidgetDescriptor(realNameTextField, pconst(config.readConfig("ACCOUNT", "REALNAME")));
	XPSetWidgetDescriptor(baseTextField, pconst(config.readConfig("ACCOUNT", "BASE")));
	char mtl_prev[4];
	XPGetWidgetDescriptor(acMTLTextField, mtl_prev, 4); 
	if (mtl_prev == "") XPSetWidgetDescriptor(acMTLTextField, pconst(config.readConfig("PLANE", "ICAO"))); // Si no hay nada en el campo de MTL, lo coge del fichero de configuraci�n

	string port = config.readConfig("ACCOUNT", "PORT");
	if(length(port) <= 1) port = "6809";
	XPSetWidgetDescriptor(portTextField, pconst(port));

	// in the servers drop down list, find the preselected server and make it active
	size_t i = 0;
	string str = config.readConfig("ACCOUNT", "SERVER");
	for(i = 0; i < _servers.size() && _servers[i].second != str; ++i);
	if(i == _servers.size()) i = 0;
	XPSetWidgetProperty(serverList, xpProperty_PopupCurrentItem, static_cast<long>(i));

	if(config.readConfig("ACCOUNT", "HIDEADM") == "1")
		XPSetWidgetProperty(hideSupCheckbox, xpProperty_ButtonState, 1);
	if(config.readConfig("ACCOUNT", "REMEMBER") == "0")
		XPSetWidgetProperty(rememberMeCheckbox, xpProperty_ButtonState, 0);
	if(config.readConfig("ACCOUNT", "FOLLOWME") == "1")
		XPSetWidgetProperty(followMeCheckbox, xpProperty_ButtonState, 1);

#ifdef HAVE_TEAMSPEAK
	if(config.readConfig("SOUND", "VOICE") == "1")
		XPSetWidgetProperty(voiceCheckbox, xpProperty_ButtonState, 1);
#endif

	XPAddWidgetCallback(callsignTextField, connectFormHandler);
	XPAddWidgetCallback(baseTextField, connectFormHandler);
	XPAddWidgetCallback(portTextField, connectFormHandler);
	
}

void ConnectForm::setUserCredentials()
{
	if(length(xivap.callsign) > 0)
		XPSetWidgetDescriptor(callsignTextField, pconst(xivap.callsign));
	if(length(xivap.realname) > 0)
		XPSetWidgetDescriptor(realNameTextField, pconst(trim(xivap.realname)));
	//if(length(xivap.fpl.aircrafttype) > 0)
	//	XPSetWidgetDescriptor(acTypeTextField, pconst(strupcase(trim(xivap.fpl.aircrafttype))));
}

int	ConnectForm::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	char buffer[512];

	if(inMessage == xpMessage_CloseButtonPushed) {

		XPGetWidgetDescriptor(acMTLTextField, buffer, sizeof(buffer));
		string aircraftMTL = buffer;
		aircraftMTL = trim(strupcase(aircraftMTL));
		XPGetWidgetDescriptor(acTypeTextField, buffer, sizeof(buffer));
		string  aircraftPV = buffer;
		aircraftPV = trim(strupcase(aircraftPV));

/* Recuperado para incorporar el ICAO de la aeronave en la conexi�n */
		if (length(aircraftMTL) > 2 && length(aircraftMTL) < 5)
		{
			xivap.setAcType(aircraftMTL, "", "");
		}
		if (length(aircraftPV) > 2 && length(aircraftPV) < 5)
		{
			xivap.fpl.aircrafttype = aircraftPV;
			if (&xivap.flightplanForm() != NULL) XPSetWidgetDescriptor(xivap.flightplanForm().getActypeTextField(), aircraftPV);
		}
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		xivap.grabKeyboardFocus();

		// cancel
		if(inParam1 == (intptr_t)cancelButton) {
			hide();
			return 1;
		}

		// connect
		if(inParam1 == (intptr_t)connectButton) {

			// read values from form
			XPGetWidgetDescriptor(callsignTextField, buffer, sizeof(buffer));
			string callsign = buffer;
			XPGetWidgetDescriptor(vidTextField, buffer, sizeof(buffer));
			string vid = buffer;
			XPGetWidgetDescriptor(passwdTextField, buffer, sizeof(buffer));
			string password = buffer;

			XPGetWidgetDescriptor(realNameTextField, buffer, sizeof(buffer));
			string realname = buffer;
			XPGetWidgetDescriptor(baseTextField, buffer, sizeof(buffer));
			string baseairport = buffer;

			unsigned int i = XPGetWidgetProperty(serverList, xpProperty_PopupCurrentItem, NULL);
			if(i > _servers.size()) i = 0;
			string server = _servers[i].second;

			XPGetWidgetDescriptor(portTextField, buffer, sizeof(buffer));
			string port = buffer;
			intptr_t hideadm = XPGetWidgetProperty(hideSupCheckbox, xpProperty_ButtonState, NULL);
			intptr_t rememberme = XPGetWidgetProperty(rememberMeCheckbox, xpProperty_ButtonState, NULL);
			intptr_t followme = XPGetWidgetProperty(followMeCheckbox, xpProperty_ButtonState, NULL);
#ifdef HAVE_TEAMSPEAK
			intptr_t voice = XPGetWidgetProperty(voiceCheckbox, xpProperty_ButtonState, NULL);
#else
			long voice = 0;
#endif

/* Recuperados campos de ICAO */
			XPGetWidgetDescriptor(acMTLTextField, buffer, sizeof(buffer));
			string aircraftMTL = buffer;
			aircraftMTL = trim(strupcase(aircraftMTL));
			XPGetWidgetDescriptor(acLivTextField, buffer, sizeof(buffer));
			string livery = buffer;
			livery = trim(strupcase(livery));
/* */
			XPGetWidgetDescriptor(acTypeTextField, buffer, sizeof(buffer));
			string  aircraftPV = buffer;
			aircraftPV = trim(strupcase(aircraftPV));

			// validate RN
			realname = trim(realname);
			if(length(realname) < 5 || pos(' ', realname) < 0) {
				xivap.messageBox().show("Por favor, introduce tu nombre real.");
				return 1;
			}

			// validate callsign... a bit
			callsign = trim(strupcase(callsign)); // FIXME: �Dejar que se pueda poner cualquier cosa o forzar a que sea "AHS"+vid?
			if(length(callsign) < 2) {
				xivap.messageBox().show("Tu alias es demasiado corto.");
				return 1;
			}

			if(length(callsign) > 7) {
				xivap.messageBox().show("Tu alias es demasiado largo.");
				return 1; 
			}

			//validate base airport
			if(length(baseairport) < 4) {
				xivap.messageBox().show("Por favor, introduce un aeropuerto base valido.");
				return 1;
			}

			bool haveChar = false;
			for(int i = 0; i < length(callsign) && !haveChar; ++i)
				if(callsign[i] >= 'A' && callsign[i] <= 'Z')
					haveChar = true;
			if(!haveChar) {
				xivap.messageBox().show("El alias tiene que contener al menos una letra.");
				return 1;
			}

			// act upon the input
			hide();

/* Recuperado para incorporar el ICAO de la aeronave en la conexi�n */
			if (length(aircraftMTL) > 2 && length(aircraftMTL) < 5)
			{
				xivap.setAcType(aircraftMTL, "", "");
//				xivap.fpl.aircrafttype = aircraft; // No cambiar el plan de vuelo, dejar el MTL s�lo para presentarlo al resto de pilotos en la red independientemente del ICAO del plan de vuelo
			}
			if (length(aircraftPV) > 2 && length(aircraftPV) < 5)
			{
				xivap.fpl.aircrafttype = aircraftPV;
				if (&xivap.flightplanForm() != NULL) XPSetWidgetDescriptor(xivap.flightplanForm().getActypeTextField(), aircraftPV);
			}

			bool fmcar = false;
			if(followme > 0)
			{
			fmcar = true;
			}
			xivap.connect(callsign, vid, password, realname + " " + baseairport,
							hideadm == 1, server, port, voice == 1, fmcar);
			xivap.realname = realname;
			xivap.baseairport = baseairport;

			xivap.flightplanForm().setUserCredentials();
			xivap.flightplanForm().hide();

			ConfigFile config;
			string filename = getXivapRessourcesDir() + CONFIG_FILE;
			config.load(filename);
			if(rememberme==1)
			{
			config.setConfig("ACCOUNT", "CALLSIGN", callsign);
			config.setConfig("ACCOUNT", "VID", vid);
			config.setConfig("ACCOUNT", "PASSWORD", password);
			config.setConfig("ACCOUNT", "REALNAME", realname);
			config.setConfig("ACCOUNT", "BASE", baseairport);
			config.setConfig("ACCOUNT", "SERVER", server);
			config.setConfig("ACCOUNT", "PORT", port);
			config.setConfig("ACCOUNT", "HIDEADM", (hideadm == 1 ? "1" : "0"));
			config.setConfig("ACCOUNT", "REMEMBER", (rememberme == 1 ? "1" : "0"));
			config.setConfig("ACCOUNT", "FOLLOWME", (followme == 1 ? "1" : "0"));
			config.setConfig("SOUND", "VOICE", (voice == 1 ? "1" : "0"));
			}
			else
			{
			config.setConfig("ACCOUNT", "CALLSIGN", "");
			config.setConfig("ACCOUNT", "VID", "");
			config.setConfig("ACCOUNT", "PASSWORD", "");
			config.setConfig("ACCOUNT", "REALNAME", "");
			config.setConfig("ACCOUNT", "BASE", "");
			config.setConfig("ACCOUNT", "SERVER", "");
			config.setConfig("ACCOUNT", "PORT", "6809");
			config.setConfig("ACCOUNT", "HIDEADM", "0");
			config.setConfig("SOUND", "VOICE", "1");
			config.setConfig("ACCOUNT", "REMEMBER", "0");
			config.setConfig("ACCOUNT", "FOLLOWME", "0");
			}
			config.save(filename);

			if(xivap.uiWindow.getPage() == SCREEN_SYSTEM)
				xivap.uiWindow.setPage(SCREEN_MAIN);

			return 1;
		}
	}

	if(inMessage == xpMsg_KeyPress) {
		XPWidgetID widget = XPGetWidgetWithFocus();

		XPKeyState_t *key = (XPKeyState_t*)inParam1;

		// don't eat some special chars: backspace, del, leftarrow, rightarrow
		if(key->key == 8 || key->key == 46 || key->key == 28 || key->key == 29) return 0;

		// these should only get UPPERCASE characters
		if(widget == callsignTextField || widget == baseTextField)
		{
			key->key = pt::upcase(key->key);

			if(widget == callsignTextField || widget == baseTextField) {
				if(!((key->key >= '0' && key->key <= '9') || (key->key >= 'A' && key->key <= 'Z')))
					return 1; // accept only 0-9 and A-Z
			}
			return 0; // don't consume the keystroke
		}

		if(widget == portTextField)
		{
			// accept only numbers in port field
			if(!(key->key >= '0' && key->key <= '9')) return 1;
			return 0;
		}

	}

	return 0;
}

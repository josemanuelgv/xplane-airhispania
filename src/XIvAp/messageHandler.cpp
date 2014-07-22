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

#include "messageHandler.h"
#include "xivap.h"


void MessageHandler(XPLMPluginID inFrom, long inMsg, void *inParam)
{
	if(inFrom == XPLM_PLUGIN_XPLANE) {
		switch(inMsg) {
			case XPLM_MSG_PLANE_LOADED:
				xivap.aircraftChange();
				break;
			case XPLM_MSG_AIRPORT_LOADED:
				xivap.airportChange();
				break;
		}
	}

	//stEventData *theEventData = (stEventData*)inParam;

	static bool visible = false;

	switch(inMsg) {
		// ---------------------------------------------------------------------------
		// -- XSQUAWKBOX SDK ---------------------------------------------------------
		// ---------------------------------------------------------------------------

		case XSB_CMD_USER_LOGIN:
			// This command poses the user's login dialog box
			xivap.connectForm().show();
			break;

		case XSB_CMD_CONNECT:
			// This command connects to the network without posing the login dialog box
			//xivap.connect();
			break;

		case XSB_CMD_DISCONNECT:
			// This command disconnect's from the network
			xivap.disconnect();
			break;

		case XSB_CMD_SHOW_FP:
			// This command poses the flight plan dialog box
			xivap.flightplanForm().show();
			break;

		case XSB_CMD_SEND_FP:
			// This command sends the FP to ATC
			xivap.sendFlightplan();
			break;
	}
}
// ---------------------------------------------------------------------------
// PLUGIN DATA CALLBACKS -----------------------------------------------------
// ---------------------------------------------------------------------------

int Handler_getInteger(void *refcon) // common integer getter
{
	xivap.addText(colWhite, string("int read from ") + (char*)refcon, true, true);

	if(refcon == (void*)XSB_VERS_NUMBER)	return SOFTWARE_VERSION_INT;
	if(refcon == (void*)XSB_CON_PORT)		return xivap.fsd.connectPort();
	if(refcon == (void*)XSB_CON_STATUS) {
		if(xivap.fsd.connected()) return 2;
		else return 0;
	}
	if(refcon == (void*)XSB_FP_FLIGHT_TYPE) {
		// int, either I, V, D, or S
		if(xivap.fpl.typeofflight == "V") return 'V';
		if(xivap.fpl.typeofflight == "I") return 'I';
		if(xivap.fpl.typeofflight == "S") return 'S';
		// wtf is 'D' supposed to be?
		return -1;
	}
	if(refcon == (void*)XSB_FP_TCAS_TYPE) {
		// int, T, H, B, L, or 0
		// erm... no idea...
		return -1;
	}
	if(refcon == (void*)XSB_FP_NAV_TYPE) {
		// int, F, G, R, A, etc.
		// incompatible. equipment is a multi-letter string in xivap.
		return 'F';
	}
	if(refcon == (void*)XSB_FP_SPEED) {
		// int, knots
		return atoi(xivap.fpl.cruisespeed);
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME) {
		// int, zulu time, e.g. 1340 = 13:40
		return atoi(xivap.fpl.deptimeest);
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME_ACTUAL) {
		// int, zulu time, e.g. 1340 = 13:40
		return atoi(xivap.fpl.deptimeact);
	}
	if(refcon == (void*)XSB_FP_ENROUTE_HRS) {
		// int, hrs
		return atoi(xivap.fpl.hoursenroute);
	}
	if(refcon == (void*)XSB_FP_ENROUTE_MINS) {
		// int, mins
		return atoi(xivap.fpl.minenroute);
	}
	if(refcon == (void*)XSB_FP_FUEL_HRS) {
		// int, mins
		return atoi(xivap.fpl.hoursfuel);
	}
	if(refcon == (void*)XSB_FP_FUEL_MINS) {
		// int, mins
		return atoi(xivap.fpl.minfuel);
	}

	return -1;
}


// reduce the amount of typing in the function below ...
#define GS_RETURNMACRO(ref, strg) {								\
	if(refcon == (void*)ref) {									\
		intptr_t len = strlen(strg);								\
		if(outbuf == NULL) return len;							\
		result = len;											\
		/* make sure not to write more than buflength */		\
		if(inMaxLength > len + 1) len = inMaxLength - 1;		\
		strncpy((char *)outbuf, strg, len + 1);					\
		return result;											\
	}															\
}

int Handler_getString(void *refcon, void *outbuf, int inOffset, int inMaxLength) // common string getter
{
	
	xivap.addText(colWhite, string("str read from ") + (char*)refcon, true, true);

	intptr_t result;
	GS_RETURNMACRO(XSB_VERS_STRING, SOFTWARE_VERSION);

	GS_RETURNMACRO(XSB_CON_CALLSIGN, xivap.fsd.callsign());
	GS_RETURNMACRO(XSB_CON_SERVER, xivap.fsd.connectHost());
	GS_RETURNMACRO(XSB_CON_PILOT_ID, xivap.fsd.vid());
	GS_RETURNMACRO(XSB_CON_PASSWORD, "(null)");
	GS_RETURNMACRO(XSB_CON_REALNAME, xivap.realname);
	GS_RETURNMACRO(XSB_CON_MODEL, xivap.fsd.mtl());
	
	if(refcon == (void*)XSB_FP_CRUISE_ALTITUDE) {
		string dummy = xivap.fpl.alttype + xivap.fpl.cruisealt;
		GS_RETURNMACRO(XSB_FP_CRUISE_ALTITUDE, dummy);
	}
	GS_RETURNMACRO(XSB_FP_CRUISE_ALTITUDE, xivap.fpl.destination);
	GS_RETURNMACRO(XSB_FP_ALTERNATE_AIRPORT, xivap.fpl.alternate);
	GS_RETURNMACRO(XSB_FP_DEPARTURE_AIRPORT, xivap.fpl.departure);
	GS_RETURNMACRO(XSB_FP_REMARKS, xivap.fpl.remarks);
	GS_RETURNMACRO(XSB_FP_ROUTE, xivap.fpl.route);

	return -1;
	
}

void Handler_setInteger(void *refcon, int value)
{
	xivap.addText(colWhite, string("int write to ") + (char*)refcon, true, true);

	if(refcon == (void*)XSB_FP_FLIGHT_TYPE) {
		// int, either I, V, D, or S
		switch(value) {
			case 'I': xivap.fpl.flightrules = "I"; break;
			case 'V': xivap.fpl.flightrules = "V"; break;
			case 'S': xivap.fpl.flightrules = "S"; break;
		}
		return;
	}
	if(refcon == (void*)XSB_FP_SPEED) {
		// int, knots
		xivap.fpl.speedtype = "N";
		xivap.fpl.cruisespeed = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME) {
		// int, zulu time, e.g. 1340 = 13:40
		xivap.fpl.deptimeest = itostring(value);
		while(length(xivap.fpl.deptimeest) < 4) xivap.fpl.deptimeest = "0" + xivap.fpl.deptimeest;
		return;
	}
	if(refcon == (void*)XSB_FP_DEPARTURE_TIME_ACTUAL) {
		// int, zulu time, e.g. 1340 = 13:40
		xivap.fpl.deptimeest = itostring(value);
		while(length(xivap.fpl.deptimeact) < 4) xivap.fpl.deptimeact = "0" + xivap.fpl.deptimeact;
		return;
	}
	if(refcon == (void*)XSB_FP_ENROUTE_HRS) {
		// int, hrs
		xivap.fpl.hoursenroute = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_ENROUTE_MINS) {
		// int, mins
		xivap.fpl.minenroute = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_FUEL_HRS) {
		// int, hrs
		xivap.fpl.hoursfuel = itostring(value);
		return;
	}
	if(refcon == (void*)XSB_FP_FUEL_MINS) {
		// int, mins
		xivap.fpl.minfuel = itostring(value);
		return;
	}

	xivap.addText(colWhite, string("not implemented: int write to ") + (char*)refcon, true, true);
}

void Handler_setString(void *refcon, void *value, int offset, int length)
{
	xivap.addText(colWhite, string("str write to ") + (char*)refcon, true, true);

	if(refcon == (void*)XSB_FP_DEPARTURE_AIRPORT) {
		xivap.fpl.departure = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_ARRIVAL_AIRPORT) {
		xivap.fpl.destination = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_ALTERNATE_AIRPORT) {
		xivap.fpl.alternate = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_CRUISE_ALTITUDE) {
		string dummy = (char*)value;
		if(dummy[0] == 'F') {
			xivap.fpl.alttype = "FL";
			del(dummy, 0, 2);
		}
		xivap.fpl.cruisealt = dummy; 
		return;
	}
	if(refcon == (void*)XSB_FP_REMARKS) {
		xivap.fpl.remarks = (char*)value;
		return;
	}
	if(refcon == (void*)XSB_FP_ROUTE) {
		xivap.fpl.route = (char*)value;
		return;
	}

	if(refcon == (void*)XSB_CON_CALLSIGN) {
		if(xivap.fsd.connected()) return; // dont set callsign while connected
		xivap.fpl.callsign = (char*)value;
		return;
	}
	/*
	if(refcon == (void*)XSB_CON_SERVER) {
		if(xivap.fsd.connected()) return; // dont set server while connected
		// ...
		return;
	}
	if(refcon == (void*)XSB_CON_PILOT_ID) {
		if(xivap.fsd.connected()) return; // dont set while connected
		// ...
		return;
	}
	if(refcon == (void*)XSB_CON_PASSWORD) {
		if(xivap.fsd.connected()) return; // dont set while connected
		// ...
		return;
	}
	if(refcon == (void*)XSB_CON_REALNAME) {
		if(xivap.fsd.connected()) return; // dont set while connected
		
		// ...
		return;
	}
	*/
	if(refcon == (void*)XSB_CON_MODEL) {
		xivap.fpl.aircrafttype = (char*)value;
		return;
	}

	xivap.addText(colWhite, string("not implemented: str write to ") + (char*)refcon, true, true);
}

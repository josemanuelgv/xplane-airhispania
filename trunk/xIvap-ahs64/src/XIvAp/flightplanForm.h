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

#ifndef FLIGHTPLAN_FORM_
#define FLIGHTPLAN_FORM_

#include "common.h"
#include "aircraftDB.h"
#include "airlinesDB.h"
#include "awyFinder.h"
#include "XPWidgetUtils.h" // added for CutCopyPaste

class FlightplanForm {
public:
	FlightplanForm();
	~FlightplanForm();

	void show();
	void hide();
	void fillForm(bool);

	int handler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t  inParam1, intptr_t inParam2);

	// updates callsign and realname from xivap
	void setUserCredentials();
	void setFPTextFields();
	void setAcfIcao();
	void FMcar(bool enable);

	// CutCopyPaste functional
	bool GetTextFromClipboard(std::string& outText);
	bool SetTextToClipboard(const std::string& inText);
	int WidgetFunc_CutCopyPaste(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2);


private:
	XPWidgetID window;
	XPWidgetID callsignTextField, actypeTextField,
		equipTextField, departureTextField, deptimeTextField, transponderTextField,
		speedTextField, altTextField, routeTextField, destTextField, eetTextField,
		alternateTextField, alternate2TextField, commentsTextField, enduranceTextField,
		pobTextField, picTextField, airlineTextField, airlineStaticText, fmcarStaticText;
	// XPWidgetID fmcTextField;
	XPWidgetID exportButton, resetButton, sendButton, cancelButton;

	XPWidgetID flightRulesPopup, typePopup, wtcPopup, speedtypePopup, xpdrPopup,
		numberTextField, alttypePopup, actypePopup, airlinePopup, liveryPopup;

	AircraftDB::AircraftList acList;
	AirlinesDB::AirlinesList alList;
	AirlinesDB::LiveryList livList;
	void updateACList();
	void updateALList();
	bool callUpdateAC, callUpdateAL;

	void create();
	void reset();
	void send(bool closeWindow = true);

	void FMCExport();

	AwyFinder *routefinder;
	
};

#endif

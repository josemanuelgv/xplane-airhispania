/****************************************************************************

    X-IvAp  -  Martin Domig <martin@domig.net>
    Copyright (C) 2006 by Martin Domig

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/
#include "flightplanForm.h"
#include "systemcallbacks.h"
#include "configFile.h"
#include "helpers.h"
#include "xivap.h"
#include "SoundGraphics.h"

extern	int	xPanelWindowLeft,  xPanelWindowBottom ; //export to soundgraphics
extern char Graphics; //export to soundgraphics

UiWindow::UiWindow()
{
	mDownX = mDownY = 0;
	dragging = false;
	clicking = false;
	textLine = 0;
	_extradark = true;
	_sortOrder = SortDistance;
	_keyboardfocus = false;
	_cursorTime = 0;
	_cursor = false;
}

bool UiWindow::visible()
{
	return XPLMGetWindowIsVisible(window) != 0;
}

void UiWindow::show()
{
	XPLMSetWindowIsVisible(window, 1);
	if(onEditPage()) grabKeyboardFocus();
	refresh();
}

void UiWindow::hide()
{
	XPLMSetWindowIsVisible(window, 0);
}

void UiWindow::pluginStart()
{
	create();
}

void UiWindow::pluginStop()
{
	XPLMDestroyWindow(window);
}

void UiWindow::create()
{
	XPLMGetFontDimensions(xplmFont_Basic, &fontWidth, &fontHeight, NULL);

	width = 2*5 + (ui_columns + SK_LEN * 2) * fontWidth+Ex_Window_Range; //extended window with new click aera's 16/08/2012
	height = 2*5 + ui_rows * fontHeight;

	int x = 50;
	int y = 720;
	int x2 = x + width;
	int y2 = y - height;

	/* Now we create a window.  We pass in a rectangle in left, top,
	 * right, bottom screen coordinates.  We pass in three callbacks. */
	window = XPLMCreateWindow(x, y, x2, y2, 1,
							  uiTextWinDrawCallback, 
							  uiTextWinKeyCallback,
							  uiTextWinMouseCallback,
							  NULL);
	screen = SCREEN_INIT;
	prepScreen();

	ConfigFile config;
	string str = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(str);
	str = config.readConfig("PREFERENCES", "DARK");
	if(str == "1") _extradark = true;
	else _extradark = false;
}

void UiWindow::textDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	// First we get the location of the window passed in to us
	XPLMGetWindowGeometry(window, &left, &top, &right, &bottom);
	//this is needed to glue the texture's to xivap 15/08/2012
	xPanelWindowLeft=left;
	xPanelWindowBottom=bottom;

	
	//dont draw the TDB, as it is not needed anymore 15/08/2012
	// Draw a translucent dark rectangle that is our window's shape
	//XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// we want it a bit darker - draw that box a second time :)
//	if(_extradark) XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// draw the select keys
	// delete as not needed anymore 15/08/2012
	/*for(int i = 0; i < 6; ++i) {
		XPLMDrawString(SK_COLOR,
			left + MARGIN_LEFT,
			bottom + MARGIN_TOP + i * 2 * fontHeight, // bottom left
			LSK, NULL, xplmFont_Basic);
		XPLMDrawString(SK_COLOR,
			right - MARGIN_LEFT - SK_LEN * fontWidth-Ex_Window_Range,
			bottom + MARGIN_TOP + i * 2 * fontHeight, // bottom left
			RSK, NULL, xplmFont_Basic);
	}
*/
	// draw the paint jobs
	for(std::vector<UIWriteJob>::const_iterator i = joblist.begin(); i != joblist.end(); ++i)
		XPLMDrawString(i->color, getX(i->x), getY(i->y), const_cast<char*>(pconst(i->text)), NULL, xplmFont_Basic);
}

void UiWindow::textToggleCallback(void *inRefcon)
{
}

void UiWindow::textKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
		void *inRefcon, int losingFocus)
{
	if(losingFocus) {
		_keyboardfocus = false;
		return;
	}

	if((inFlags & (xplm_OptionAltFlag | xplm_ControlFlag)) != 0) {
		// Ctrl or Alt key (Alt doesnt work?) is down
		// This is a shortcut of some kind -> get rid of the keyboard focus
		dumpKeyboardFocus();
		return;
	}

	// accept key event if key is pressed down or held down
	if(((inFlags & xplm_DownFlag) != 0) // key down
		|| ((inFlags & xplm_DownFlag) == 0 && (inFlags & xplm_UpFlag) == 0)) { // key held down

		if(inKey < 0) return; // special characters on the keyboard

		switch(inKey) {
			case 13: keyEnter(); break;
			case 8: keyBackspace(); break;
			case 30: keyUpArrow(); break;
			case 31: keyDownArrow(); break;
			case 28: keyLeftArrow(); break;
			case 29: keyRightArrow(); break;
			default: handleKey(inKey); break;
		}

		_lastKeyPress = XPLMGetElapsedTime();
		_cursor = true;	// show cursor after keystroke
		_cursorTime = _lastKeyPress;
		prepScreen();	// refresh UI window

	}
}

int UiWindow::textMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	switch(inMouse) {
		case xplm_MouseDown:
			XPLMBringWindowToFront(window);
			if(onEditPage())
				grabKeyboardFocus();

			if(checkClick(x, y))
				return 1;
			else
				return beginDrag(x, y);

			break;

		case xplm_MouseDrag:
			if(dragging) return continueDrag(x, y);
			else if(clicking) return continueClick(x, y);
			break;

		case xplm_MouseUp:
			if(dragging) return endDrag(x, y);
			else if(clicking) return endClick(x, y);
			break;
	}
	return 0;
}

void UiWindow::addText(UI_Alignment align, int line, float *color, const string& text, bool big, bool infms)
{
	UIWriteJob job;
	job.text = text;
	job.color = color;
	job.y = line;
	job.align = align;
	job.big = big;


	switch(align) {
		case AL_ctr:
			job.x = getMiddleX(text);
			break;

		case AL_right:
			job.x = getRightX(text);
			break;

		default:
		case AL_left:
			job.x = 0;
			break;
	}

	// replace " characters with '
	int p = pos('"', job.text);
	while(p >= 0) {
		job.text[p] = '\'';
		p = pos('"', job.text);
	}

	joblist.push_back(job);
}

int UiWindow::beginDrag(int x, int y)
{
	mDownX = x;
	mDownY = y;
	dragging = true;
	return 1;
}

int UiWindow::continueDrag(int x, int y)
{
	int dX = mDownX - x;
	int dY = mDownY - y;

	left -= dX;
	top -= dY;
	bottom = top - height;
	right = left + width;

	mDownX = x;
	mDownY = y;

	XPLMSetWindowGeometry(window, left, top, right, bottom);
	return 1;
}

int UiWindow::endDrag(int x, int y)
{
	dragging = false;
	return 1;
}

int UiWindow::checkButton(int x, int y)
{
	bool l = false;
	bool r = false;

	if(x >= left + MARGIN_LEFT && x <= left + MARGIN_LEFT + SK_LEN * fontWidth)
		l = true;	// click into one of the LSK buttons
	else if(x >= right - MARGIN_LEFT - SK_LEN * fontWidth-Ex_Window_Range && x <= right - MARGIN_LEFT-Ex_Window_Range)
		r = true;	// click into one of the RSK buttons

	if(l || r) {
		for(int i = 0; i < 6; ++i) {
			if(y <= getY(i*2) && y >= getY(i*2+1)) {
				// bingo
				int result = i+1;
				if(r) result += 6;
				return result;
			}
		}
	}
	//last check for the new keys 16/08/2012
	x-=left;
	y=top-y;
	int i=0;
	while (i<4)  {
		if (x>=ExtMouseFields[4*i] && x<=ExtMouseFields[4*i+2]) if (y>=ExtMouseFields[4*i+1] && y<=ExtMouseFields[4*i+3]) return (i+Connect_Capture);
		i++;
	}

	return 0; //nothing else found, quit with 0
}

int UiWindow::checkClick(int x, int y)
{
	clickButton = checkButton(x, y);
	if(clickButton == 0) {
		clicking = false;
		return 0;
	}
	clicking = true;
	return 1;
}

int UiWindow::continueClick(int x, int y)
{
	if(clicking) return 1;
	return 0;
}

int UiWindow::endClick(int x, int y)
{
	int button = checkButton(x, y);
	if(button == clickButton)
		buttonClicked(button);
	clickButton = 0;
	clicking = false;
	return 0;
}

void UiWindow::_appendText(float *color, const string& line)
{
	if(uiText.size() >= SCROLLBACK)
		uiText.pop_back();
	uiText.push_front(UiWindow::line(color, line));

	if(textLine != 0)
		scrollUp();
}


void UiWindow::addMessage(float *color, const string& text, bool logToConsole, bool logToLogfile)
{
	if(length(text) <= 0) return;
	xivap.addText(color, text, logToConsole, logToLogfile);

	// wrap line if necessary
	string line = text;

	// UI window
	while(length(line) > ui_columns) {
		int p = ui_columns;
		while(line[p] != ' ' && p > 15) --p;
		if(p <= 15) p = ui_columns; // no space found in first part of line -> wrap at last column

		_appendText(color, copy(line, 0, p));
		del(line, 0, p+1);
		if(length(line) > 0)
			line = "   " + trim(line);
	}
	if(length(line) > 0)
		_appendText(color, line);

	

	prepScreen();
}

void UiWindow::scrollUp()
{
	if(uiText.size() <= 5) {
		textLine = 0;
		return;
	}
	++textLine;
	if(textLine + 5 > uiText.size())
		textLine = uiText.size() - 5;
}

void UiWindow::scrollDown()
{
	if(uiText.size() <= 5) {
		textLine = 0;
		return;
	}
	if(textLine > 0)
		--textLine;
}

void UiWindow::setPage(int page)
{
	if(page >= SCREEN_INIT && page <= SCREEN_MAXPAGE) {
		screen = page;

		if(page == SCREEN_COM1 || page == SCREEN_COM2)
			prepAtcList();

		prepScreen();
	}
}

void UiWindow::buttonClicked(int button)
{
	switch (button)
	{
	case TX_Capture: xivap.xpdrModeToggle();break;
	case TCAS_Capture: {xivap.Tcasbox.hide();break;}
	case Ident_Capture: {xivap.setXpdrIdent();break;}
	case Connect_Capture: if(xivap.fsd.connected()) xivap.disconnectForm().show();
					else xivap.connectForm().show();
					break;
	}

	switch(screen) {
		case SCREEN_INIT:
			switch(button) {
				case RSK5:
					screen = SCREEN_MAIN;
					break;
			}
			break;

		case SCREEN_MAIN:
			switch(button) {
				case LSK1:
					screen = SCREEN_COM1;
					prepAtcList();
					break;
				case LSK2:
					screen = SCREEN_COM2;
					prepAtcList();
					break;
				case LSK3:
				case RSK3:
					xivap.uiWindow.scrollUp();
					break;
				case LSK5:
				case RSK5:
					xivap.uiWindow.scrollDown();
					break;
				case RSK1:
					screen = SCREEN_ACARS;
					break;
				case RSK2:
					screen = SCREEN_SYSTEM;
					break;
			}
			break;

		case SCREEN_ACARS:
			switch(button) {
				case LSK1:
					xivap.fsd.sendWxRequest(FSD::WX_METAR, xivap.fpl.departure);
					screen = SCREEN_MAIN;
					break;
				case LSK2:
					xivap.fsd.sendWxRequest(FSD::WX_METAR, xivap.fpl.destination);
					screen = SCREEN_MAIN;
					break;
				case LSK3:
					xivap.fsd.sendWxRequest(FSD::WX_TAF, editLine);
					editLine = "";
					screen = SCREEN_MAIN;
					break;
				case LSK4:
					xivap.fsd.sendWxRequest(FSD::WX_SHORTTAF, editLine);
					editLine = "";
					screen = SCREEN_MAIN;
					break;
				case LSK5:
					xivap.fsd.sendWxRequest(FSD::WX_METAR, editLine);
					editLine = "";
					screen = SCREEN_MAIN;
					break;
				case RSK1:
					xivap.flightplanForm().show();
					screen = SCREEN_MAIN;
					break;
				case RSK2:
					xivap.fsd.sendInfoRequest(editLine, _FSD_INFOREQ_ATIS_);
					editLine = "";
					screen = SCREEN_MAIN;
					break;
				case RSK5:
					screen = SCREEN_MAIN;
					break;
			}
			break;

		case SCREEN_COM1:
			switch(button) {
				case LSK1:
					screen = SCREEN_MAIN;
					xivap.setComActive(1);
					break;
				case LSK3:
					switch(_sortOrder) {
						case SortDistance: _sortOrder = SortFrequency; break;
						case SortFrequency: _sortOrder = SortCallsign; break;
						case SortCallsign: _sortOrder = SortDistance; break;
					}
					prepAtcList();
					break;
				case LSK4:
					if(_atcListOffset > 0) _atcListOffset -= 6;
					break;
				case LSK5:
					if(_atcPositions.size() > _atcListOffset + 6) _atcListOffset += 6;
					break;
				case LSK6:
					screen = SCREEN_MAIN;
					break;

				case RSK1:
					if(selectStation(1))
						screen = SCREEN_MAIN;
					break;
				case RSK2:
					if(selectStation(2))
						screen = SCREEN_MAIN;
					break;
				case RSK3:
					if(selectStation(3))
						screen = SCREEN_MAIN;
					break;
				case RSK4:
					if(selectStation(4))
						screen = SCREEN_MAIN;
					break;
				case RSK5:
					if(selectStation(5))
						screen = SCREEN_MAIN;
					break;
				case RSK6:
					if(selectStation(6))
						screen = SCREEN_MAIN;
					break;
			}
			break;

		case SCREEN_COM2:
			switch(button) {
				case LSK2:
					screen = SCREEN_MAIN;
					xivap.setComActive(2);
					break;
				case LSK3:
					switch(_sortOrder) {
						case SortDistance: _sortOrder = SortFrequency; break;
						case SortFrequency: _sortOrder = SortCallsign; break;
						case SortCallsign: _sortOrder = SortDistance; break;
					}
					prepAtcList();
					break;
				case LSK4:
					if(_atcListOffset > 0) _atcListOffset -= 6;
					break;
				case LSK5:
					if(_atcPositions.size() > _atcListOffset + 6) _atcListOffset += 6;
					break;
				case LSK6:
					screen = SCREEN_MAIN;
					break;

				case RSK1:
					if(selectStation(1))
						screen = SCREEN_MAIN;
					break;
				case RSK2:
					if(selectStation(2))
						screen = SCREEN_MAIN;
					break;
				case RSK3:
					if(selectStation(3))
						screen = SCREEN_MAIN;
					break;
				case RSK4:
					if(selectStation(4))
						screen = SCREEN_MAIN;
					break;
				case RSK5:
					if(selectStation(5))
						screen = SCREEN_MAIN;
					break;
				case RSK6:
					if(selectStation(6))
						screen = SCREEN_MAIN;
					break;
			}
			break;

		case SCREEN_SYSTEM:
			switch(button) {
				case LSK1:
					xivap.setRadioCoupled(!xivap.radioCoupled());
					break;
			/*	mot needed anymore due to graphics switches 16/08/2012
				case LSK4:
					xivap.xpdrModeToggle();
					break;
				case LSK5:
					xivap.setXpdrIdent();
					break;
			
				case RSK1:
					if(xivap.fsd.connected()) xivap.disconnectForm().show();
					else xivap.connectForm().show();
					break;
				*/
				case RSK2:
					screen = SCREEN_PREFS;
					break;
				case RSK3:
					if(xivap.consoleVisible()) xivap.setConsoleVisible(false);
					else xivap.setConsoleVisible(true);
					break;
			/*	not needed anymore due graphics texture 16/08/2012
				case RSK4: {
						_extradark = !_extradark;
						ConfigFile config;
						string filename = getXivapRessourcesDir() + CONFIG_FILE;
						config.load(filename);
						config.setConfig("PREFERENCES", "DARK", (_extradark ? "1" : "0"));
						config.save(filename);
					}
					break;
					*/
				case RSK6:
					screen = SCREEN_MAIN;
					break;
			}
			break;

		case SCREEN_PREFS:
			switch(button) {
				case LSK1:
					xivap.setWeather(!xivap.usingWeather());
					break;
				case LSK2:
					xivap.setMultiplayer(!xivap.usingMultiplayer());
					break;
				case LSK3:
					xivap.setLabels(!xivap.usingLabels());
					break;
				case LSK4:
					xivap.setPMSG(!xivap.usingPMSG());
					break;
#ifdef HAVE_TEAMSPEAK
				case LSK5:
					xivap.setVoice(!xivap.usingVoice());
					break;
#endif
				case RSK5:
					screen = SCREEN_SYSTEM;
					break;
				case RSK6:
					screen = SCREEN_MAIN;
					break;
			}
	}

	prepScreen();

	if(!onEditPage())
		dumpKeyboardFocus();	// give away the keyboard focus if we dont need it
}

void UiWindow::prepScreen()
{
	clearText();
	switch(screen) {
		case SCREEN_INIT:
		default:
			screen = SCREEN_INIT;
			addText(AL_ctr, 0, colDarkGreen, "INIT", true);
			addText(AL_left, 3, colGreen, string(SOFTWARE_NAME) + " " + SOFTWARE_VERSION
#ifdef IVAO
				+ " IVAO"
#endif
				+ " (Rev. " + xivap.revision() + ")  for " + PLATFORM, true);
			addText(AL_left, 5, colGreen, SOFTWARE_COPYRIGHT1, true); //splitted the copyrights to fit in the new resized window
			addText(AL_left, 6, colGreen, SOFTWARE_COPYRIGHT2, true);
			#if (X64) 
			addText(AL_left, 7, colGreen, "(64 bit version)", true);
			#else 
			addText(AL_left, 7, colGreen, "(32 bit version)", true);
			#endif	
			addText(AL_right, 9, colWhite, "MAIN>", true);
			
			break;

		case SCREEN_MAIN: {
				addText(AL_ctr, 0, colDarkGreen, "MAIN", true);
				addText(AL_left, 0, colDarkGreen, "VHF1 " + xivap.com1freq(), false);
				
				addText(AL_ctr, 1, colYellow,"WxStat:"+ xivap._erwin.oldWxStation,false); //added for display Used wxname 4/01/2013
				string line = "OFFLINE";
				if(xivap.online()) {line = xivap.fsd.callsign() + " ON";Graphics|=4;} else Graphics&=~4;
				addText(AL_right, 0, colLightBlue, line, false);

				addText(AL_left, 1, colWhite, "<" + xivap.comStr(1), true);
				addText(AL_left, 2, colDarkGreen, "VHF2 " + xivap.com2freq(), false);
				addText(AL_left, 3, colWhite, "<" + xivap.comStr(2), true);
				addText(AL_right, 1, colWhite, "ACARS>", true);

				addText(AL_right, 3, colWhite, "SYSTEM>", true);

				// top line --------------
				line = "-"; while(length(line) < ui_columns) line += "-";
				addText(AL_left, 4, colDarkGreen, line, true);

				// text messages - hide them from the GW-FMS (last param = false)
				int j = 0;
				for(size_t i = textLine; i < uiText.size() && j < 5; ++i, ++j)
					addText(AL_left, 9 - j, uiText[i].first, uiText[i].second, true, false);
				
				// bottom line --------------
				addText(AL_left, 10, colDarkGreen, line, true);
			}
			break;

		case SCREEN_ACARS: {
				addText(AL_ctr,  0, colDarkGreen, "ACARS", true);
				addText(AL_left, 0, colDarkGreen, "REQ WX DEP", false);
				addText(AL_left, 1, colWhite, "<" + xivap.fpl.departure, true);
				addText(AL_left, 2, colDarkGreen, "REQ WX ARR", false);
				addText(AL_left, 3, colWhite, "<" + xivap.fpl.destination, true);
				addText(AL_left, 5, colWhite, "<REQ TAF", true);
				addText(AL_left, 7, colWhite, "<REQ SHORTTAF", true);
				addText(AL_left, 9, colWhite, "<REQ METAR", true);
				addText(AL_right, 1, colWhite, "SEND FPLN>", true);
				addText(AL_right, 3, colWhite, "RQ ATIS>", true);
				addText(AL_right, 9, colWhite, "MAIN>", true);

				string line = "-"; while(length(line) < ui_columns) line += "-";
				addText(AL_left, 10, colDarkGreen, line, true);
			}
			break;

		case SCREEN_SYSTEM: {
			addText(AL_ctr, 0, colDarkGreen, "SYSTEM", true);
			addText(AL_left, 0, colDarkGreen, "RADIO CPL", false);
			string str = "<BOTH";
			if(!xivap.radioCoupled()) {
				if(xivap.activeRadio() == 1) str = "<COM1 ONLY";
				else str = "<COM2 ONLY";
			}
			addText(AL_left, 1, colWhite, str, true);
/* not needed anymore 16/08/2012
			addText(AL_left, 6, colDarkGreen, "XPDR MODUS", false);
			str = "<MODE ";
			if(xivap.xpdrIdent() || xivap.xpdrModus() == IdentC) {str += "C";Graphics&=~2;}
			else {str += "SBY";Graphics|=2;}
			addText(AL_left, 7, colWhite, str, true);

			addText(AL_left, 9, colWhite, "<XPDR IDENT", true);
			if(xivap.xpdrIdent()) addText(AL_left, 10, colYellow, "IDENTING", false);

			str = "CONNECT>";
			if(xivap.fsd.connected()) str = "DISCONNECT>";
			addText(AL_right, 1, colWhite, str, true);
*/
			addText(AL_right, 3, colWhite, "PREFERENCES>", true);

			str = xivap.consoleVisible() ? "HIDE CONSOLE>" : "SHOW CONSOLE>";
			addText(AL_right, 5, colWhite, str, true);
			/* not needed anymore due graphics textures
			str = "WINDOW BRTN";
			addText(AL_right, 6, colDarkGreen, str, false);
			if(_extradark) str = "DARK>"; else str = "BRIGHT>";
			addText(AL_right, 7, colWhite, str, true);
			*/
			addText(AL_right, 11, colWhite, "MAIN>", true);
			}
			break;

		case SCREEN_PREFS:
			addText(AL_ctr, 0, colDarkGreen, "PREFERENCES", true);
			addText(AL_left, 0, colDarkGreen, "IVAO WX", false);
			addText(AL_left, 1, colWhite, string("<WX ") + (xivap.usingWeather() ? "ON" : "OFF"), true);
			addText(AL_left, 2, colDarkGreen, "MULTIPLAYER", false);
			addText(AL_left, 3, colWhite, string("<MP ") + (xivap.usingMultiplayer() ? "ON" : "OFF"), true);
			addText(AL_left, 5, colWhite,string("<Tagging ") + (xivap.usingLabels() ? "ON" : "OFF"), true);
			addText(AL_left, 7, colWhite,string("<Private MSG alert ") + (xivap.usingPMSG() ? "ON" : "OFF"), true);
			addText(AL_left, 8, colDarkGreen, "TeamSpeak", false);
#ifdef HAVE_TEAMSPEAK
			addText(AL_left, 9, colWhite, string("<VOICE ") + (xivap.usingVoice() ? "ON" : "OFF"), true);
#else
			addText(AL_left, 9, colGreen, "INOP", true);
#endif
			
			addText(AL_right, 9, colWhite, "SYSTEM>", true);
			addText(AL_right, 11, colWhite, "MAIN>", true);
			break;

		case SCREEN_COM1:
			addText(AL_ctr, 0, colDarkGreen, "COM1", true);
			addText(AL_left, 0, colDarkGreen, xivap.comStr(1), false);
			addText(AL_left, 1, colWhite, "<ACT COM1", true);

			atcList();

			addText(AL_left, 11, colWhite, "<MAIN", true);
			break;

		case SCREEN_COM2:
			addText(AL_ctr, 0, colDarkGreen, "COM2", true);
			addText(AL_left, 2, colDarkGreen, xivap.comStr(2), false);
			addText(AL_left, 3, colWhite, "<ACT COM2", true);

			atcList();

			addText(AL_left, 11, colWhite, "<MAIN", true);
	}

	if(onEditPage()) {
		string str = editLine;
		if(length(str) > ui_columns - 1)
			del(str, 0, length(str) - ui_columns + 1);

		if(!_keyboardfocus) _cursor = false;
		else {
			float elapsed = XPLMGetElapsedTime();
			if(elapsed >= _cursorTime + CURSOR_INTERVAL) {
				_cursor = !_cursor;
				_cursorTime = elapsed;
			}
		}
		if(_cursor) str = str + "_";
		addText(AL_left, 11, colGreen, str, true, true);
	}
}

void UiWindow::atcList()
{
	addText(AL_left, 4, colDarkGreen, "SORT ORDER", false);

	string str;
	switch(_sortOrder) {
		case SortDistance: str = "<DISTANCE"; break;
		case SortFrequency: str = "<FREQ"; break;
		case SortCallsign: str = "<CALLSIGN"; break;
	}
	addText(AL_left, 5, colWhite, str, true);
	addText(AL_left, 7, colWhite, "<PREV PG", true);
	addText(AL_left, 9, colWhite, "<NEXT PG", true);

	int page		= static_cast<int>(_atcListOffset / 6 + 1);
	int numpages	= static_cast<int>((_atcPositions.size()-1) / 6 + 1);
	if(_atcPositions.size() == 0) numpages = 1;
	str = "PG " + itostring(page) + "/" + itostring(numpages);

	// station vs. stationS
	str += " (" + itostring(static_cast<int>(_atcPositions.size())) + ")";

	addText(AL_left, 8, colDarkGreen, str, false);

	// atc frequencies are remembered in _atcFreqs, for easy access when being clicked
	int j;
	for(j = 0; j < 6; ++j) _atcFreqs[j] = "";

	j = 0;
	for(size_t i = _atcListOffset; i < _atcPositions.size() && j < 6; ++i, ++j) {
		//str = "D" + itostring(static_cast<int>(_atcPositions[i].distance)) + " "
		str = _atcPositions[i].callsign;
		addText(AL_right, static_cast<int>(j*2), colDarkGreen, str, false);
		str = _atcPositions[i].frequency;
		addText(AL_right, static_cast<int>(j*2 +1), colWhite, str, true);
		_atcFreqs[j] = _atcPositions[i].frequency;
		_atcNames[j] = _atcPositions[i].callsign;
	}
}

void UiWindow::prepAtcList()
{
	// prepare a new ATC list (in case we just changed the
	// sort order or switched to page COM1/COM2)
	_atcListOffset = 0;
	_atcPositions = xivap.getAtcPositions(_sortOrder);
	for(int i = 0; i < 6; ++i) _atcFreqs[i] = "";
}

bool UiWindow::selectStation(int sk)
{
	sk -= 1; // RSK1-6, but array starts at 0

	// some checks... just for paranoia
	if(length(_atcFreqs[sk]) < 6) return false;
	if(_atcFreqs[sk][3] != '.') return false;

	// which radio are we tuning?
	int radio = 1;
	if(screen == SCREEN_COM2) radio = 2;

	// convert string "123.455" to "123455"
	string s = copy(_atcFreqs[sk], 0, 3) + copy(_atcFreqs[sk], 4, 3);

	// convert "123455" to 123455
	int freq = static_cast<int>(stringtoi(s));
    string name = _atcNames[sk];
	// and tune da shit
	xivap.tuneCom(radio, freq, name);
	return true;
}

void UiWindow::keyEnter()
{
	if(screen != SCREEN_MAIN) return;
	xivap.processLine(editLine);
	editLine = "";
}

void UiWindow::keyBackspace()
{
	if(!onEditPage()) return;
	if(length(editLine) > 0)
		del(editLine, length(editLine) - 1, 1);
}

void UiWindow::keyUpArrow()
{
	if(screen == SCREEN_MAIN) {
		scrollUp();

	}
}

void UiWindow::keyDownArrow()
{
	if(screen == SCREEN_MAIN) {
		scrollDown();

	}
}

void UiWindow::keyLeftArrow()
{
	if(!onEditPage()) return;
}

void UiWindow::keyRightArrow()
{
	if(!onEditPage()) return;
}

void UiWindow::handleKey(int key)
{
	if(!onEditPage()) return;
	if(key == '"') key = '\'';
	if(length(editLine) < 300)
		editLine += key;
}

void UiWindow::dumpKeyboardFocus()
{
	if(_keyboardfocus) {
		XPLMTakeKeyboardFocus(0);
		_keyboardfocus = false;
	}
}

void UiWindow::grabKeyboardFocus()
{
	if(visible()) {
		XPLMTakeKeyboardFocus(window);
		_keyboardfocus = true;
	} else {
		_keyboardfocus = true;
	}
}

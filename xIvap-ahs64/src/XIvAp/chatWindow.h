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

#ifndef CHAT_WINDOW_
#define CHAT_WINDOW_

#include "common.h"

#define CHAT_CLOSE_BUTTON 1
#define CHAT_TOGGLE_BUTTON 2
#define CHAT_DARKEN_BUTTON 3

#include <deque>

class ChatWindow
{
public:
	ChatWindow(const string& callsign, int x = 20, int y = 768 - 20);
	~ChatWindow();

	void addMessage(float* color, string message);

	// callbacks
	void chatDrawCallback(XPLMWindowID inWindowID, void *inRefcon);
	void chatKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
		void *inRefcon, int losingFocus);
	int chatMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon);

	int checkClick(int x, int y);
	int continueClick(int x, int y);
	int endClick(int x, int y);
	int checkButton(int x, int y);
	void buttonClicked(int button);
	int beginDrag(int x, int y);
	int continueDrag(int x, int y);
	int endDrag(int x, int y);

	bool hasCallsign(const string& callsign);
	void grabKeyboardFocus();
	void toggleExtended();

	int getTop() { return top; };
	int getLeft() { return left; };

	bool visible() { return XPLMGetWindowIsVisible(window) != 0; };
	void hide() { XPLMSetWindowIsVisible(window, 0); };

private:
	XPLMWindowID window;
	int fontWidth, fontHeight;
	int columns, rows;
	int scroll;
	int top, left;
	bool _extradark, _keyboardfocus, _extended;
	int getX(int column) { return left + 5 + column * fontWidth; };
	int getY(int line)   { return top - 5 - (line+1) * fontHeight; };

	bool clicking, dragging;
	int mDownX, mDownY, clickButton;

	typedef std::pair<float*, string> line;
	std::deque<line> messages;
	void _appendText(float* color, string line);

	std::vector<string> callsigns;
	string titleLine, editLine, displayLine;
	void processLine(string line);

	const static int maxlines = 300;

	void keyEnter();
	void keyBackspace();
	void keyUpArrow();
	void keyDownArrow();
	void keyLeftArrow();
	void keyRightArrow();
	void handleKey(int key);

	float _lastKeyPress, _cursorTime;
	bool _cursor, _newText;

};

#endif

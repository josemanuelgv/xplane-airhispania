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

#include "XPmessageBox.h"
#include "systemcallbacks.h"
#include "xivap.h"

XPMessageBox::XPMessageBox()
{
	create();
}

void XPMessageBox::show(string message)
{
	string capt1, capt2, capt3;
	string capt4, capt5, capt6; // Añadidas tres líneas más de texto
	capt1 = message;
	capt2 = "";
	capt3 = "";
	capt4 = capt5 = capt6 = "";

	if(length(capt1) > 37) {
		int p = 37;
		while(capt1[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt2 = capt1;
		capt1 = copy(capt1, 0, p);
		del(capt2, 0, p+1);
	}

	if(length(capt2) > 37) {
		int p = 37;
		while(capt2[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt3 = capt2;
		capt2 = copy(capt2, 0, p);
		del(capt3, 0, p+1);
	}

	// Añadidas dos líneas más de texto
	if(length(capt3) > 37) {
		int p = 37;
		while(capt3[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt4 = capt3;
		capt3 = copy(capt3, 0, p);
		del(capt4, 0, p+1);
	}
	if(length(capt4) > 37) {
		int p = 37;
		while(capt4[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt5 = capt4;
		capt4 = copy(capt4, 0, p);
		del(capt5, 0, p+1);
	}
	if(length(capt5) > 37) {
		int p = 37;
		while(capt5[p] != ' ' && p > 15) --p;
		if(p <= 15) p = 37; // no space found in first part of line -> wrap at last column

		capt6 = capt5;
		capt5 = copy(capt5, 0, p);
		del(capt6, 0, p+1);
	}

	XPSetWidgetDescriptor(text1, pconst(capt1));
	XPSetWidgetDescriptor(text2, pconst(capt2));
	XPSetWidgetDescriptor(text3, pconst(capt3));

	// Añadidas tres líneas más de texto
	XPSetWidgetDescriptor(text4, pconst(capt4));
	XPSetWidgetDescriptor(text5, pconst(capt5));
	XPSetWidgetDescriptor(text6, pconst(capt6));

	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void XPMessageBox::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void XPMessageBox::create()
{
	int x = 300;
	int y = 500;
	int x2 = x + 280;
//	int y2 = y - 120;
	int y2 = y - 162;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"Mensaje",	// desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 0);

//	XPCreateWidget(x+10, y-30, x+270, y-85, 1, "", 0, window, xpWidgetClass_SubWindow);
	XPCreateWidget(x+10, y-30, x+270, y-129, 1, "", 0, window, xpWidgetClass_SubWindow);

	y -= 35;
	// bla bla
	text1 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);
	y -= 14;
	text2 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);
	y -= 14;
	text3 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);

	// Añadidas tres líneas más de mensaje para el copyright
	y -= 14;
	text4 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);
	y -= 14;
	text5 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);
	y -= 14;
	text6 = XPCreateWidget(x+20, y, x+260, y-12, 1, "", 0, window, xpWidgetClass_Caption);

	y -= 30;
	// OK
	okButton = XPCreateWidget(x+30, y, x+250, y-24,
					1, "Entendido", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(okButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, msgBoxHandler);
}

int	XPMessageBox::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2)
{
	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		xivap.grabKeyboardFocus();

		// no
		if(inParam1 == (long)okButton) {
			hide();
			return 1;
		}
	}
	return 0;
}

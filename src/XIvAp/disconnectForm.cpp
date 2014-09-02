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

#include "disconnectForm.h"
#include "systemcallbacks.h"
#include "xivap.h"

DisconnectForm::DisconnectForm()
{
	create();
}

void DisconnectForm::show()
{
	if(!XPIsWidgetVisible(window))
		XPShowWidget(window);
	XPBringRootWidgetToFront(window);
}

void DisconnectForm::hide()
{
	if(XPIsWidgetVisible(window))
		XPHideWidget(window);
}

void DisconnectForm::create()
{
	int x = 150;
	int y = 500;
	int x2 = x + 240;
	int y2 = y - 80;

	window = XPCreateWidget(x, y, x2, y2,
					1,			// Visible
					"Disconnect", // desc
					1,			// root
					NULL,		// no container
					xpWidgetClass_MainWindow);
	XPSetWidgetProperty(window, xpProperty_MainWindowHasCloseBoxes, 0);

	// bla bla
	XPCreateWidget(x+10, y-28, x+200, y-40,
					1, "Are you sure that you want to disconnect?",
					0, window, xpWidgetClass_Caption);

	// Yes
	yesButton = XPCreateWidget(x+30, y-48, x+30+80, y-71,
					1, "Yes", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(yesButton, xpProperty_ButtonType, xpPushButton);

	// No
	noButton = XPCreateWidget(x+130, y-48, x+130+80, y-71,
					1, "No", 0, window, xpWidgetClass_Button);
	XPSetWidgetProperty(noButton, xpProperty_ButtonType, xpPushButton);

	// hook up the handler
	XPAddWidgetCallback(window, disconnectFormHandler);
}

int	DisconnectForm::handler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMessage_CloseButtonPushed) {
		hide();
		return 1;
	}
	if(inMessage == xpMsg_PushButtonPressed) {
		xivap.grabKeyboardFocus();

		// no
		if(inParam1 == (intptr_t)noButton) {
			hide();
			return 1;
		}

		// yes
		if(inParam1 == (intptr_t)yesButton) {
			hide();
			xivap.disconnect();
			if(xivap.uiWindow.getPage() == SCREEN_SYSTEM)
				xivap.uiWindow.setPage(SCREEN_MAIN);

			return 1;
		}
	}
	return 0;
}

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

#ifndef REF_INSPECTOR_
#define REF_INSPECTOR_

#include "common.h"

class RefInspector {
public:
	RefInspector();

	void show();
	void hide();

	int handler(XPWidgetMessage inMessage, XPWidgetID inWidget, long inParam1, long inParam2);

private:
	XPWidgetID window;
	XPWidgetID refTextField, enterTextField, resultCaption, readBtn, writeBtn;

	void create();
};

#endif

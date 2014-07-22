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

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "common.h"
#include "XSBComDefs.h"

void MessageHandler(XPLMPluginID inFrom, long inMsg, void *inParam);

// data ref getters
int Handler_getInteger(void *refcon);
int Handler_getString(void *refcon, void *outbuf, int inOffset, int inMaxLength);

// data ref setters
void Handler_setInteger(void *refcon, int value);
void Handler_setString(void *refcon, void *value, int offset, int length);

#endif

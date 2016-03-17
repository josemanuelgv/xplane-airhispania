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

#ifndef _PTPOS_H_
#define _PTPOS_H_

const UInt16 IVAO_EchoRequest		= 0x0401;
const UInt16 IVAO_EchoReply			= 0x0403;

const UInt16 IVAO_PTPOS_Interval	= 0x0501;
const UInt16 IVAO_PTPOS_Position	= 0x0502;
const UInt16 IVAO_PTPOS_Params		= 0x0504;
const UInt16 IVAO_PTPOS_CallsignReq	= 0x0505;
const UInt16 IVAO_PTPOS_Callsign	= 0x0506;

// size of one position packet, including TL headers:
// lat (float), lon (float), alt (float), PBH (uint32), timestamp (uint32), speed (uint16)
#define PTPOS_POSITION_SIZE  24

#endif

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

#include "multiplayerPilot.h"
#include "xivap.h"
#include "helpers.h"

#include <math.h>

using namespace Multiplayer;

/*
void MultiplayerPilot::initPosition(XPMPPlanePosition_t& pos)
{
	pos.size = sizeof(XPMPPlanePosition_t);
	pos.lat = 0;
	pos.lon = 0;
	pos.elevation = 0;
	pos.heading = 0;
	pos.pitch = 0;
	pos.roll = 0;
}
*/

void MultiplayerPilot::initSurfaces(XPMPPlaneSurfaces_t& surfaces)
{
	surfaces.size = sizeof(XPMPPlaneSurfaces_t);
	surfaces.flapRatio = 0;
	surfaces.gearPosition = 0;
	surfaces.slatRatio = 0;
	surfaces.speedBrakeRatio = 0;
	surfaces.spoilerRatio = 0;
	surfaces.thrust = 0;
	surfaces.wingSweep = 0;
	surfaces.yokeHeading = 0;
	surfaces.yokePitch = 0;
	surfaces.lights.timeOffset = rand();
}

void MultiplayerPilot::setLightPattern(string icao)
{
	surfaces.lights.flashPattern = xpmp_Lights_Pattern_Default;

	AircraftDB::Aircraft ac = xivap.aircraftDB.get(icao);
	if(ac.category == 'L')
		surfaces.lights.flashPattern = xpmp_Lights_Pattern_GA;
	else if(icao[0] == 'A' && length(icao) == 4)
		surfaces.lights.flashPattern = xpmp_Lights_Pattern_EADS;    
}

MultiplayerPilot::MultiplayerPilot()
{
	init();
}

MultiplayerPilot::~MultiplayerPilot()
{
}

void MultiplayerPilot::init()
{
	onground = false;
	initSurfaces(surfaces);

	radarStatus.size = sizeof(XPMPPlaneRadar_t);
	radarStatus.code = 0;
	radarStatus.mode = xpmpTransponderMode_Standby;

	callsign = "";
	mtl = "";
	mtlRequested = false;
	XPregistered = false;
	id = NULL;

	timestamp = 0;
	posFrameCalculated = 0;

	lastFsdPos_t = 0;

	p2pmode = 0;
	p2pip = p2pip_int = ipaddress(0);
	p2pport = p2pport_int = 0;
	p2psamelan = false;

	p2pmaxrate_send = -1;
	p2pmaxrate_recv = 0;
	p2pmaxrate_send_real = 0;

	p2plastpossent_t = 0;
	p2plastposseen_t = 0;
	p2plastpostime_t = 0;

	p2psentparams = 0;
	p2psentparams_t = 0;

	p2prate = 0;

	p2prequested = false;
	didrequestp2p = false;

	didseep2ptraffic = false;
	p2pdeadcount = 0;

	lastp2pping_t = 0;
	p2platency = 0;
	lastp2phandshake_t = 0;

	distance = 0;
	pingSerial = 0;
}

double MultiplayerPilot::CalcDistance(double lat, double lon)
{
	const PlanePosition& p = interpolator.lastKnownPosition();

	if(p.pos.lat == 0 && p.pos.lon == 0)
		distance = 0;
	else
		distance = deg2dist(lat, lon, p.pos.lat, p.pos.lon);

	return distance;
}

bool MultiplayerPilot::ignoreLegacyUpdates()
{
	// if I got a p2p packet within the last
	// 5 seconds, ignore the FSD updates.
	if(xivap.watch.getTime() - p2plastposseen_t <= 5000000)
		return true;
	else {
		// last p2p too long ago, forget internal p2p state
		p2plastpostime_t = 0;
		p2plastposseen_t = 0;
		return false;
	}
}

int MultiplayerPilot::RegisterEchoRequest()
{
	lastp2pping_t = xivap.watch.getTime();
	return ++pingSerial;
}

void MultiplayerPilot::HandleEchoReply(int serial)
{
	// drop if echo reply came to soon
	if(serial != pingSerial)
		return;

	UInt32 runtime = (xivap.watch.getTime() - lastp2pping_t) / 2;
	if(p2platency == 0) {

		p2platency = runtime;

	} else {

		if(runtime > p2platency * 4) return;
		p2platency = (p2platency * 15 + runtime) / 16;
	}
}

bool MultiplayerPilot::calculateFrame()
{
	if(posFrameCalculated >= XPLMGetCycleNumber())
		return true;

	if(interpolator.renderPosition(xivap.watch.getTime(), position)) {
		posFrameCalculated = XPLMGetCycleNumber();
		return true;
	}

	return false;
}

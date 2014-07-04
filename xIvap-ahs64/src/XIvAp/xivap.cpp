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

#include "xivap.h"
#include "systemcallbacks.h"
#include "helpers.h"
#include "configFile.h"

#include "SoundGraphics.h"
#include "XPLMPlanes.h"
#include "PlatformUtils.h"

#include "fsdmessage.h"



#ifndef WIN32
#include <stdlib.h>
#include <fcntl.h>
//#include <linux/kd.h>
#include <sys/ioctl.h>
#endif
extern char Graphics;
Xivap xivap;

float colWhite[]		= { 1.0f, 1.0f, 1.0f };
float colLightGray[]	= { 0.8f, 0.8f, 0.8f };
float colBlue[]			= { 0.4f, 0.4f, 1.0f };
float colLightBlue[]	= { 0.7f, 0.7f, 1.0f };
float colRed[]			= { 1.0f, 0.2f, 0.2f };
float colGreen[]		= { 0.1f, 1.0f, 0.1f };
float colDarkGreen[]	= { 0.0f, 0.8f, 0.0f };
float colYellow[]		= { 1.0f, 1.0f, 0.0f };
float colCyan[]			= { 0.0f, 1.0f, 1.0f };
float colGray[]			= { 0.8f, 0.8f, 0.8f };


Xivap::Xivap()
{
	console = NULL;
	focusDumper = NULL;
	uiWindowHotKey = NULL;
	debugWindowHotKey = NULL; // Recipiente para hotkey de la consola de depuración
	nextFSDPosUpdate = 0;
	nextFSDPoll = 0;
	nextStatusUpdate = 0;
	_connectForm = NULL;
	_disconnectForm = NULL;
	_flightplanForm = NULL;
	_messageBox = NULL;
	_refInspector = NULL;
	_fmcForm = NULL;
	menuConnect = 0;
	menuDisconnect = 0;
	menuFlightplan = 0;
	_fplPrefiled = false;
	_fplSend = false;
	_radioCoupled = false;
	_onground = true;
	_setXpdrIdOff = 0;
	posSent = false;
	_identOverride = false;
	_lastPrivSender = "";
	numEngines = 0;
	_lastParams = 0;
	fpl.normalize();
	_useWeather = true;
	_nextWxCheck = 0;
	_nextWxTransition = 0;
	realname = "";
	callsign = "";
	_nextRadioAtisCheck = 0;
	lastNullFrameTime = 0;
	_acType = _acAirline = _acLivery = "";
	_deptAirport = "";
	_destAirport = "";
	_flightRoute = "";
	// frameTiming = 0;
	// highResolutionTime = oldFrameTiming = XPLMGetElapsedTime();

	_renderWindowWidth = 1024; // default window size (for all XP < 8.20?)
	_renderWindowHeight = 768;

#ifdef HAVE_TEAMSPEAK
	_useVoice = true;
	_tsLoaded = false;
	_tsTuned = false;
#else
	_useVoice = false;
#endif

	_useMultiplayer = true;
	_downloadingServerStatus = false;
	
	cfgICAO = "";
//	string dummy1 = "C172";
	string dummy1 = "C182";
	string dummy2 = "";
	setAcType(dummy1, dummy2, dummy2);

	_revision =  SOFTWARE_REVISION;	// "$Rev: 201 $"
	//del(_revision, 0, 6);			// "201 $"  //deleted ask Kenny why the number is lower then 201
	//del(_revision, length(_revision)-2, 2); // "201"

	_stun_server = STUN_SERVER;
	_p2p_enabled = false;
	_p2p_forced_port = 0;
	_p2p_max_sendbps = P2P_DEFAULT_MAX_SENDBPS;
	_p2p_max_recvbps = P2P_DEFAULT_MAX_RECVBPS;

	_plane_resolution = 3;
//	_default_icao = "A320";
	_default_icao = "C182";

	// Niveles de debug iniciados a 0
	debug.debuglevels = 0;

	altpeque = 1; // Altura añadida por defecto para corrección de aviones en tierra
	altgrande = 5; // Altura extra añadida por defecto para aviones grandes para corrección de aviones en tierra

}

Xivap::~Xivap()
{
	if(_connectForm != NULL)
		delete _connectForm;
	if(_disconnectForm != NULL)
		delete _disconnectForm;
	if(_flightplanForm != NULL)
		delete _flightplanForm;
	if(_fmcForm != NULL)
		delete _fmcForm;
	if(_messageBox != NULL)
		delete _messageBox;
	if(_refInspector != NULL)
		delete _refInspector;
}

ConnectForm& Xivap::connectForm()
{
	if(_connectForm == NULL)
		_connectForm = new ConnectForm();
	return *_connectForm;	
}

DisconnectForm& Xivap::disconnectForm()
{
	if(_disconnectForm == NULL)
		_disconnectForm = new DisconnectForm();
	return *_disconnectForm;	
}

XPMessageBox& Xivap::messageBox()
{
	if(_messageBox == NULL)
		_messageBox = new XPMessageBox();
	return *_messageBox;	
}

RefInspector& Xivap::refInspector()
{
	if(_refInspector == NULL)
		_refInspector = new RefInspector();
	return *_refInspector;	
}

FlightplanForm& Xivap::flightplanForm()
{
	if(_flightplanForm == NULL)
		_flightplanForm = new FlightplanForm();
	return *_flightplanForm;	
}

FMCForm& Xivap::fmcForm()
{
	if(_fmcForm == NULL)
		_fmcForm = new FMCForm();
	return *_fmcForm;	
}

void Xivap::XPluginEnable()
{
//	XPluginStart(); // FIXME: Añadido para que realmente habilite/deshabilite el plugin con las opciones enable/disable (PROBAR)
	// do that upon connection
	if(_useMultiplayer) {
		if(!_multiplayer.enable())
			uiWindow.addMessage(colRed, "Error iniciando multijugador: " + _multiplayer.errorMessage(), true, true);
	}

//Quitada descarga de servidores de IVAO
	// start to download the server list
//	_HTTPclient.Download(SERVERSTATUS_URL, getXivapRessourcesDir() + SERVERS_FILENAME, XPLMGetElapsedTime());
//	_downloadingServerStatus = true;

}

void Xivap::XPluginDisable()
{
//	disconnect();
//	XPluginStop(); // FIXME: Cambiado para que realmente se deshabilite el plugin (PROBAR)
}

void Xivap::XPluginStart(){
//	cfgICAO= "";
	// X-Plane Version
	int xplmVersion = 0;
	XPLMHostApplicationID appId;
	XPLMGetVersions(&_xplane_version, &xplmVersion, &appId);
	
	// position references
	gPlaneLat		= XPLMFindDataRef("sim/flightmodel/position/latitude");
	gPlaneLon		= XPLMFindDataRef("sim/flightmodel/position/longitude");
	gPlaneEl		= XPLMFindDataRef("sim/flightmodel/position/elevation");
	gPlanePitch		= XPLMFindDataRef("sim/flightmodel/position/theta");
	gPlaneBank		= XPLMFindDataRef("sim/flightmodel/position/phi");
	gPlaneHeading	= XPLMFindDataRef("sim/flightmodel/position/psi");
	gPlaneSpeed		= XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	gPlaneAGL		= XPLMFindDataRef("sim/flightmodel/position/y_agl");

	gXpdrCode		= XPLMFindDataRef("sim/cockpit/radios/transponder_code");
	gXpdrMode		= XPLMFindDataRef("sim/cockpit/radios/transponder_mode");
	gXpdrId			= XPLMFindDataRef("sim/cockpit/radios/transponder_id");

	gCom1			= XPLMFindDataRef("sim/cockpit/radios/com1_freq_hz");
	gCom2			= XPLMFindDataRef("sim/cockpit/radios/com2_freq_hz");
	gAudioSwitches	= XPLMFindDataRef("sim/cockpit/switches/audio_panel_out");

	// autopilot stuff
	gAPmode			= XPLMFindDataRef("sim/cockpit/autopilot/autopilot_mode");
	gAPstate		= XPLMFindDataRef("sim/cockpit/autopilot/autopilot_state");

	// barometer
	gBarometer		= XPLMFindDataRef("sim/cockpit/misc/barometer_setting");
	gIndicatedAlt	= XPLMFindDataRef("sim/flightmodel/misc/h_ind");

	// plane params references
							// bools
	gGearDeploy		= XPLMFindDataRef("sim/cockpit/switches/gear_handle_status");
	gLightBeacon	= XPLMFindDataRef("sim/cockpit/electrical/beacon_lights_on");
	gLightLanding	= XPLMFindDataRef("sim/cockpit/electrical/landing_lights_on");
	gLightNav		= XPLMFindDataRef("sim/cockpit/electrical/nav_lights_on");
	gLightStrobe	= XPLMFindDataRef("sim/cockpit/electrical/strobe_lights_on");
	gLightTaxi		= XPLMFindDataRef("sim/cockpit/electrical/taxi_light_on");

							// Prop mode: feather=0,normal=1,beta=2,reverse=3
	gEngineMode		= XPLMFindDataRef("sim/flightmodel/engine/ENGN_propmode");
							// Throttle (per engine) as set by user, 0 = idle, 1 = max
	gEngineThrust	= XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro");
	gEngineRunning	= XPLMFindDataRef("sim/flightmodel/engine/ENGN_running");
	

							// Actual flap 1 deplyoment -
							// there is a second one: flap2rat - i'm going to ignore it
	gFlapsDeploy	= XPLMFindDataRef("sim/flightmodel/controls/flaprat");
							// speed brakes
	gSpeedBrakes	= XPLMFindDataRef("sim/flightmodel/controls/sbrkrqst");
	gAcfTailnum		= XPLMFindDataRef("sim/aircraft/view/acf_tailnum");
	gAcfDescription	= XPLMFindDataRef("sim/aircraft/view/acf_descrip");

	// rendering window size
	XPLMDataRef gtmp = XPLMFindDataRef("sim/graphics/view/window_width");
	if(gtmp != NULL)
		_renderWindowWidth = XPLMGetDatai(gtmp);
	gtmp = XPLMFindDataRef("sim/graphics/view/window_height");
	if(gtmp != NULL)
		_renderWindowHeight = XPLMGetDatai(gtmp);

	// tell me how big one character is
	XPLMGetFontDimensions(xplmFont_Basic, &fontWidth, &fontHeight, NULL);

	// create a window:  pass in a rectangle in left, top, right, bottom
	console = XPLMCreateWindow(5, _renderWindowHeight - 18,
							   5 + 10 + CONSOLE_ROWS * fontWidth,
							   _renderWindowHeight - 28 - CONSOLE_LINES * fontHeight,
							   0,						// Start invisible
							   consoleDrawCallback,		// Callbacks
							   emptyKeyCallback,
							   focusdumpMouseCallback,	// dumps the xivap keyboard focus if clicked
							   NULL);					// Refcon - not used

	focusDumper = XPLMCreateWindow(0, _renderWindowHeight, _renderWindowWidth, 0,
								   1,
								   emptyDrawCallback,		// draw Callback
								   emptyKeyCallback,		// key Callback
								   focusdumpMouseCallback, // dumps the xivap keyboard focus if clicked
								   NULL);

	uiWindowHotKey = XPLMRegisterHotKey(XPLM_VK_TAB, xplm_DownFlag,
										  "Conmutar ventana de X-IvAp AHS", uiToggleCallback, NULL);

	//Añadida hotkey (mayús + TAB) para ventana de depuración
	debugWindowHotKey = XPLMRegisterHotKey(XPLM_VK_TAB, xplm_DownFlag | xplm_ShiftFlag,
										  "Conmutar ventana de debug de X-IvAp AHS", debugToggleCallback, NULL);

	string str = string(SOFTWARE_NAME) + " " + SOFTWARE_VERSION + " para " + PLATFORM + " (Rev. " + _revision + ")";
	uiWindow.addMessage(colWhite, str + " listo", true, true);

	uiWindow.pluginStart();
//	uiWindow.addMessage(colWhite, "Pasado uiWindow", true, true);
	msgWindow.pluginStart();
//	uiWindow.addMessage(colWhite, "Pasado msgWindow", true, true);
	Tcasbox.pluginStart(); //added for tcasplot 16/10/2012
//	uiWindow.addMessage(colWhite, "Pasado Tcasbox", true, true);
	aircraftChange();
//	uiWindow.addMessage(colWhite, "Pasado aircraftChange()", true, true);

	// load configuration
	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
#ifdef HAVE_TEAMSPEAK
	str = config.readConfig("SOUND", "VOICE");
	if(str == "1") _useVoice = true;
	else _useVoice = false;
#endif

	str = config.readConfig("PREFERENCES", "COUPLING");
	// compare against 1 -> if it is not set, it defaults to OFF
	if(str == "1") _radioCoupled = true;
	else _radioCoupled = false;

	str = config.readConfig("PREFERENCES", "WEATHER");
	// compare wx against 0 -> if it is not set, it defaults to ON
	if(str == "0") _useWeather = false;
	else _useWeather = true;
	str = config.readConfig("PREFERENCES", "MULTIPLAYER");
	// compare against 0 -> if it is not set, it defaults to ON
	if(str == "0") _useMultiplayer = false;
	else _useMultiplayer = true;
	str = config.readConfig("PREFERENCES", "TAGGING");
	// compare against 0 -> if it is not set, it defaults to ON
	if(str == "0") _useLabels = false;
	else _useLabels = true;
	/* these are added for the modified weathersystem  bvk 16/03/2014*/
	/*extra parameter for the middle windlayer x10*/
	str = config.readConfig("PREFERENCES", "FIXEDWINDLAYER");
	if(str == "0") _WindLayer = false;
	else _WindLayer = true;

	/*extra parameter for the maxvisibilty x10*/
	str = config.readConfig("PREFERENCES", "MAXVISIBILITY");
	if(length(str) > 0 && str != "0") _Maxvisibility=atoi(pconst(str));
	if (_Maxvisibility>100000 || _Maxvisibility<40000) _Maxvisibility=40000; //in km

		/*extra parameter for turbulance*/
	str = config.readConfig("PREFERENCES", "TURBULANCE");
	if(length(str) > 0 && str != "0") _Turbulance=atoi(pconst(str));
	
	if (_Turbulance>30 || _Turbulance<4) _Turbulance=15;
	/*char text[100];
	sprintf(text,"extra parameters turbulance %d, maxvisbility %d  ",_Turbulance,_Maxvisibility);
	if (_WindLayer) XPLMDebugString("Fixed windlayer\r\n"); else XPLMDebugString("Floating Windlayer\r\n");
	XPLMDebugString(text);
	*/
	str = config.readConfig("PREFERENCES", "PRVTMSGSOUND");
	// compare against 0 -> if it is not set, it defaults to ON
	if(str == "0") _usePMSG = false;
	else _usePMSG = true;

	// p2p preferences
	str = config.readConfig("P2P", "ENABLE");
	// compare against 0 -> if it is not set, it defaults to ON
	if(str == "0") _p2p_enabled = false;
	else _p2p_enabled = true;

	// forced p2p port?
	str = config.readConfig("P2P", "FORCED_PORT");
	if(length(str) > 0 && str != "0")
		_p2p_forced_port = atoi(pconst(str));

	// max data rates
	str = config.readConfig("P2P", "MAXSENDBPS");
	if(length(str) > 0 && str != "0")
		_p2p_max_sendbps = atoi(pconst(str));
	str = config.readConfig("P2P", "MAXRECVBPS");
	if(length(str) > 0 && str != "0")
		_p2p_max_recvbps = atoi(pconst(str));

	str = config.readConfig("P2P", "STUNSERVER");
	if(length(str) > 0)
		_stun_server = str;

	// test if icao.dat can be read
	FILE *fp = fopen(getXivapRessourcesDir() + STATIONS_FILE, "r");
	if(fp == NULL) {
		uiWindow.addMessage(colRed, string("Aviso: ") + STATIONS_FILE
			+ " no se encuentra! Compruebe si " + getXivapRessourcesDir()
			+ " existe!", true, true);
	} else {
		fclose(fp);
	}
	_weatherDB.loadPositions(getXivapRessourcesDir() + STATIONS_FILE);

	// Inicializar variable a false para no tener CAVOK por defecto
	cavok = false;
	
	str = config.readConfig("PLANE", "RESOLUTION");
	if(length(str) > 0 && str != "0")
		_plane_resolution = atoi(pconst(str));

	str = config.readConfig("PLANE", "ICAO");
	if(length(str) > 0)
	{
		while(length(str) < 4) str = str + " ";
		cfgICAO = copy(pconst(str),0,4);
	}
	else cfgICAO = "";

#ifdef HAVE_TEAMSPEAK
	// Estado de AhsControl
	_ahsControl = new AhsControl();
	_ahsControl->parse();
	if(_ahsControl->getStatus() == 1){
		string msg = "Estado actual de AhsControl: " + std::to_string(_ahsControl->numDep()) + " dependencias activas.";
		uiWindow.addMessage(colGreen, msg , true, true);
		_ahsControl->saveToFile();
	}
	else
		uiWindow.addMessage(colRed, "Fallo al intentar descargar el estado actual de AhsControl", true, true);
#endif		
}

	bool Xivap::consoleVisible()
{
	int visible = XPLMGetWindowIsVisible(console);
	return (visible == 1);
}

void Xivap::setConsoleVisible(bool value)
{
	int visible = 1;
	if(!value) visible = 0;
	XPLMSetWindowIsVisible(console, visible);
}

void Xivap::XPluginStop()
{
	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	#ifdef HAVE_TEAMSPEAK
	if(config.readConfig("SOUND", "QUITVOICE") == "1")
	{
		XPLMDebugString("X-IvAp: Deteniendo Teamspeak\n");
		tsRemote.Quit();
	}
	#endif
	disconnect();
	XPLMDestroyWindow(console);
	XPLMDestroyWindow(focusDumper);
	uiWindow.pluginStop();
	msgWindow.pluginStop();
	Tcasbox.pluginStop(); //added 16/10/2012

	for(std::vector<ChatWindow*>::iterator it = chatWindows.begin(); it != chatWindows.end(); ++it)
		if(*it != NULL) delete *it;
	chatWindows.clear();

	if (caja.Activada()) caja.Fin(); // Desactiva caja negra si estaba activada
}

void Xivap::connect(const string& callsign_, const string& vid, const string& password,
					const string& realname, bool hideadm, const string& server, const string& port,
					bool voice, bool fmcar_)
{
	// start the weather engine
	_erwin.init();

	callsign = callsign_;
	fmcar = fmcar_;
 
	uiWindow.addMessage(colYellow, "Conectando como " + callsign + " a " + server + "...", true, true);

	FSD::SimulatorType simType = FSD::SIM_XPLANE;
	int majorVersion = _xplane_version / 100;
	if (majorVersion > 70) {
		majorVersion = _xplane_version / 1000;
	}	
	
	if (fmcar) {
		simType = FSD::SIM_FMCAR;
	} else if (majorVersion == 8) {
		simType = FSD::SIM_XP8;
	} else if (majorVersion == 9) {
		simType = FSD::SIM_XP9;
	} else if (majorVersion == 10) {
		simType = FSD::SIM_XP10;
	} else if (majorVersion == 11) {
		simType = FSD::SIM_XP11;
	} else if (majorVersion == 12) {
		simType = FSD::SIM_XP12;
	}
	
	fsd.connectPilot(server, port, callsign, vid, password, hideadm, simType, realname);

	// Quitado aviso de que no hay plan de vuelo 
	// warn user if no flightplan filed
//	if(!_fplPrefiled) {
//		messageBox().show("AVISO: Se esta conectando sin un plan de vuelo. Por favor, envie su plan (Plugins -> X-IvAp -> Send flightplan)");
//	} else {
		//otherwise send actual flightplan aircraft data
// FIXME: Supongo que habría que enviar la información a la lista de pilotos conectados a la red AHS, si existe
//		  Esta información se mete en sendPilotPos como último parámetro extra, siempre que no interfiera en el proceso de la posición en otros clientes
		fsd.sendPlaneInfo(_acType + _acAirline + _acLivery, "dummy");
//	}

	if(_useWeather) {
		fsd.subscribeWeather(); // Aquí se piden los METARs de los ADs del FP (si existen) a la red de AHS
		_erwin.init();
//		_nextWxCheck = XPLMGetElapsedTime() + WEATHER_UPDATE_INTERVAL;
		_nextWxCheck = XPLMGetElapsedTime() + 2.0f;
	}

	_useVoice = voice;
	fsd.sendVoice(_useVoice);

	if(fmcar)
	{
		xivap.flightplanForm().FMcar(true);
	}

	// send plane params on connect
	sendPlaneParams();
	
	nextFSDPosUpdate = XPLMGetElapsedTime() + 5.0f;
	nextFSDPoll = XPLMGetElapsedTime();

	// enable disconnect menu entry
	XPLMEnableMenuItem(xivap.menuEntry, xivap.menuDisconnect, 1);
	// disable connect menu entry
	XPLMEnableMenuItem(xivap.menuEntry, xivap.menuConnect, 0);
	// Icono de conectado
	Graphics |= 4;

// disable this for test
	
	if(_useMultiplayer) {
		if(!_multiplayer.init(_p2p_enabled, _p2p_max_sendbps, _p2p_max_recvbps, _p2p_forced_port, _default_icao)) {
			uiWindow.addMessage(colRed, string("Falló al inicializar Multijugador: ")
				+ _multiplayer.errorMessage());
			_useMultiplayer = false;
		}

		// re-test p2p (user might have hooked his dialup modem just before connecting,
		// or his IP changed... or god knows what else)
//		_multiplayer.testp2p(_stun_server);
	}

	if(_useMultiplayer) {
		if(!_multiplayer.enable())
			uiWindow.addMessage(colRed, "Falló al arrancar Multijugador: " + _multiplayer.errorMessage(), true, true);
	}

#ifdef HAVE_TEAMSPEAK
	if(!_tsLoaded && _useVoice) {
		tsRemote.Start();
		_tsLoaded = true;
	}
#endif
}

void Xivap::FSDPoll()
{
	FSD::Message m;
	do {
		m = fsd.receive();
		if(m.type != _FSD_INVALID_ && m.source != fsd.callsign()) {
			// ignore invalid packets or packets that are from myself

			// handle message / fsd packet
			switch(m.type) {
				case _FSD_ADDPILOT_:
				case _FSD_DELPILOT_:
				case _FSD_PILOTPOS_:
				case _FSD_PLANEINFO_:
				case _FSD_PLANEPARAMS_:
				case _FSD_FASTPILOT_:
				case _FSD_CUSTOMPILOT_: // Añadido para interpretar comandos #SB para recuperación de CSLs de la red de AHS
					if(_useMultiplayer)
						_multiplayer.eatThis(m);
					break;

				case _FSD_WEAGENERAL_:
				case _FSD_WEAWINDLAY_:
				case _FSD_WEATEMPLAY_:
				case _FSD_WEACLOUDLAY_:
				case _FSD_WEAVISLAY_:
				case _FSD_WEADATA_RC_AHS_: // Añadido para interpretar el tiempo desde los METARs de la red AHS
					handleWxData(m);
					break;

				case _FSD_TEXTMESSAGE_:
				
					handleTextMessage(m);
					
					break;
				case _FSD_ATCPOS_:
					
					updateATC(m);
					break;
				case _FSD_ADDATC_:
					addATC(m);
					break;
				case _FSD_DELATC_:
					delATC(m);
					break;
				case _FSD_WINDVAR_:	// wind variation -> IGNORED -> FIXME: Intentar incorporar este dato, averiguando cómo interpretarlo
					break;
				case _FSD_INFOREQ_:
				case _FSD_INFOREPLY_:
					handleInfoReply(m);
					break;
				case _FSD_CHANGEFP_:
					handleFPchange(m);
					break;
				case _FSD_ERROR_:
					handleError(m);
					break;
				case _FSD_KILL_:
					handleKill(m);
					break;
				case _FSD_WEADATA_:
					handleWxReply(m);
					break;
				case _FSD_ATIS_:
					handleAtisReply(m);
					break;

				default:
					addText(colCyan, "Unhandled packet: " + m.encoded, true, true);
					break;
			} // switch

		} // if packet not invalid

	} while(m.type != _FSD_INVALID_);

	if(!fsd.connected()) {
		// lost connection?
		disconnect();
	} else {
		// prepare for next call
		nextFSDPoll += _FSD_POLL_INTERVAL_;
		if(nextFSDPoll < XPLMGetElapsedTime()) // can happen on pause
			nextFSDPoll = XPLMGetElapsedTime() + _FSD_POLL_INTERVAL_;
	}
}

double Xivap::flightlevelft() const
{
	float h_ind = XPLMGetDataf(gIndicatedAlt);
	float baro = XPLMGetDataf(gBarometer);

	float result = h_ind - (baro - 29.92f) * 1000;
	return result;
}

void Xivap::sendPosUpdate()
{
	updateStatus();

	IdentMode send_ident = xpdrMode;
	if(_identOverride) send_ident = IdentID;

	fsd.sendPilotPos(send_ident, xpdrCode, static_cast<float>(lat), static_cast<float>(lon),
		static_cast<int>(elevationft()),
		static_cast<int>(speed), -pitch, -bank, heading, _onground, static_cast<int>(flightlevelft()));

//	sendPlaneParams(); // Añadido para enviar los plane params justo después de la posición

	nextFSDPosUpdate += 5.0;
	if(nextFSDPosUpdate < XPLMGetElapsedTime()) // can happen on pause
		nextFSDPosUpdate = XPLMGetElapsedTime() + 5.0f;

	if(_fplPrefiled) {
		if(fsd.sendFlightplan(fpl))
			_fplPrefiled = false;
	}

	// reset the 0 frame timestamp
	int n = msecs(now());
	int diff = n - lastNullFrameTime;
	if(diff > 4800 && diff < 5200)
		lastNullFrameTime += 5000;
	else
		lastNullFrameTime = n;

	posSent = true;
}

UInt32 Xivap::getParams()
{
	updateParams();
	return _planeParams.params;
}

const FSD::PlaneParams& Xivap::updateParams()
{
	static float delay = 0;
	// update at most once ever 0.2 seonds
	if(XPLMGetElapsedTime() < delay + 0.2f)
		return _planeParams;
	delay = XPLMGetElapsedTime();

	// update params
	_planeParams.gear			= XPLMGetDatai(gGearDeploy);
	_planeParams.beaconLight	= XPLMGetDatai(gLightBeacon);
	_planeParams.landLight		= XPLMGetDatai(gLightLanding);
	_planeParams.navLight		= XPLMGetDatai(gLightNav);
	_planeParams.strobeLight	= XPLMGetDatai(gLightStrobe);
	_planeParams.taxiLight		= XPLMGetDatai(gLightTaxi);

	int intArray[8];
	float floatArray[8];
	float average;

	// Prop mode: feather=0,normal=1,beta=2,reverse=3
	XPLMGetDatavi(gEngineMode, intArray, 0, 8);
	if(intArray[0] == 3) _planeParams.thrustReversers = 1;
	else _planeParams.thrustReversers = 0;

	_planeParams.engine1Running = 0;
	_planeParams.engine2Running = 0;
	_planeParams.engine3Running = 0;
	_planeParams.engine4Running = 0;

	if(numEngines > 0) {
		// Throttle (per engine) as set by user, 0 = idle, 1 = max
		XPLMGetDatavf(gEngineThrust, floatArray, 0, 8);
		average = 0;
		for(int i = 0; i < numEngines; ++i) {
			average += floatArray[i];
		}
		average = average / numEngines;
		_planeParams.thrustRatio = static_cast<unsigned int>(average * 7);

		XPLMGetDatavi(gEngineRunning, intArray, 0, 8);
		for(int i = 0; i < numEngines && i < 4; ++i) {
			switch(i) {
				case 0: _planeParams.engine1Running = intArray[i]; break;
				case 1: _planeParams.engine2Running = intArray[i]; break;
				case 2: _planeParams.engine3Running = intArray[i]; break;
				case 3: _planeParams.engine4Running = intArray[i]; break;
			}
		}

	} else {
		_planeParams.thrustRatio = 0;
	}

	average = XPLMGetDataf(gFlapsDeploy);
	_planeParams.flapsRatio = static_cast<unsigned int>(average * 7);
	average = XPLMGetDataf(gSpeedBrakes);
	_planeParams.speedbrakeRatio = static_cast<unsigned int>(average * 7);

	return _planeParams;
}

void Xivap::sendPlaneParams()
{
	if(XPLMGetElapsedTime() < _lastParams + PARAMS_MINDELAY)
		return;

	_lastParams = XPLMGetElapsedTime();
	updateParams();

//	if (online())
//	{
		std::vector<string> lista = _multiplayer.listaConectados(); // Obtiene la lista de indicativos de pilotos conectados a la red
		for (int i = 0; i < lista.size(); ++i) // Envía lista de parámetros del avión a cada piloto conectado
		{
			if(fsd.setParams(_planeParams, lista[i]))
				_lastParams = XPLMGetElapsedTime();
		}
//	}
}

void Xivap::updatePosition(PlanePosition* position)
{
	lat			= XPLMGetDatad(gPlaneLat);
	lon			= XPLMGetDatad(gPlaneLon);
	el			= XPLMGetDatad(gPlaneEl);
	pitch		= XPLMGetDataf(gPlanePitch);
	bank		= XPLMGetDataf(gPlaneBank);
	heading		= XPLMGetDataf(gPlaneHeading);
	agl			= XPLMGetDataf(gPlaneAGL);
	speed		= XPLMGetDataf(gPlaneSpeed) * 1.94384449f; // speed is in meters/sec

	if(position != NULL) {
		position->timestamp = position->p_timestamp = watch.getTime();
		position->latency = 0;
		position->pos.elevation = el;
		position->pos.heading = heading;
		position->pos.lat = lat;
		position->pos.lon = lon;
		position->pos.pitch = pitch;
		position->pos.roll = bank;
	}

	_onground = agl < 1.5; // anything that does not fly higher than 1.50cm is "on ground"
	groundalt = el - agl;
}

void Xivap::updateStatus()
{
	updatePosition();

	xpdrCode	= XPLMGetDatai(gXpdrCode);
	int xm		= XPLMGetDatai(gXpdrMode);
	int xpdrId	= XPLMGetDatai(gXpdrId);

	// Transponder mode (off=0,stdby=1,on=2,test=3)
	switch(xm) {
		case 0: case 1: xpdrMode = IdentSby; break;
		case 2: case 3: default: xpdrMode = IdentC; break;
	}
	if(xpdrId != 0) xpdrMode = IdentID; // if identing -> ident

	// that value is 10 if com1 is lit, and 11 if com2 is lit
	// but somehow, you can also lite up nav1 and nav2, which doesn't make much sense,
	// because only ONE of com1, com2, nav1 and nav2 can be lit at the same time?
	int i = XPLMGetDatai(gAudioSwitches);

	//addText(colYellow, string("audio panel mode: ") + itostring(i));

	// I have no idea if this is correct...
	// for now i'm going to assume that if this value is 11, com2 is active, else it is com1
	int act;
	if(i == 11) act = 2;
	else act = 1;
	if(_activeRadio != act) setComActive(act);

	int freq = XPLMGetDatai(gCom1);
	// handle a possible 6 digit frequency
	if(_com1freq > 99999) {
		if(freq != _com1freq / 10)
			tuneCom(1, freq);
	} else {
		if(freq != _com1freq)
			tuneCom(1, freq);
	}

	freq = XPLMGetDatai(gCom2);
	// handle a possible 6 digit frequency
	if(_com2freq > 99999) {
		if(freq != _com2freq / 10)
			tuneCom(2, freq);
	} else {
		if(freq != _com2freq)
			tuneCom(2, freq);
	}

	if(XPLMGetElapsedTime() > _lastParams + PARAMS_MINDELAY)
		sendPlaneParams();

	caja.Graba(); // Añadido para grabación de registro en caja negra, si procede

	uiWindow.refresh();

	// kill all hidden chat windows
	std::vector<ChatWindow*>::iterator it = chatWindows.begin();
	while(it != chatWindows.end()) {
		if(!(*it)->visible()) {
			chatWindows.erase(it);
			it = chatWindows.begin();
		} else {
			++it;
		}
	}

	nextStatusUpdate += 1.0f;
	if(nextStatusUpdate < XPLMGetElapsedTime()) // can happen on pause
		nextStatusUpdate = XPLMGetElapsedTime() + 1.0f;
}

bool Xivap::autopilotOn() const
{
	int on = XPLMGetDatai(gAPmode);
	int flags = XPLMGetDatai(gAPstate);
	return (on == 2 && flags != 0); // (off=0, flight director=1, on=2)
}

string Xivap::autopilotState() const
{
	int flags = XPLMGetDatai(gAPstate);
/*
This dataref summarizes all of the possible state for the autopilot.
The return value is a series of bit flags, as follows:

 1	autothrottle
 2	heading hold
 4	wing leveler on
 8	airspeed hold
 16	vvi climb
 32	altitude hold armed*
 64	level change
 128	auto-sync
 256	hnav armed
 512	hnav engaged
 1024	gls armed**
 2048   gls engaged**
 (X-Plane 8.11 and newer only)
 4096   fms vnav armed
 8192   fms vnav engaged
 16384  altitude hold engaged
*/
	string result = "";

	if(flags & 0x00000001)	// autothrottle
		result += "ATHR ";
	if(flags & 0x00000002)	// heading
		result += "HDG ";
	if(flags & 0x00000004)	// wing leveler
		result += "WNGLVL ";
	if(flags & 0x00000008)	// airspeed hold
		result += "IAS ";
	if(flags & 0x00000010)	// vvi climb
		result += "VVICLMB ";
	if(flags & 0x00000020)	// altitude hold armed
		result += "ALT ";
	if(flags & 0x00000040)	// level change
		result += "LVLCH ";
	if(flags & 0x00000080)	// auto synch
		result += "AUTOSYNC ";
	if(flags & 0x00000100)	// HNAV armed
		result += "HNAVarm ";
	if(flags & 0x00000200)	// HNAV
		result += "HNAV ";
	if(flags & 0x00000400)	// GLS armed
		result += "GLSarm ";
	if(flags & 0x00000800)	// GLS
		result += "GLS ";
	if(flags & 0x00001000)	// VNAV armed
		result += "VNAVarm ";
	if(flags & 0x00002000)	// VNAV
		result += "VNAV ";
	if(flags & 0x00004000)	// altitude hold
		result += "ALT ";

	return result;
}

void Xivap::tuneCom(int radio, int freq, string name)
{
	if(name == "") 	name = freq2name(freq);
	AtcPosition p = _atcList.findName(name);

        // if we failed to find a name, let's see whether we received the freq
        // from XP in 25kHz format and attempt some 8.33 frequencies close to
        // it.
        string freqStr = freq2str(freq);
        if(!p.isValid() && freq < 99999 && is25kHzFreq(freqStr)) {
            AtcPosition q;
            int lastDigit = freq % 10;
            int freq833 = freq * 10;
            for(int i = 0; i < 4; i++) {
                if(0 == i && (lastDigit == 2 || lastDigit == 7)) {
                    freq833 += 10;
                } else {
                    freq833 += 5;
                }
                freqStr = freq2str(freq833);
                q = _atcList.findFreq(freqStr, lat, lon);
                if(q.isValid()) {
                    if(!p.isValid() || q.distance < p.distance)
                        p = q;
                        freq = freq833;
                        name = freq2name(freq);
                }
				if (debug.teamspeak)
                addText(colYellow, "Frecuencia 8.33 kHz estimada: " + freqStr, true, true);
            }
        } 

	if(radio == 1) com1name=name;
	else com2name=name;

	#ifdef HAVE_TEAMSPEAK
	if (online()) // Sólo sintonizar la radio en el TS si está conectado
	{
//		if(_activeRadio == radio && freq2str(freq) == UNICOM_FREQ) tsRemote.Disconnect(); //switch unicom -> disconnect TS
		if(_activeRadio == radio && freq2str(freq) == UNICOM_FREQ)
		{
				tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), UNICOM_NAME);
		}
		else // TODO: ¿Qué hacer con el canal 121.500 de emergencia?
		{
			if(p.isValid()) {
				if(_activeRadio == radio)  {
					switch(radio) {
					case 1:
					fsd.sendInfoRequest(com1name, _FSD_INFOREQ_ATIS_);
					_atcList.setAtis(com1name, true);
					// Añadido para cambiar al canal de TS de la dependencia correspondiente de AHS
					tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), p.callsign);		
					break;
					case 2:
					fsd.sendInfoRequest(com2name, _FSD_INFOREQ_ATIS_);
					_atcList.setAtis(com2name, true);
					// Añadido para cambiar al canal de TS de la dependencia correspondiente de AHS
					tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), p.callsign);
					break;
					}
					caja.atc = p.callsign;
				}
			}  
//			if(_activeRadio == radio && copy(com1name,3,1) == ".") tsRemote.Disconnect(); //tune com1 where there is no atc -> disconnect TS
			else 
			{
				caja.atc = "";
				if(_activeRadio == radio && copy(com1name,3,1) == ".")
				{
//					tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), TS_CANAL_GENERAL);
					tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), "");
				}
//				if(_activeRadio == radio && copy(com2name,3,1) == ".") tsRemote.Disconnect(); //tune com2 where there is no atc -> disconnect TS
				if(_activeRadio == radio && copy(com2name,3,1) == ".")
				{
//					tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), TS_CANAL_GENERAL);
					tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), "");
				}
			}
		}
	} // if online
	#endif


	switch(radio) {
		case 1:
			_com1freq = freq;

			// X-Plane uses 5 digits in frequencies, and it always sets the last 5 digits.
			// If the freq has 6 digits, drop the last one
			if(freq > 99999) freq /= 10;

			XPLMSetDatai(gCom1, freq);
			break;
		case 2:
			_com2freq = freq;

			// X-Plane uses 5 digits in frequencies, and it always sets the last 5 digits.
			// If the freq has 6 digits, drop the last one
			if(freq > 99999) freq /= 10;

			XPLMSetDatai(gCom2, freq);
			break;
	}
}

void Xivap::addText(float *color, const string& text, bool logToScreen, bool logToFile)
{
	if(logToScreen) {
		while(consoleText.size() >= CONSOLE_LINES)
			consoleText.pop_back();

		consoleText.push_front(line(color, text));
	}

//	if(logToFile)	{}//XPLMDebugString("X-IvAp: " + text + "\n");//disabled for the moment, to much noise in the logfile
	if(logToFile)	XPLMDebugString("X-IvAp: " + text + "\n");
}

void Xivap::checkRadioAtis()
{
	// lets see if we need an atis for either of the
	// radio stations we are currently tuned to

	string radioName;
	if(_activeRadio == 1 || _radioCoupled) {
		radioName = freq2name(_com1freq);
		AtcPosition p = _atcList.findName(radioName);
		if(p.isValid() && !p.haveAtis) {
			fsd.sendInfoRequest(radioName, _FSD_INFOREQ_ATIS_);
			_atcList.setAtis(radioName, true);
		}
	}

	if(_activeRadio == 2 || _radioCoupled) {
		radioName = freq2name(_com2freq);
		AtcPosition p = _atcList.findName(radioName);
		if(p.isValid() && !p.haveAtis) {
			fsd.sendInfoRequest(radioName, _FSD_INFOREQ_ATIS_);
			_atcList.setAtis(radioName, true);
		}
	}
}

string Xivap::comStr(int radio)
{
	if(copy(com1name,3,1) == ".") com1name=freq2name(_com1freq);
	if(copy(com2name,3,1) == ".") com2name=freq2name(_com2freq);
	string result;
	if(radio == 1) result = com1name;
	else result = com2name;

	if(_activeRadio == radio) result = result + " <T/R>";
	else if(_radioCoupled) result = result + " <RX>";

	return result;
}

string Xivap::freq2str(int freq)
{
	// no name, return the frequency as string
	string s = itostring(freq);			// 124
	int ls = length(s);
	string lastdigit = copy(s,4,1);
	string result = copy(s, 0, 3) + ".";   // 124.
	del(s, 0, 3);
    result += s;					// 124.40	
	if ( ( lastdigit == "2" || lastdigit == "7" ) && ls==5 ) result += "5";
	while(length(result) < 7) result += "0";
	return result;
}

string Xivap::freq2name(int freq)
{
	if(freq == UNICOM_FREQ_NUM) return UNICOM_NAME;

	// if freq matches an ATC station in range, map it to the name here
	string f = freq2str(freq);
	AtcPosition p = _atcList.findFreq(f, lat, lon);
        if(p.isValid()) return p.callsign;
        else return f;          // if nothing matches, just return the frequency
}

string Xivap::getActiveRadioFreq()
{
	int freq = _com1freq;
	if(_activeRadio == 2) freq = _com2freq;
	return freq2str(freq);
}

int Xivap::listeningToRadioFreq(string freq)
{
	if(freq == "121.500") return 3;

	// get radio freqs and make them equal in length
	string f1 = freq2str(_com1freq); while(length(f1) < 7) f1 += "0";
	string f2 = freq2str(_com2freq); while(length(f2) < 7) f2 += "0";

	//addText(colWhite, "am i listening to this freq? " + freq + " " + f1 + " " + f2);

	while(length(freq) < 7) freq += "0";

	if(_activeRadio == 1 || _radioCoupled)
		if(f1 == freq) return 1;

	if(_activeRadio == 2 || _radioCoupled)
		if(f2 == freq) return 2;

	return 0;
}

void Xivap::handleTextMessage(const FSD::Message& m)
{
	float *color = colWhite;
	string message = m.tokens[0];

	if(m.source == _FSD_SERVERDEST_) color = colLightBlue; // server message
	else {
		// radio chatter
		if(m.dest[0] == '@') {
			string freq = m.dest;
			del(freq, 0, 1);
			string f = "1" + freq;
			freq = copy(f, 0, 3) + ".";
			del(f, 0, 3);
			freq += f;

			bool to_me = false;

			int com = listeningToRadioFreq(freq);
			if(com > 0) {

				// radio message to me
				if(copy(message, 0, length(fsd.callsign())) == fsd.callsign()) to_me = true;

				if(com == _activeRadio) {
					color = colGreen;
					message = m.source + "> " + message;
					Playsound("text.wav");
				} else if (com == 3) { // guard
					color = colYellow;
					message = m.source + " (guard)> " + message;
					Playsound("textpilot.wav");
				} else {
					color = colDarkGreen;
					message = m.source + " (#2)> " + message;
					Playsound("textpilot.wav");
				}

				if(to_me) color = colYellow;

			} else {
				// not listening to this - ignore it
				message = "";
			}
		} else
		if(m.dest == "*S") {
			// wallop
			color = colWhite;
			message = m.source + " [wallop]> " + message;
			Playsound("text.wav");

		} else
		if(m.dest == "*") {
			// broadcast
			color = colWhite;
			message = m.source + " [bcast]> " + message;
			Playsound("text.wav");
		} else
		if (message == "Esta es informacion") // Recuperar qué información ATIS hay después de ":"
		{
			if (m.tokens.size() > 1)
			{
				atisV_ant = atisV;
				atisV = m.tokens[1];
				message = message + ": " + atisV;
			}
		}
		if(m.dest == fsd.callsign()) {
			// private message
			bool inChatWindow = false;
			// if we have a chat window with this guy, display the message only there
			std::vector<ChatWindow*>::iterator it = chatWindows.begin();
			while(it != chatWindows.end()) {
				// put the message into all chat windows concerned
				if((*it)->hasCallsign(m.source)) {
					inChatWindow = true;
					(*it)->addMessage(colGreen, m.source + "> " + message);
					if (xivap.usingPMSG()) Playsound("selcal.wav");
				}
				++it;
			}

			if(inChatWindow) {
				message = "";
			} else {
				message = m.source + " [prv]> " + message;
				color = colRed;
				_lastPrivSender = m.source;
				if (xivap.usingPMSG()) 	Playsound ("selcal.wav"); //this one 
			}
		}
	}

	if(length(message) > 0) {
		uiWindow.addMessage(color, message);
		msgWindow.addMessage(color, message);
	}
}

void Xivap::handleInfoReply(const FSD::Message &m)
{
	if(m.tokens[0] == "C?") { // COM request

		string freq = getActiveRadioFreq(); // returns 134.35
		while(length(freq) < 7) freq = freq + "0";   // r is now 134.350
		fsd.sendInfoRequestReply(m.source, "C?", freq);

	}
	else if(m.tokens[0] == "FP") { // Resend flightplan
		if(fpl.isValid())
			sendFlightplan();
	}
	else if(m.tokens[0] == "SEL") { // Selcal
		Selcal(m.source);
	}
	// Eliminado para compatibilidad con red de AHS
//	else if(m.tokens[0] == "P2P") { // p2p request
//		if(m.tokens.size() > 2) {
//			// $CQAAAA:BBBB:P2P:<mode>:<application>[:<ip>:<port>]
//			if(m.tokens[2] == P2P_POS_PROTOCOL)
//				_multiplayer.eatThis(m);
//		}
//	}
	//FIXME: Añadir respuesta a otras peticiones como "CAPS"
}

void Xivap::setRadioCoupled(bool value) {
	_radioCoupled = value;

	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PREFERENCES", "COUPLING", (_radioCoupled == 1 ? "1" : "0"));
	config.save(filename);
}

void Xivap::handleError(const FSD::Message &m)
{
	// source dest number extrainfo text
	string errstr = "Error: ";
	if(m.tokens.size() > 0) errstr += m.tokens[0];
	if(m.tokens.size() > 1) errstr += " " + m.tokens[1];
	if(m.tokens.size() > 2) errstr += " " + m.tokens[2];
	if(m.tokens[0] != "009") uiWindow.addMessage(colRed, errstr, true, true); // No mostrar en ventana el error 009 "No such weather profile"
	else if (debug.weather > 2) uiWindow.addMessage(colRed, errstr, true, true); // FIXME: ¿Sería posible sólo mostrarlo en log y no en ninguna ventana sin causar deterioro de rendimiento?
}

void Xivap::handleKill(const FSD::Message &m)
{
	// source dest reason
	// $!!EBBR_APP:N1697J:YOU ARE ANNOYING
	string reason = "(Ninguna razon dada)";
	if(m.tokens.size() > 0)	reason = m.tokens[0];
	uiWindow.addMessage(colRed, "Expulsado: " + reason, true, true);
}

void Xivap::disconnect()
{
	if(fsd.connected())
		fsd.disconnectPilot();
	Playsound("disconnect.wav");
	uiWindow.addMessage(colWhite, "Desconectado", true, true);
	msgWindow.addMessage(colWhite, "Desconectado");
	nextFSDPoll = 0;
	nextFSDPosUpdate = 0;

	_atcList.clear();
	_weatherDB.clear();
	_multiplayer.clear();

	// re-initialize wx engine (stop weather changing)
	_erwin.init();

	// disable disconnect menu entry
	XPLMEnableMenuItem(xivap.menuEntry, xivap.menuDisconnect, 0);
	// enable connect menu entry
	XPLMEnableMenuItem(xivap.menuEntry, xivap.menuConnect, 1);
	// Icono de desconectado
	Graphics&=~4;

#ifdef HAVE_TEAMSPEAK
// No hacer nada con el TS hasta que no se sintonice la radio
//	if(_tsLoaded && _useVoice && _tsTuned && !_tsQuit)
//		tsRemote.Disconnect();
#endif

	// reset the plane info in fsd in case the user wants to reconnect again
    xivap.flightplanForm().FMcar(false);
	xivap.fmcar = false;
// Quitado para la conexión de red de AHS
//	fsd.sendPlaneInfo(_acType + _acAirline + _acLivery);
	fsd.sendPlaneInfo(_acType + _acAirline + _acLivery,"dummy");

	// if we had a flightplan, remember it for the next connect and automatically
	// send it when we connect again
	if(_fplSend) _fplPrefiled = true;
	com1name =  freq2name(_com1freq);
	com2name  = freq2name(_com2freq);; //Removes ATC
}

void Xivap::sendFlightplan()
{
	_fplSend = true;
	Graphics|=8; //added to glue to textures
	if(fsd.connected()) {
		fsd.sendFlightplan(fpl);

		// Pre-archivar los METARs de los ADs del plan de vuelo
		fsd.unsubscribeWeather();
		fsd.subscribeWeather();

		uiWindow.addMessage(colYellow, "Plan de Vuelo enviado", true, true);
	} else {
		_fplPrefiled = true;
		
		uiWindow.addMessage(colYellow, "Plan de vuelo pre-archivado", true, true);
	}
}

#define HANDLECHG(message) { changed = true; if(length(changes) > 0) changes += ", "; changes += message; }

void Xivap::test(string linea) // Diversos tests relacionados con la simulación de recepción de mensajes desde la red
{
	
	linea = "@*:" + trim(strupcase(linea)); // prepara texto para utilizar FSD::Message.decompose
	FSD::Message m, mlinea;
	mlinea.decompose(linea);
	
	if (mlinea.dest == "MSG") // Primer parámetro "MSG" para simular recepción de un mensaje "tal cual" desde la red
	{
		del(linea,0,7); // Borra "@*:MSG:" de la línea, para dejar únicamente el mensaje a enviar
		m = fsd.testreceive(linea);
//FIXME: DEBUG
		addText(colDarkGreen, "Pasado testreceive()", true, true);
	}
	else if (mlinea.dest == "PPOS") // Mandar posición de presunto piloto de la red, para visualizar un avión
	{
		m.type = _FSD_PILOTPOS_;
		m.source = "S";
		m.dest = "AHS0000";
		m.tokens.push_back("7400");
		m.tokens.push_back("1");
		m.tokens.push_back(ftoa(lat));
		m.tokens.push_back(ftoa(lon + 0.002f)); // Varía un poco la posición respecto a la propia
//		if (mlinea.tokens.size() > 1) m.tokens.push_back(itostring((int)(elevationft() + 1 + atof(mlinea.tokens[1])))); // 2º parámetro: altitud extra añadida para aviones grandes
//		else m.tokens.push_back(itostring((int)(elevationft() + 1)));
//		if (mlinea.tokens.size()> 1) altgrande = atof(mlinea.tokens[1]);
		m.tokens.push_back(itostring((int)(elevationft() + 1)));
		m.tokens.push_back("0");
		m.tokens.push_back("4290774782"); // PBH de avión cogido al azar de una traza de la red
		m.tokens.push_back("0");
	}
	else if (mlinea.dest == "PI")
	{
		m.type = _FSD_CUSTOMPILOT_;
		m.source = "AHS0000";
//		if (mlinea.tokens.size() > 1) m.dest = mlinea.tokens[1]; // 2º parámetro: callsign
//		else m.dest = "AHS120D";
		m.dest = fsd.callsign();
		m.tokens.push_back("PI");
		m.tokens.push_back("X");
		m.tokens.push_back("0");
		if (mlinea.tokens.size() > 2) m.tokens.push_back(mlinea.tokens[3]); // 3er. parámetro: el segundo número del comando (de momento puede ser 0 ó 1)
		else m.tokens.push_back("0");
		if (mlinea.tokens.size() > 0) m.tokens.push_back("~" + mlinea.tokens[0]); // 1er. parámetro: ICAO de la aeronave
		else m.tokens.push_back("~" + _default_icao);
		if (mlinea.tokens.size()> 1) altgrande = atof(mlinea.tokens[1]); // 2º parámetro: Altitud en tierra extra añadida para aviones grandes (TODO: Hacerlo configurable, por cada MTL si fuera posible)

		string ac0 = m.tokens[4]; // ICAO de aeronave
		if (ac0[0] == '~') del(ac0,0,1); // Borra el primer carácter "~" que suele mandar la red (no sé si significará algo)
		m.tokens[4] = ac0;
	}
	else if (mlinea.dest == "PP")
	{
		m.type = _FSD_CUSTOMPILOT_;
		m.source = "AHS0000";
//		if (mlinea.tokens.size() > 1) m.dest = mlinea.tokens[1]; // 2º parámetro: callsign
//		else m.dest = "AHS120D";
		m.dest = "*";
		m.tokens.push_back(_FSD_CUSTOMPILOT_PLANEPARAMS_);
		if (mlinea.tokens.size() > 0) 
		{
			m.tokens.push_back(mlinea.tokens[0]); // 1er. parámetro: parámetros del avión en forma numérica
		}
	}

		if(m.type != _FSD_INVALID_ && m.source != fsd.callsign()) {
			// ignore invalid packets or packets that are from myself

//FIXME:DEBUG
			addText(colRed, "No es invalid y no soy yo la fuente", true, true);

			// handle message / fsd packet
			switch(m.type) {
				case _FSD_ADDPILOT_:
				case _FSD_DELPILOT_:
				case _FSD_PILOTPOS_:
				case _FSD_PLANEINFO_:
				case _FSD_PLANEPARAMS_:
				case _FSD_FASTPILOT_:
				case _FSD_CUSTOMPILOT_: // Añadido para interpretar comandos #SB para recuperación de CSLs de la red de AHS
					if(_useMultiplayer)
						_multiplayer.eatThis(m);
					break;

				case _FSD_WEAGENERAL_:
				case _FSD_WEAWINDLAY_:
				case _FSD_WEATEMPLAY_:
				case _FSD_WEACLOUDLAY_:
				case _FSD_WEAVISLAY_:
				case _FSD_WEADATA_RC_AHS_: // Añadido para interpretar el tiempo desde los METARs de la red AHS
					handleWxData(m);
					break;

				case _FSD_TEXTMESSAGE_:
				
					handleTextMessage(m);
					
					break;
				case _FSD_ATCPOS_:
					
					updateATC(m);
					//FIXME:DEBUG
					addText(colDarkGreen, "ATCPOS -> Pasado updateATC(m)", true, true);

					break;
				case _FSD_ADDATC_:
					addATC(m);
					break;
				case _FSD_DELATC_:
					delATC(m);
					break;
				case _FSD_WINDVAR_:	// wind variation -> IGNORED -> FIXME: Intentar incorporar este dato, averiguando cómo interpretarlo
					break;
				case _FSD_INFOREQ_:
				case _FSD_INFOREPLY_:
					handleInfoReply(m);
					break;
				case _FSD_CHANGEFP_:
					handleFPchange(m);
					break;
				case _FSD_ERROR_:
					handleError(m);
					break;
				case _FSD_KILL_:
					handleKill(m);
					break;
				case _FSD_WEADATA_:
					handleWxReply(m);
					break;
				case _FSD_ATIS_:
					handleAtisReply(m);
					break;

				default:
					addText(colCyan, "Unhandled packet: " + m.encoded, true, true);
					break;
			} // switch

		} // if packet not invalid

}

/*
void Xivap::test(void)
{
//	Flightplan new_fp = fsd.getFlightplan(m);
	bool changed = false;
	string changes = "";
	string cruisespeed="080";
	string departure="GMMX";
	string destination="DAAG";
	string hoursenroute="10";
	string minenroute="30";
	string remarks="KLMVA";
	string route="ABDOR AGOVA FES CHE";
	string alternate="ZZZZ";
	string deptimeest="1200";
	string deptimeact="1200";
	string cruisealt="F310";
	string new_alttype="F";
	char text[100];
/*	
	XPLMDebugString("cruisespeed");
	XPLMDebugString(new_fp.cruisespeed);
	XPLMDebugString("\r\ncruisealitude");
	XPLMDebugString(new_fp.cruisealt);
	XPLMDebugString("\r\ncDepattime");
	XPLMDebugString(new_fp.deptimeact);
	XPLMDebugString("\r\nAirport");
	XPLMDebugString(new_fp.departure);
	XPLMDebugString("\r\nest. dep. time");
	XPLMDebugString(new_fp.deptimeest);
	XPLMDebugString("\r\ndestination");
	XPLMDebugString(new_fp.destination);
	XPLMDebugString("\r\nhours enroute");
	XPLMDebugString(new_fp.hoursenroute);
	XPLMDebugString("\r\nminutes enroute");
	XPLMDebugString(new_fp.minenroute);
	XPLMDebugString("\r\nremarks");
	XPLMDebugString(new_fp.remarks);
	XPLMDebugString("\r\nroute");
	XPLMDebugString(new_fp.route);
	XPLMDebugString("\r\nalternate");
	XPLMDebugString(new_fp.alternate);
	
	//fix for cruisealtitude bug

	if (pos("F",cruisealt)==0)	{del(cruisealt, 0, 1);new_alttype="F";}
	else if (pos("A",cruisealt)==0)	{del(cruisealt, 0, 1);new_alttype="A";}
	else if (pos("VFR",cruisealt)==0)	{del(cruisealt, 0, 3);new_alttype="VFR";}
	
	XPLMDebugString(fpl.deptimeact);
	XPLMDebugString("=waardeflp");
	XPLMDebugString(deptimeact);
	XPLMDebugString("\r\n");
	XPLMDebugString(fpl.cruisealt);
	XPLMDebugString("=cruisealt");
	XPLMDebugString(cruisealt);
	XPLMDebugString("\r\n");
	if(fpl.cruisealt != cruisealt) { HANDLECHG("cruise altitude"); fpl.cruisealt = cruisealt; }
	if(fpl.deptimeact != deptimeact) { HANDLECHG("dep. time"); fpl.deptimeact = deptimeact; }
//	if(fpl.cruisespeed != cruisespeed) { HANDLECHG("cruise speed"); fpl.cruisespeed = cruisespeed; }
	if(fpl.destination != destination) { HANDLECHG("destination"); fpl.destination = destination; }
	if(fpl.departure != departure) { HANDLECHG("dep. airport"); fpl.departure = departure; }
	if(fpl.hoursenroute != hoursenroute) { HANDLECHG("hours enroute"); fpl.hoursenroute = hoursenroute; }
	if(fpl.minenroute != minenroute) { HANDLECHG("minutes enroute"); fpl.minenroute = minenroute; }
	if(fpl.remarks != remarks) { HANDLECHG("remarks"); fpl.remarks = remarks; }
	if(fpl.route != route) { HANDLECHG("route"); fpl.route =route ; }
	if(fpl.alternate != alternate) { HANDLECHG("alternate"); fpl.alternate = alternate; }
	if(fpl.deptimeest != deptimeest) { HANDLECHG("est. dep. time"); fpl.deptimeest = deptimeest; }
	if(fpl.alttype!=new_alttype) {HANDLECHG("Alttype"); fpl.alttype =new_alttype ; }

	if(changed) {
		uiWindow.addMessage(colYellow, "Flightplan changed by : " + changes, true, true);
		msgWindow.addMessage(colYellow, "Flightplan changed by : " + changes);

		// update flightplan in flightplan window
		flightplanForm().fillForm(FALSE);

	//	fsd.sendFlightplan(fpl);
	}
}
*/
void Xivap::handleFPchange(const FSD::Message& m)
{
	Flightplan new_fp = fsd.getFlightplan(m);
	bool changed = false;
	string changes = "";
	//fix for cruisealtitude/ATC bug 22/02/2014 bvk
	if (pos("F",new_fp.cruisealt)==0)	{del(new_fp.cruisealt, 0, 1);new_fp.alttype="F";}
	else if (pos("A",new_fp.cruisealt)==0)	{del(new_fp.cruisealt, 0, 1);new_fp.alttype="A";}
	else if (pos("VFR",new_fp.cruisealt)==0)	{del(new_fp.cruisealt, 0, 3);new_fp.alttype="VFR";}

	if(fpl.cruisespeed != new_fp.cruisespeed) { HANDLECHG("cruise speed"); fpl.cruisespeed = new_fp.cruisespeed; }
	if(fpl.cruisealt != new_fp.cruisealt) { HANDLECHG("cruise altitude"); fpl.cruisealt = new_fp.cruisealt; }
	if(fpl.deptimeact != new_fp.deptimeact) { HANDLECHG("dep. time"); fpl.deptimeact = new_fp.deptimeact; }
	if(fpl.departure != new_fp.departure) { HANDLECHG("dep. airport"); fpl.departure = new_fp.departure; }
	if(fpl.deptimeest != new_fp.deptimeest) { HANDLECHG("est. dep. time"); fpl.deptimeest = new_fp.deptimeest; }
	if(fpl.destination != new_fp.destination) { HANDLECHG("destination"); fpl.destination = new_fp.destination; }
	if(fpl.hoursenroute != new_fp.hoursenroute) { HANDLECHG("hours enroute"); fpl.hoursenroute = new_fp.hoursenroute; }
	if(fpl.minenroute != new_fp.minenroute) { HANDLECHG("minutes enroute"); fpl.minenroute = new_fp.minenroute; }
	if(fpl.alternate != new_fp.alternate) { HANDLECHG("alternate"); fpl.alternate = new_fp.alternate; }
	if(fpl.remarks != new_fp.remarks) { HANDLECHG("remarks"); fpl.remarks = new_fp.remarks; }
	if(fpl.route != new_fp.route) { HANDLECHG("route"); fpl.route =new_fp.route ; }
	if(fpl.alttype!=new_fp.alttype) {HANDLECHG("Alttype"); fpl.alttype =new_fp.alttype ; }

	if(changed) {
		uiWindow.addMessage(colYellow, "Plan de vuelo cambiado por " + m.source + ": " + changes, true, true);
		msgWindow.addMessage(colYellow, "Plan de vuelo cambiado por " + m.source + ": " + changes);

		// update flightplan in flightplan window
		flightplanForm().fillForm(false);

		fsd.sendFlightplan(fpl);
	}
}

void Xivap::setComActive(int radio)
{
/* Eliminado para poder cambiar de canal en el TS al cambiar el COM activo (la función se incluye en tuneCom)

	string name;
	if(radio == 1) name = freq2name(_com1freq);
	else name = freq2name(_com2freq);	
	AtcPosition p = _atcList.findName(name);
	if(p.isValid()) {
		if(_activeRadio != radio)  {
			fsd.sendInfoRequest(name, _FSD_INFOREQ_ATIS_);
			_atcList.setAtis(name, true);
		}
	}
*/
	
	// that value is 10 if com1 is lit, and 11 if com2 is lit
	// but somehow, you can also lite up nav1 and nav2, which doesn't make much sense,
	// because only ONE of com1, com2, nav1 and nav2 can be lit at the same time?
	int value = 10;
	if(radio != 1) value = 11;
	XPLMSetDatai(gAudioSwitches, value);
	_activeRadio = radio;

// Añadido para cambiar el canal TS automáticamente al cambiar el COM activo
	string name;
	if(radio == 1)	
	{
		name = freq2name(_com1freq);
		tuneCom(radio,_com1freq,name);
	}
	else
	{
		name = freq2name(_com2freq);
		tuneCom(radio,_com2freq,name);
	}
}

/*Activa el ID del transponder durante 5.5 segundos*/
void Xivap::setXpdrIdent()
{
	XPLMSetDatai(gXpdrId, 1);
	_identOverride = true;
	Graphics|=16;
	// sq ID for 5.5 seconds
	_setXpdrIdOff = XPLMGetElapsedTime() + 5.5f;
	
}

/*Activa/desactiva el transponder (off=0,stdby=1,on=2,test=3)*/
void Xivap::xpdrModeToggle()
{
	// Transponder mode (off=0,stdby=1,on=2,test=3)
	int xm = XPLMGetDatai(gXpdrMode);

	int newMode = 1;
	xpdrMode = IdentSby;
	if(xm == 0 || xm == 1) {
		newMode = 2;
		xpdrMode = IdentC;
	}

	XPLMSetDatai(gXpdrMode, newMode);
}

void Xivap::CAVOKModeToggle()
{
	cavok = !cavok;
	checkWeather(XPLMGetElapsedTime());
	if (cavok){
		uiWindow.addMessage(colDarkGreen, "CAVOK activado");
		Graphics|=32;
	}
	else{
		uiWindow.addMessage(colDarkGreen, "CAVOK desactivado");
		Graphics&=~32;
	}
	checkWeather(XPLMGetElapsedTime());
}


/* !!! Called once per frame*/
void Xivap::flightLoopCallback()
{
	if(XPLMGetElapsedTime() >= nextFSDPosUpdate && nextFSDPosUpdate != 0.0) {
		sendPosUpdate();
	}

	if(XPLMGetElapsedTime() >= nextFSDPoll && nextFSDPoll != 0.0) {
		FSDPoll();
	}

	if(XPLMGetElapsedTime() >= nextStatusUpdate) {
		updateStatus();
	}

	// stop forced squawk ID
	if(_setXpdrIdOff > 0 && XPLMGetElapsedTime() > _setXpdrIdOff) {
		_setXpdrIdOff = 0;
		_identOverride = false;
		Graphics&=~16;
		XPLMSetDatai(gXpdrId, 0);
	}

	// FIXME: Quitada la carga de servidores de IVAO
	// Download data for make/update serverlist.txt
	/*
	static float nextServerListCallTime = 0.0;
	if (XPLMGetElapsedTime() > nextServerListCallTime && _downloadingServerStatus){
		nextServerListCallTime = XPLMGetElapsedTime() + 5.0f;
		// poll server list download if necessary
		// uiWindow.addMessage(colRed, "Polling HTTP ...", true, true);
		switch(_HTTPclient.status(static_cast<float>(XPLMGetElapsedTime()), 30.0f)) { // give up after 30 seconds
			case HTTP::Status_Connecting:
			case HTTP::Status_Downloading:
				// uiWindow.addMessage(colRed, "Status is connecting or downloading...", true, true);
				break;

			case HTTP::Status_Error:
				uiWindow.addMessage(colRed, "Failed to download server list", true, true);
			case HTTP::Status_Finished:
				_downloadingServerStatus = false;
				// uiWindow.addMessage(colRed, "Status is finished", true, true);
				break;
		}
	}
	*/
	// check if we should switch to a new weather station
	// and check the http downloader
	if(XPLMGetElapsedTime() > _nextWxCheck && _useWeather) {
		_nextWxCheck = XPLMGetElapsedTime() + WEATHER_UPDATE_INTERVAL;
		checkWeather(XPLMGetElapsedTime());
	}

	// weather transition
	if(XPLMGetElapsedTime() > _nextWxTransition && _useWeather && online()) {
		_nextWxTransition = XPLMGetElapsedTime() + WEATHER_TRANSITION_INTERVAL;
		_erwin.transit();
	}

	// check radios
	if(XPLMGetElapsedTime() > _nextRadioAtisCheck && online()) {
		checkRadioAtis();
		_nextRadioAtisCheck = XPLMGetElapsedTime() + RADIO_ATIS_CHECK_INTERVAL;
	}
/*
	// grabar registro en caja negra
	if(XPLMGetElapsedTime() > _siguienteRegistroCaja && caja.Activada()) {
		caja.Graba();
		_siguienteRegistroCaja = XPLMGetElapsedTime() + CAJA_INTERVALO;
	}
*/
	// things that need to be done once, but cannot be done during start or enable
	static bool initialized = false;
	if(!initialized) {
		initialized = true;
		int numAircraft, numActive;
		XPLMPluginID pluginId;
		XPLMCountAircraft(&numAircraft, &numActive, &pluginId);
		if(numAircraft < 4) {
			uiWindow.addMessage(colYellow, "Aviso: TCAS mostrara " + itostring(numAircraft) + " objetivos solamente");
			messageBox().show("Aviso: TCAS mostrara " + itostring(numAircraft) + " objetivos solamente. Si necesita mas, incremente el numero de aviones en las opciones graficas.");
		}
	}

	// let multiplayer do its stuff (update p2p info, send p2p keepalives)
	_multiplayer.frameTick();

	posSent = false;
}

/*Añade ATC */
void Xivap::addATC(const FSD::Message& m)
{
	_atcList.add(m, XPLMGetElapsedTime());
}

/*Elimina ATC*/
void Xivap::delATC(const FSD::Message& m)
{
	_atcList.remove(m);
}

/*Actualiza ATC*/
void Xivap::updateATC(const FSD::Message& m)
{
	_atcList.add(m, XPLMGetElapsedTime());
}

void Xivap::handleWxData(const FSD::Message &m)
{
	_weatherDB.add(m, XPLMGetElapsedTime());
}

std::vector<AtcPosition> Xivap::getAtcPositions(SortOrder sort)
{
	return _atcList.get(sort, lat, lon, XPLMGetElapsedTime());
}

void Xivap::handleWxReply(const FSD::Message& m)
{
	uiWindow.addMessage(colGreen, m.tokens[1], true, true);
	msgWindow.addMessage(colGreen, m.tokens[1]);
}

void Xivap::handleAtisReply(const FSD::Message& m)
{
	int tuneAudio = 0; // don't tune audio
	bool primaryTune = false;	// we're tuning our primary

	if((_activeRadio == 1 || _radioCoupled) && (m.source == com1name)) {
		// ATIS source is first radio station
		tuneAudio = 1;
		primaryTune = _activeRadio == 1;
	}
	else if ((_activeRadio == 2 || _radioCoupled) && (m.source == com2name)) {
		// ATIS source is second radio station
		tuneAudio = 2;
		primaryTune = _activeRadio == 2;
	}

	if(tuneAudio > 0 && isVoiceUrl(m.tokens[0]) && _useVoice) {
		addText(colYellow,
			"Com " + itostring(tuneAudio) + (primaryTune ? " (primaria) " : "") 
				+ " deberia ser sintonizada en " + m.tokens[0] + " ahora", true, true);
#ifdef HAVE_TEAMSPEAK
		if(primaryTune) {
			string channel = m.tokens[0];
			// No hay que procesar el nombre de la dependencia ATC en la red de AHS, el canal de TS es el nombre tal cual
//			int p = pos('/', channel);
//			string server = copy(channel, 0, p);
//			del(channel, 0, p+1);
			tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), channel);
			_tsTuned = true;
		}
#endif
	} else {// just display the ATIS text here
		if(copy(com1name,3,1) == ".") com1name=freq2name(_com1freq);
		if(copy(com2name,3,1) == ".") com2name=freq2name(_com2freq);
		if(_activeRadio==1) {
			if (com1name != m.source && m.source == freq2name(_com1freq)) 
			{
			}
			else {
				uiWindow.addMessage(colGreen, trim(m.tokens[0]));
				msgWindow.addMessage(colGreen, trim(m.tokens[0]));
			}
		}
		else {
			if (com2name != m.source && m.source == freq2name(_com2freq)) 
			{
			}
			else {
				uiWindow.addMessage(colGreen, trim(m.tokens[0]));
				msgWindow.addMessage(colGreen, trim(m.tokens[0]));
			}
					
		}
	
	}
	_atcList.setAtis(m.source, true);
}

bool Xivap::isVoiceUrl(string str)
{
	// a valid URL looks like: <something>.<something>/<something>_<something>

	int slashpos = pos('/', str);
	if(slashpos < 3) return false;	// i want a slash - "x.x/" is the first valid position

	int dotpos = pos('.', str);
	if(dotpos+1 >= slashpos || dotpos < 1) return false;	// i want a dot BEFORE the slash, but not as first char

	int underscorepos = pos('_', str);
	if(underscorepos <= slashpos+1 || underscorepos == length(str)) return false;
									// i want that underscore AFTER the slash, but njot at the end of the line

	return true;
}

void Xivap::processLine(string line)
{
	line = trim(line);
	if(length(line) <= 0) return;

	if(line[0] == '.') {
		handleCommand(line);
		return;
	}

	if(!online()) {
//		uiWindow.addMessage(colRed, "not online or not verified!");

		uiWindow.addMessage(colRed, "NO conectado");

		//FIXME: DEBUG
		if (debug.net)
		{
			if (!fsd.verified()) uiWindow.addMessage(colRed, "_verified = FALSE");
			else uiWindow.addMessage(colRed, "_verified = true");
			if (!fsd.connected()) uiWindow.addMessage(colRed, "_connected = FALSE");
			else uiWindow.addMessage(colRed, "_connected = true");
		}
		return;
	} else {
		// FIXME: ¿Cómo hacer que pase el texto a FlyWithLua antes que al COM1?
		// send input text to com1
		string freq = getActiveRadioFreq();	// 123.450
		freq[0] = '@';						// @23.450
		del(freq, 3, 1);					// @23450
		
		fsd.sendMessage(freq, line);

		uiWindow.addMessage(colGreen, (_activeRadio == 1 ? "COM1> " : "COM2> ") + line);
		msgWindow.addMessage(colGreen, (_activeRadio == 1 ? "COM1> " : "COM2> ") + line);
	}
}

void Xivap::airportChange()
{
    char aptBuf[64];// previously defined
    float lat = XPLMGetDataf(XPLMFindDataRef("sim/flightmodel/position/latitude"));
    float lon = XPLMGetDataf(XPLMFindDataRef("sim/flightmodel/position/longitude"));
    XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &lat, &lon, NULL, xplm_Nav_Airport);
            
    if (ref != XPLM_NAV_NOT_FOUND){
        XPLMGetNavAidInfo(ref, NULL, &lat, &lon, NULL, NULL, NULL, aptBuf, NULL, NULL);        
    }
	addText(colWhite, "Su aeropuerto: " + string(aptBuf), true, true);
	_deptAirport = string(aptBuf);


	//look for GPS airport
	char gpsBuf[64];
	XPLMNavRef gpsRef = XPLMGetGPSDestination();
    if (gpsRef != XPLM_NAV_NOT_FOUND){
        XPLMGetNavAidInfo(gpsRef, NULL, &lat, &lon, NULL, NULL, NULL, gpsBuf, NULL, NULL);        
    }
	addText(colWhite, "Su destino GPS: " + string(gpsBuf), true, true);

	_destAirport = string(gpsBuf);

	//set the route to nothing for GPS
	_flightRoute = "";

	/* 
		FMS ( dataref sim/aircraft/overflow/acf_has_FMS )
		if the aircraft has FMS then use that to get flightplan data
		otherwise use plane location and GPS destination
	*/

	int hasFMS = XPLMGetDatai(XPLMFindDataRef("sim/aircraft/overflow/acf_has_FMSx"));

	long fmsEntryCount = XPLMCountFMSEntries();

	for (int index = 0; index < fmsEntryCount; index++) {
		XPLMNavType outType;
		XPLMNavRef outRef;
		char outID[256];
		XPLMGetFMSEntryInfo(index, &outType, outID, &outRef, NULL, NULL, NULL);
		addText(colWhite, "Su destino FMS: " + string(outID), true, true);

		if ( index == 0 && outType == xplm_Nav_Airport) {
			_deptAirport = string(outID);
		}

		if ( index > 0 && outType != xplm_Nav_Unknown && outType != xplm_Nav_Airport) {
			_flightRoute += string(outID) + " ";
		}

		if ( index > 0 && outType == xplm_Nav_Airport) {
			_destAirport = string(outID);
		}

		if (outType == xplm_Nav_Unknown) {
			break;
		}
	}
	
	xivap.flightplanForm().setFPTextFields();
}

/*Toma el modelo de avion actualmente en uso*/
void Xivap::aircraftChange() 
{
	// how many engines?
	XPLMDataRef df	= XPLMFindDataRef("sim/aircraft/engine/acf_num_engines");
	numEngines = XPLMGetDatai(df);

	df	= XPLMFindDataRef("sim/aircraft/view/acf_ICAO");
	char byteBuf[40];
	XPLMGetDatab(df, byteBuf, 0, 40);
	string icao = string(byteBuf);
	icao = strupcase(trim(icao));
	//FIXME: DEBUG
	if (debug.multiplayer)
		addText(colWhite, "Su ICAO (fichero ACF): '" + icao + "'", true, true);
//	int len;
//	string icao_ = "test";//STDSTRING(icao);
//   len = length(icao_);
//	if(len > 1){ // changes the acType only if we found a working acType value
    _acType = icao; //otherwise, keep the value from the FPL
	xivap.flightplanForm().setAcfIcao();
/*
	if (online())
	{
		std::vector<string> lista = _multiplayer.listaConectados(); // Obtiene la lista de indicativos de pilotos conectados a la red
		for (int i = 0; i < lista.size(); ++i) // Envía tipo de avión (ICAO, airline, livery) a cada piloto conectado
		{
			fsd.sendPlaneInfo(_acType + _acAirline + _acLivery, lista[i]);
		}	
	}
	else // Si no está conectado, llama a la función de envío para que inicialice la variable _mtl y se pueda conectar más adelante
	{
		fsd.sendPlaneInfo(_acType + _acAirline + _acLivery, "dummy");
	}
*/
//	}
	
	addText(colWhite, "Su ICAO: " + _acType, true, true);
//	uiWindow.addMessage(colWhite, "Su ICAO: " + _acType, true, true);


	/*
	// update description if needed...

	byteBuf[1000];
	XPLMGetDatab(gAcfDescription, byteBuf, 0, 500);
	string desc = string(byteBuf);
	addText(colWhite, desc, true, true);
	*/

	XPLMGetDatab(gAcfTailnum, byteBuf, 0, 40);
	string tailnum = string(byteBuf);
	addText(colWhite, "Su matricula: " + tailnum, true, true);

	if (caja.Activada()) caja.Fin(); // Desactiva caja negra si estaba activada

}

void Xivap::handleCommand(string line)
{
	del(line, 0, 1);
	if(length(line) < 3) return;
	line = trim(line);
	int p = pos(' ', line);
	string command = strupcase(line);
	if(p >= 0) {
		command = strupcase(copy(line, 0, p));
		del(line, 0, p+1);
		line = trim(line);
		if(length(line) <= 0) return;
	}

	// the ".dot commands"
	if(command == "R") {
		if(!online()) return;
		if(_lastPrivSender != "") {
			fsd.sendMessage(_lastPrivSender, line);
			uiWindow.addMessage(colGreen, _lastPrivSender + "> " + line);
			msgWindow.addMessage(colGreen, _lastPrivSender + "> " + line);
		}
		return;

	} else if(command == "MSG") {
		if(!online()) return;
		p = pos(' ', line);		// .msg callsign text
		if(p == -1) return;		// callsign ends at the first space
		string dest = strupcase(copy(line, 0, p));
		del(line, 0, p+1);
		line = trim(line);	// remove leading/trailing blanks from text
		if(length(line) <= 0) return;

		// dont allow to send to funny destinations...
		dest = stripchar(dest, '.');	
		dest = stripchar(dest, '@');
		dest = stripchar(dest, '*');
		if(length(dest) <= 0) return;

		fsd.sendMessage(dest, line);
		uiWindow.addMessage(colGreen, dest + "> " + line);
		msgWindow.addMessage(colGreen, dest + "> " + line);
		return;

	} else if(command == "METAR") {
		if(!online()) return;
		line = trim(strupcase(line));
//		fsd.sendWxRequest(FSD::WX_METAR, line);
//		Sleep(2000); // Espera 2 seg. para que se reciba el METAR y se guarde en la dB
//		line = strupcase(line);
		if (length(line) == 4)
			{
			string rMETAR = fsd.getMETAR(line); // Conseguir el METAR de la estación
			if (rMETAR != "ERROR") // Si se ha recibido un METAR válido
			{
//				uiWindow.addMessage(colWhite, "METAR de " + line + ": " + rMETAR);
//				msgWindow.addMessage(colWhite, "METAR de " + line + ": " + rMETAR);
				uiWindow.addMessage(colWhite, rMETAR);
//				msgWindow.addMessage(colWhite, rMETAR);
			}
			else
			{
				WxStation station = _weatherDB.findName(line);
				if(station.name != "")
				{
					uiWindow.addMessage(colWhite, station.metar);
					msgWindow.addMessage(colWhite, station.metar);
				}
				else
				{
					uiWindow.addMessage(colWhite, "No hay datos de METAR para " + line);
					msgWindow.addMessage(colWhite, "No hay datos de METAR para " + line);
				}
			}
		}

		return;

	} else if(command == "TAF") {
		if(!online()) return;
		fsd.sendWxRequest(FSD::WX_TAF, line);
		return;

	} else if(command == "SHORTTAF") {
		if(!online()) return;
		fsd.sendWxRequest(FSD::WX_SHORTTAF, line);
		return;

	} else if(command == "ATIS") {
		if(!online()) return;
		fsd.sendInfoRequest(line, _FSD_INFOREQ_ATIS_);
		return;

	} else if(command == "WALLOP") {
		if(!online()) return;
		fsd.sendMessage("*S", line);
		uiWindow.addMessage(colWhite, "WALLOP> " + line);
		return;

	} else if(command == "BROADCAST") {
		if(!online()) return;
		fsd.sendMessage("*", line);
		uiWindow.addMessage(colWhite, "BROADCAST> " + line);
		return;

	} else if(command == "NOMP") {
		setMultiplayer(false);
		uiWindow.addMessage(colDarkGreen, "Multijugador desactivado");
		return;

	} else if(command == "YESMP") {
		uiWindow.addMessage(colDarkGreen, "Activando multijugador");
		setMultiplayer(true);
		return;

	} else if(command == "NOWX") {
		setWeather(false);
		uiWindow.addMessage(colDarkGreen, "Meteorología desactivada");
		return;

	} else if(command == "CAVOK") {
		line = strupcase(trim(line));
		if (length(line) <= 0) // si se envía el comando sin parámetros
		{
			if (cavok) // conmuta el CAVOK
			{
				cavok = false;
				Graphics&=~32;
				uiWindow.addMessage(colDarkGreen, "Conmutado CAVOK a desactivado");
			}
			else
			{
				cavok = true;
				Graphics|=32;
				uiWindow.addMessage(colDarkGreen, "Conmutado CAVOK a activado");
			}
		}
		else
		{
			line = strupcase(line);
			if (line == "OFF") // "CAVOK OFF" desactiva el CAVOK
			{
				cavok = false;
				Graphics&=~32;
				uiWindow.addMessage(colDarkGreen, "CAVOK desactivado");
			}
			else // cualquier otro parámetro, se interpretará como activar CAVOK
			{
				cavok = true;
				Graphics|=32;
				uiWindow.addMessage(colDarkGreen, "CAVOK activado");
			}
		}
		checkWeather(XPLMGetElapsedTime());
		return;

	} else if(command == "YESWX") {
		setWeather(true);
		uiWindow.addMessage(colDarkGreen, "Meteorología activada");
		return;

#ifdef HAVE_TEAMSPEAK
	} else if(command == "VOICE") {
		setVoice(true);
		uiWindow.addMessage(colDarkGreen, "Voz activada");
		return;

	} else if(command == "NOVOICE") {
		setVoice(false);
		uiWindow.addMessage(colDarkGreen, "Voz desactivada");
		return;
#endif

	} else if(command == "CHAT") {
		ChatWindow* window;
		line = strupcase(trim(line));
		if(chatWindows.size() > 0) {
			int y = chatWindows[chatWindows.size()-1]->getTop() - 15;
			int x = chatWindows[chatWindows.size()-1]->getLeft() + 15;
			window = new ChatWindow(line, x, y);
		} else {
			window = new ChatWindow(line);
		}
		chatWindows.push_back(window);
		return;

	} else if(command == "INF") {
		line = strupcase(trim(line));
		int p = pos(' ', line);
		if(p > 0) line = trim(copy(line, 0, p));
		fsd.sendInfoRequest(line, "INF");
		return;

	} else if(command == "KILL") {
		int p = pos(' ', line);
		if(p < 0) {
			uiWindow.addMessage(colYellow, "Tiene que porporcionar un callsign y una razon", false, false);
			return;
		}
		callsign = strupcase(trim(copy(line, 0, p)));
		del(line, 0, p);
		fsd.sendKill(callsign, trim(line));
		return;

	} else if(command == "X") {
		if(length(line) > 4) line = copy(line, 0, 4);
		for(int i = 0; i < length(line); ++i)
			if((line[i] < '0') || (line[i] > '7')) return;
		int xpdrCode = atoi(pconst(line));
		XPLMSetDatai(gXpdrCode, xpdrCode);
		return;

	} else if(command == "C1" || command == "C2") {
		int com = 1;
		if(command[1] == '2') com = 2;
		// convert string 124.4 to 12440
		line = stripchar(line, '.');
		while(length(line) < 5) line += "0";
		int freq = atoi(pconst(line));
		if(!((freq >= 11800 && freq <= 13697) ||
		   (freq >= 118000 && freq <= 136970))) return; // 6 digit check

		if(com == 1) {
			// com1
			if(freq != _com1freq) tuneCom(1, freq);
		} else {
			// com2
			if(freq != _com2freq) tuneCom(2, freq);
		}
		return;
		
	} else if(command == "INSPECTOR") {
		refInspector().show();
		return;

	} else if(command == "QNH") {
		if(length(line) < 3) return;
		if(length(line) > 4) return;
		float qnh = static_cast<float>(atof(pconst(line)));
		if(qnh < 900 || qnh > 1120) return;
		qnh = 29.92f * qnh / 1013.25f;
		XPLMSetDataf(gBarometer, qnh);
		return;

	} else if(command == "ALT") {
		if(length(line) != 5) return;
		float alti = static_cast<float>(atof(pconst(line)));
		if(alti < 20 || alti > 45) return;
		XPLMSetDataf(gBarometer, alti);
		return;

	} else if(command == "DUMPWX") {
		if(length(line) != 4) return;
		fsd.sendWxRequest(FSD::WX_METAR, line);

		addText(colWhite, "WX DUMP FOLLOWS:", true, true);
		currentWxStation.dump();
		WxStation station = _weatherDB.findName(line);
		if(station.name != "")
			station.dump();
		_erwin.dump();
		return;

	} else if(command == "FIND") {
		std::vector<string> lines = _navQuery.query(line);
		for(std::vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
			uiWindow.addMessage(colWhite, *it);
		return;

	} else if(command == "MODEL") {
		line = strupcase(line);
		int p = pos('/', line);
		if(p < 0) {
			uiWindow.addMessage(colYellow, "Usage: .model AIRCRAFT/AIRLINE/LIVERY", false, false);
			return;
		}
		string aircraft = copy(line, 0, p);
		del(line, 0, p+1);
		p = pos('/', line);
		if(p < 0) {
			uiWindow.addMessage(colYellow, "Usage: .model AIRCRAFT/AIRLINE/LIVERY", false, false);
			return;
		}
		string airline = copy(line, 0, p);
		del(line, 0, p+1);
		string livery = line;

		uiWindow.addMessage(colYellow, "Estableciendo todos los modelos en " + aircraft + "/" + airline + "/" + livery, true, true);
		_multiplayer.setAllModels(aircraft, airline, livery);
		return;

	}	else if (command=="TEST") { //special inserted debugconsole command 22/02/2014 bvk
		/*insert here the functions you whish to check*/
		xivap.test(line); // Rehabilitado comando "TEST" para mandar un mensaje al plugin como si viniera de la red
		return;

	}	else if (command=="SEND") { // Añadido para mandar un mensaje manualmente a la red
		if(!online()) return;
		line = strupcase(trim(line));
		FSD::Message m;
		if (m.decompose(line)) fsd.send(m);
		return;

	}	else if (command=="TS") { // Añadido comando para cambiar de canal directamente en el Teamspeak
		line = trim(strupcase(line));
		tsRemote.SwitchChannel("AHS"+fsd.vid(), fsd.password(), string(AHS_SERVER_URL), fsd.callsign(), line);
		return;

	}	else if (command=="ICAO") { // Añadido comando para cambiar el ICAO de la aeronave propia para mostrar en red
		line = trim(strupcase(line));
		if (length(line) > 2 && length(line) < 5)
		{
			setAcType(line,"","");
			XPSetWidgetDescriptor(_connectForm->acMTLTextField, pconst(_acType)); //  copia ICAO de la aeronave al "ICAO en red" de la ventana de conexión

			uiWindow.addMessage(colWhite, "Nuevo ICAO de la aeronave para mostrar en la red: " + _acType, true, true);
		}
		return;

	}	else if (command=="DEBUG") { // Añadido comando para aplicar niveles de debug al vuelo (parámetros separados por ":")
		line = trim(strupcase(line));
		if (line == "") // Si no hay parámetros
		{
			debug.debuglevels = MAXUINT32; // Pone todos los niveles de debug activos
		}
		else
		{
			line = "@*:*:" + line; // Engaña al "message decomposer" para que descomponga la línea en "tokens"
			FSD::Message mdebug;
			if (mdebug.decompose(line)) // Buscar parámetros separados por ":"
			{
				if (mdebug.tokens.size() == 1) // Si hay un parámetro
				{
					if (mdebug.tokens[0] == "NET") debug.net = 1; // mensajes de red
					else if (mdebug.tokens[0] == "WX") debug.weather = 3; // sistema de meteorología debug completo
					else if (mdebug.tokens[0] == "MP") debug.multiplayer = 1; // sistema multiplayer
					else if (mdebug.tokens[0] == "TS") debug.teamspeak = 1; // teamspeak
					else if (mdebug.tokens[0] == "PARAMS") debug.params = 1; // parámetros del avión
					else if (mdebug.tokens[0] == "UI") debug.ui = 1; // interfaz de usuario
					else if (mdebug.tokens[0] == "BB") debug.bb = 1; // interfaz de usuario
					else if (mdebug.tokens[0] == "OFF") debug.debuglevels = 0; // debug desactivado
				} // if 1 parámetro
				else if (mdebug.tokens.size() == 2) // Si hay 2 parámetros
				{
					if (mdebug.tokens[1] == "OFF")
					{
						if (mdebug.tokens[0] == "NET") debug.net = 0;
						else if (mdebug.tokens[0] == "WX") debug.weather = 0;
						else if (mdebug.tokens[0] == "MP") debug.multiplayer = 0;
						else if (mdebug.tokens[0] == "TS") debug.teamspeak = 0;
						else if (mdebug.tokens[0] == "PARAMS") debug.params = 0;
						else if (mdebug.tokens[0] == "UI") debug.ui = 0;
					}
					else if (mdebug.tokens[1] == "ON")
					{
						if (mdebug.tokens[0] == "NET") debug.net = 1; // mensajes de red
						else if (mdebug.tokens[0] == "WX") debug.weather = 3; // sistema de meteorología debug completo
						else if (mdebug.tokens[0] == "MP") debug.multiplayer = 1; // sistema multiplayer
						else if (mdebug.tokens[0] == "TS") debug.teamspeak = 1; // teamspeak
						else if (mdebug.tokens[0] == "PARAMS") debug.params = 1; // parámetros del avión
						else if (mdebug.tokens[0] == "UI") debug.ui = 1; // interfaz de usuario
					}
					else
					{
						if (mdebug.tokens[0] == "NET") debug.net = 1; // mensajes de red
						else if (mdebug.tokens[0] == "WX") debug.weather = atoi(mdebug.tokens[1]); // nivel de debug para el sistema de meteorología (de 1 a 3)
						else if (mdebug.tokens[0] == "MP") debug.multiplayer = 1; // sistema multiplayer
						else if (mdebug.tokens[0] == "TS") debug.teamspeak = 1; // teamspeak
						else if (mdebug.tokens[0] == "PARAMS") debug.params = 1; // parámetros del avión
						else if (mdebug.tokens[0] == "UI") debug.ui = 1; // interfaz de usuario
					}
				} // if 2 parámetros
			}
		}
			return;

	}	else if(command == "HELP") {
		uiWindow.addMessage(colYellow, "Lista de comandos:", false, false);
		uiWindow.addMessage(colYellow, ".R <text> - responder a mensaje privado", false, false);
		uiWindow.addMessage(colYellow, ".MSG <callsign> <text> - enviar un mensaje privado a un callsign", false, false);
		uiWindow.addMessage(colYellow, ".CHAT <callsign> - abrir una ventana de chat a un callsign (teclear .HELP allí)", false, false);
		uiWindow.addMessage(colYellow, ".WALLOP <message> - enviar un mensaje a todos los SUPs online", false, false);
		uiWindow.addMessage(colYellow, ".METAR <station> - solicitar METAR de una estación", false, false);
		uiWindow.addMessage(colYellow, ".TAF <station - solicitar TAF de una estación", false, false);
		uiWindow.addMessage(colYellow, ".SHORTTAF <station> - solicitar SHORTTAF de una estación", false, false);
		uiWindow.addMessage(colYellow, ".ATIS <callsign> - solicitar mensaje ATIS", false, false);
		uiWindow.addMessage(colYellow, ".NOMP / .YESMP - modo Multijugador off/on", false, false);
		uiWindow.addMessage(colYellow, ".NOWX / .YESWX - modo Meteorología off/on", false, false);
		uiWindow.addMessage(colYellow, ".VOICE / .NOVOICE - modo Voz off/on", false, false);
		uiWindow.addMessage(colYellow, ".MODEL <aircraft>/<airline>/<livery> - ver a todos los demás jugadores como el avion especificado (para desarrolladores CSL)", false, false);
		uiWindow.addMessage(colYellow, ".DUMPWX <ICAO> - volcar el perfil de clima actual y el perfil para ICAO en un fichero log y consola (para depuración WX )", false, false);
		uiWindow.addMessage(colYellow, ".FIND <ID> - imprimir informacion sobre la NavId ID", false, false);
		uiWindow.addMessage(colYellow, ".X <code> - sintonizar el transponder en el código especificado", false, false);
		uiWindow.addMessage(colYellow, ".C1 <freq> / .C2 <freq> - sintonizar COM1 o COM2 en freq", false, false);
		uiWindow.addMessage(colYellow, ".QNH <QNH> .ALT <ALTIMETER> - Establecer el altímetro primario en el valor barómetrico dado", false, false);
		uiWindow.addMessage(colYellow, ".TEST MSG:<mensaje> - probar mensaje como si viniera de la red (solo para desarrolladores)", false, false);
		uiWindow.addMessage(colYellow, ".SEND <mensaje> - enviar mensaje a la red (solo para desarrolladores)", false, false);
		uiWindow.addMessage(colYellow, ".CAVOK / .CAVOK OFF - Activar o desactivar modo CAVOK forzado", false, false);
		uiWindow.addMessage(colYellow, ".TS <canal> - Conectarse al canal del Teamspeak", false, false);
		uiWindow.addMessage(colYellow, "Usar flecha-arriba y flecha-abajo para desplazar pantalla", false, false);
		return;

	}

}

void Xivap::setAcType(const string& aircraft, const string& airline, const string& livery)
{
	_acType = aircraft;
	_acAirline = airline;
	_acLivery = livery;
	while(length(_acType) < 4) _acType = _acType + " ";

	if (_connectForm != NULL)
	{
		XPSetWidgetDescriptor(_connectForm->acMTLTextField, pconst(_acType)); //  copia ICAO de la aeronave al "ICAO en red" de la ventana de conexión
		_connectForm->hide();
	}

	if (online())
	{
		std::vector<string> lista = _multiplayer.listaConectados(); // Obtiene la lista de indicativos de pilotos conectados a la red
		for (int i = 0; i < lista.size(); ++i) // Envía tipo de avión (ICAO, airline, livery) a cada piloto conectado
		{
			fsd.sendPlaneInfo(_acType + _acAirline + _acLivery, lista[i]);
		}	
	}
	else // Si no está conectado, llama a la función de envío para que inicialice la variable _mtl y se pueda conectar más adelante
	{
		fsd.sendPlaneInfo(_acType + _acAirline + _acLivery, "dummy");
	}

	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PLANE", "ICAO",_acType);
	config.save(filename);

}

// if we are using weather, this is being called once every WEATHER_UPDATE_INTERVAL secs
void Xivap::checkWeather(double elapsed)
{
//	std::vector<WxStation> stations = _weatherDB.get(lat, lon, static_cast<float>(elapsed));

	std::vector<WxStation> stations = _weatherDB.estacionesCercanas(lat, lon, static_cast<float>(elapsed));

	bool use_glob = false;
	int iest = 0;
	if(stations.size() == 0) 
	{
		use_glob = true;

//FIXME: DEBUG
		if (debug.weather > 0)
			xivap.addText(colRed, "No se han encontrado estaciones cercanas (a menos de 60 Nm)", true, true);
	}

	else 
	{
		use_glob = true;
		while (iest < stations.size() && iest < 10) // Pedir datos del tiempoa las 10 primeras estaciones a menos de 60 Nm
		{
			// Petición de METAR para la estación para tener los datos del tiempo de la red de AHS
			if (online())
			{
				fsd.sendWxRequest(FSD::WX_METAR, stations[iest].name);
//				Sleep(2000); // Hacer tiempo para recibir respuesta del METAR
//				FSDPoll();
			}
			//FIXME: De alguna manera tiene que dar tiempo a que se reciba el METAR de la estación más cercana, ya que a veces se recibe antes otra estación más alejada)
			if (_weatherDB.findName(stations[iest].name,static_cast<float>(elapsed)).isValid(static_cast<float>(elapsed))) // Si la estación está en el mapa de estaciones con datos de meteorología (es válida)
			{
				stations[iest] = _weatherDB.findName(stations[iest].name,static_cast<float>(elapsed)); // Copia la estación con datos a la vacía 
				use_glob = false;
				break;
			}
			

			++iest;
		}

//FIXME: DEBUG
		if (debug.weather > 0)
			if (use_glob) xivap.addText(colRed, "No se han encontrado estaciones cercanas con datos de meteorologia (distancia a la estacion mas cercana = " + itostring((int)stations[0].distance) + ")", true, true);
		
	}

	if(use_glob) {
		// get the GLOB weather
		WxStation newStation = _weatherDB.findName("GLOB", static_cast<float>(elapsed));
		if(!newStation.isValid()) return;

		// set the GLOB weather
		_erwin.setWeather(newStation, static_cast<float>(el), static_cast<float>(groundalt));
		currentWxStation = newStation;

//FIXME: DEBUG
		if (debug.weather > 0)
			xivap.addText(colRed, "Usando GLOB como WxStation", true, true);
		return;
	}

//FIXME: DEBUG
	if (debug.weather > 2)
		xivap.addText(colRed, "currentWxStation.name = " + currentWxStation.name + " antes del if 'GLOB' o invalid", true, true);

	if(currentWxStation.name == "GLOB" || !currentWxStation.isValid())
		currentWxStation.distance = 9999; 
	else
		currentWxStation.distance = deg2dist(lat, lon, currentWxStation.lat, currentWxStation.lon);

	if(currentWxStation.distance > stations[iest].distance + WEATHER_SWITCH_HYSTERESE
		|| !(currentWxStation == stations[iest])) {

		currentWxStation = stations[iest];

//FIXME: DEBUG
	if (debug.weather > 2)
		xivap.addText(colRed, "currentWxStation = stations[0] (" + stations[iest].name + ") después del if 'GLOB' o invalid", true, true);

// Añadida petición de METAR para la estación para tener los datos del tiempo de la red de AHS
//		if (online())
//		{
//			fsd.sendWxRequest(FSD::WX_METAR, currentWxStation.name);
//			Sleep(2000); // Hacer tiempo para recibir respuesta del METAR
//			FSDPoll();
//		}
		if (_fplSend)
		{
			metarOrg = metarDest = metarAlt = metarAlt2 = "";
			WxStation ADOrg, ADDest, ADAlt, ADAlt2;
			
			// Busca ADs del FP en la dB de meteorología
			ADOrg = _weatherDB.findName(fpl.departure,static_cast<float>(elapsed));
			ADDest = _weatherDB.findName(fpl.destination,static_cast<float>(elapsed));
			ADAlt = _weatherDB.findName(fpl.alternate,static_cast<float>(elapsed));
			
			// Recupera METARs
			if (ADOrg.metar != "") metarOrg = ADOrg.metar;
			else metarOrg = fpl.departure + ": No hay METAR disponible";
			if (ADDest.metar != "") metarDest = ADDest.metar;
			else metarDest = fpl.destination +": No hay METAR disponible";
			if (ADAlt.metar != "") metarAlt = ADAlt.metar;
			else metarAlt = fpl.alternate + ": No hay METAR disponible";

			// Muetra METARs en ventana de mensajes
			msgWindow.addMessage(colGreen,"Origen: " + metarOrg + "\n\n"); // Muestra el METAR del AD origen en los mensajes
			if (!cavok) msgWindow.addMessage(colWhite,"METAR Actual: " + currentWxStation.metar + "\n\n"); // Muestra el METAR del AD más próximo en los mensajes
			else msgWindow.addMessage(colWhite,"METAR Actual: " + currentWxStation.metar + " (Tiempo CAVOK forzado)\n\n"); 
			msgWindow.addMessage(colCyan,"Destino: " + metarDest + "\n\n"); // Muestra el METAR del AD de destino en los mensajes
			msgWindow.addMessage(colRed,"Alternativo: " + metarAlt + "\n\n"); // Muestra el METAR del AD alternativo en los mensajes
			if (xivap.fpl.alternate2 != "") // Si existe 2º AD alternativo, mostrar su METAR también
			{
				ADAlt2 = _weatherDB.findName(fpl.alternate2,static_cast<float>(elapsed));
				if (ADAlt2.metar != "") metarAlt2 = ADAlt2.metar;
				else metarAlt2 = fpl.alternate2 + ": No hay METAR disponible";
				msgWindow.addMessage(colWhite,"Alternativo 2: " + metarAlt2); // Muestra el METAR del 2º AD alternativo en los mensajes
			}
		}
		else
		{
			if (!cavok) msgWindow.addMessage(colWhite,"METAR Actual: " + currentWxStation.metar + "\n\n"); // Muestra el METAR del AD más próximo en los mensajes
			else msgWindow.addMessage(colWhite,"METAR Actual: " + currentWxStation.metar + " (Tiempo CAVOK forzado)\n\n"); 
		}
		_erwin.setWeather(currentWxStation, static_cast<float>(el), static_cast<float>(groundalt));
	}
}

/*Activar voz*/
void Xivap::setVoice(bool value)
{
	if(_useVoice == value) return;

#ifdef HAVE_TEAMSPEAK
	_useVoice = value;

	if(_useVoice) { // voice is being turned on

		if(!_tsLoaded) { // start TS
			tsRemote.Start();
			_tsLoaded = true;
		}

	} else { // voice is being turned off

		if(_tsTuned && _tsLoaded) {
			tsRemote.Disconnect();
			_tsTuned = false;
			_tsLoaded = false;
		}
	}

#endif

	fsd.sendVoice(_useVoice);

	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("SOUND", "VOICE", (_useVoice == 1 ? "1" : "0"));
	config.save(filename);
}
void Xivap::setPMSG(bool value)
{
	if (_usePMSG==value) return;
	_usePMSG=value;
	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PREFERENCES", "PRVTMSGSOUND", (_usePMSG == 1 ? "1" : "0"));
	config.save(filename);
}
void Xivap::setLabels(bool value)
{
	if(_useLabels == value) return;

	_useLabels=value;
	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PREFERENCES", "TAGGING", (_useLabels == 1 ? "1" : "0"));
	config.save(filename);
}

/*Activar climatologia*/
void Xivap::setWeather(bool value)
{
	if(_useWeather == value) return;
	_erwin.init();
	_weatherDB.clear();
	_useWeather = value;
	if(!_useWeather) fsd.unsubscribeWeather();
	else fsd.subscribeWeather();

	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PREFERENCES", "WEATHER", (_useWeather == 1 ? "1" : "0"));
	config.save(filename);
}

/*Activar modo multijugador*/
void Xivap::setMultiplayer(bool value)
{
	if(_useMultiplayer == value) return;

	_useMultiplayer = value;

	if(!_useMultiplayer) { // turn off MP
		_multiplayer.disable();
	} else { // turn on MP
		if(!_multiplayer.initialized()) {
			if(!_multiplayer.init()) {
				uiWindow.addMessage(colRed, "Falló al inicializar el multijugador: "
					+ _multiplayer.errorMessage(), true, true);
				_useMultiplayer = false;
				return;
			}
		}
		_multiplayer.enable();
	}

	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
	config.setConfig("PREFERENCES", "MULTIPLAYER", (_useMultiplayer == 1 ? "1" : "0"));
	config.save(filename);
}

/*Cerrar ventana de chat*/
void Xivap::closeChatWindow(ChatWindow *win)
{
	std::vector<ChatWindow*>::iterator it = chatWindows.begin();
	while(it != chatWindows.end()) {
		if(*it == win) {
			win->hide();
			return;
		} else {
			++it;
		}
	}
}

/*Solicitar conexion P2P*/
void Xivap::RequestP2P(const string& callsign, const string& protocol, int mode, const string& ipport)
{
	if(mode < 2) return;

	// $CQAAAA:BBBB:P2P:<mode>:<application>[:<ip>:<port>:<intip>:<intport>]
	string request = "P2P:" + itostring(mode) + ":" + protocol;
	if(mode > 1)
		request += ":" + ipport;

	fsd.sendInfoRequest(callsign, request);
	xivap.addText(colRed, "Enviada petición P2P: " + request, true, true);
}

/*Enviar respuesta P2P*/
void Xivap::SendP2PReply(const string& callsign, const string& protocol, int mode, const string& ipport)
{
	// $CRAAAA:BBBB:P2P:<mode>:<application>[:<ip>:<port>:<intip>:<intport>]
	string reply = itostring(mode) + ":" + protocol;
	if(mode > 1)
		reply += ":" + ipport;

	fsd.sendInfoRequestReply(callsign, "P2P", reply);
	xivap.addText(colRed, "Enviada respuesta P2P: " + reply, true, true);
}

/*SELCAL, o llamada selectiva, es un sistema de llamada usado conjuntamente con las comunicaciones por radio de
alta frecuencia (HF). Se utiliza para alertar a pilotos que comunican con ATC con un mensaje de radio entrante en el
HF.*/
void Xivap::Selcal(string source)
{
	ConfigFile config;
	string filename = getXivapRessourcesDir() + CONFIG_FILE;
	config.load(filename);
#ifdef WIN32
	uiWindow.addMessage(colYellow, source + ">S-E-L-C-A-L", true, true);
	msgWindow.addMessage(colYellow, source + ">S-E-L-C-A-L");
	if(config.readConfig("SOUND", "PCSELCAL") == "1") 
	{
	Beep(SELCAL_BEEP_1,500);
    Beep(SELCAL_BEEP_2,500);
	}
#endif
#ifdef LINUX
#define KDMKTONE 0x4B30
	uiWindow.addMessage(colYellow, source + ">S-E-L-C-A-L", true, true);
	msgWindow.addMessage(colYellow, source + ">S-E-L-C-A-L");
	if(config.readConfig("SOUND", "PCSELCAL") == "1") 
	{
	int fd = open("/dev/tty10", O_RDONLY);
	if (fd == -1) 
	{
	uiWindow.addMessage(colRed, "Couldn't access to /dev/tty10", true, true);
	msgWindow.addMessage(colRed, "Couldn't access to /dev/tty10");
	}
	else
	{
	ioctl(fd, KDMKTONE, (500<<16)+(1193180/SELCAL_BEEP_1));
	ioctl(fd, KDMKTONE, (500<<16)+(1193180/SELCAL_BEEP_2));
	}
	}
#endif
#ifdef MAC
	uiWindow.addMessage(colYellow, source + ">S-E-L-C-A-L", true, true);
	msgWindow.addMessage(colYellow, source + ">S-E-L-C-A-L");
#endif
}


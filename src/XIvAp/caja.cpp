/******************************************************************************
 * Grabación de caja negra para X-IvAp AHS									  *
 * Copyright (C) 2014 by Mikel N'Dong (AHS120D)								  *
 *																			  *
 ******************************************************************************/

#include "caja.h"
#include "xivap.h"
//#include "XPLMDataAccess.h"

extern char Graphics;

CajaNegra::CajaNegra()
{
	gXp_ALT = XPLMFindDataRef(XP_ALT);
	gXp_AGL = XPLMFindDataRef(XP_AGL);
	gXp_ZuluH = XPLMFindDataRef(XP_ZULU_HOUR);
	gXp_ZuluM = XPLMFindDataRef(XP_ZULU_MIN);
	gXp_ZuluS = XPLMFindDataRef(XP_ZULU_SEC);
	gXp_SimSecs = XPLMFindDataRef(XP_SIM_ZULU_SECS);
	gXp_Kias = XPLMFindDataRef(XP_KIAS);
	gXp_MSec = XPLMFindDataRef(XP_MSEC);
	gXp_SoundS = XPLMFindDataRef(XP_SOUNDS);
	gXp_Vs = XPLMFindDataRef(XP_VSPEED);
	gXp_Stall = XPLMFindDataRef(XP_STALL);
	gXp_OverS = XPLMFindDataRef(XP_OVERSPEED);
	gXp_Vne = XPLMFindDataRef(XP_VNE);
	gXp_Lat = XPLMFindDataRef(XP_LAT);
	gXp_Lon = XPLMFindDataRef(XP_LON);
	gXp_Amsl = XPLMFindDataRef(XP_AMSL);
	gXp_Qnh = XPLMFindDataRef(XP_QNH);
	gXp_Pitch = XPLMFindDataRef(XP_PITCH);
	gXp_Roll = XPLMFindDataRef(XP_ROLL);
	gXp_Hdg = XPLMFindDataRef(XP_HDG);
	gXp_Ap = XPLMFindDataRef(XP_AP);
	gXp_Flaps = XPLMFindDataRef(XP_FLAPS);
	gXp_Gear = XPLMFindDataRef(XP_GEAR);
	gXp_SimSpeed = XPLMFindDataRef(XP_SIMSPEED);
	gXp_GForce = XPLMFindDataRef(XP_GFORCE);
	gXp_Fuel = XPLMFindDataRef(XP_FUEL);
	gXp_Gps = XPLMFindDataRef(XP_GPS);
	gXp_Payload = XPLMFindDataRef(XP_PAYLOAD);
//	gXp_LbsG = XPLMFindDataRef(XP_LBG);
	gXp_Weight = XPLMFindDataRef(XP_WEIGHT);
	gXp_Pitot = XPLMFindDataRef(XP_PITOT);
	gXp_Oat = XPLMFindDataRef(XP_OAT);
	gXp_Vis = XPLMFindDataRef(XP_VIS);
	gXp_Wknt = XPLMFindDataRef(XP_WKNT);
	gXp_Wknt0 = XPLMFindDataRef(XP_WKNT0);
	gXp_Wknt1 = XPLMFindDataRef(XP_WKNT1);
	gXp_Wknt2 = XPLMFindDataRef(XP_WKNT2);
	gXp_Wdir = XPLMFindDataRef(XP_WDIR);
	gXp_Wdir0 = XPLMFindDataRef(XP_WDIR0);
	gXp_Wdir1 = XPLMFindDataRef(XP_WDIR1);
	gXp_Wdir2 = XPLMFindDataRef(XP_WDIR2);
	gXp_Magvar = XPLMFindDataRef(XP_MAGVAR);
	gXp_Avionics = XPLMFindDataRef(XP_AVIONICS);

	dir = "";
	nfichero = "";
//	fichero.clear() = NULL;
	atc = "";

	_enabled = true; // Se habilita la caja negra por defecto

	_activa = false; // Al inicio no está activada la caja

}

CajaNegra::~CajaNegra()
{
}

int CajaNegra::Inicio()
{
	if (!_activa && _enabled)
	{
//		char cdir[512];
		if (length(dir) == 0) dir = getXplaneHomeDir();
		int anio, mes, dia, hora, min, seg;
		datetime dhoy = now(false);
		decodedate(dhoy, anio, mes, dia);
//		if (anio > 1999) anio -= 2000; // Forma tosca de pasar años de 4 cifras a 2 cifras
		if (anio > 99) anio = anio % 100; // Pasa año a dos cifras
		decodetime(dhoy, hora, min, seg);
		nfichero = itostring(anio,10,2,'0') + itostring(mes,10,2,'0') + itostring(dia,10,2,'0') + itostring(hora,10,2,'0') + itostring(min,10,2,'0') + itostring(seg,10,2,'0') + ".AHX"; // Nombre de fichero en base a la fecha y hora actuales
		char ficheropath[512];
		sprintf(ficheropath, dir + nfichero);
//		fichero = fopen(ficheropath.c_str(), "a");
		fichero.open(ficheropath, std::ios::out | std::ios::binary);
		if (fichero.is_open())
		{
			xivap.uiWindow.addMessage(colCyan, "Caja Negra: Grabacion activada en carpeta '" + dir + "'", true, true);
			_numreg = 0;

			_activa = true;
			Graphics|=64;

			//FIXME: DEBUG
			if (xivap.debug.bb)
				xivap.addText(colRed,"Caja Negra: Abierto fichero '" + nfichero + "' para grabacion de caja negra", true, true);
		}
		else xivap.uiWindow.addMessage(colRed, "Caja Negra: Error intentando abrir archivo para grabacion en carpeta '" + dir + "'", true, true);

	}
	return 0;
}

int CajaNegra::Graba()
{
	if (XPLMGetDatai(gXp_Avionics) != 0 && _enabled)
	{
		Inicio();

//		s_caja rCaja;

		_rCaja.firma = CAJA_MAGIC_NUMBER;
		_rCaja.alt = XPLMGetDatad(gXp_ALT) * METROS_A_PIES;
		_rCaja.gl = (_rCaja.alt - (XPLMGetDataf(gXp_AGL)  * METROS_A_PIES));
		_rCaja.t_seg = XPLMGetDatai(gXp_ZuluS);
		_rCaja.t_min = XPLMGetDatai(gXp_ZuluM);
		_rCaja.t_hora = XPLMGetDatai(gXp_ZuluH);
		_rCaja.tierra = xivap.onGround();
		_rCaja.kias = XPLMGetDataf(gXp_Kias);
		_rCaja.mach = XPLMGetDataf(gXp_MSec) / XPLMGetDataf(gXp_SoundS);
		_rCaja.vs = XPLMGetDataf(gXp_Vs);
		_rCaja.stall = XPLMGetDatai(gXp_Stall);
		_rCaja.overspeed = XPLMGetDatai(gXp_OverS);
		_rCaja.pad1 = 0; // bytes sin usar, aparentemente
		_rCaja.lat = XPLMGetDatad(gXp_Lat);
		_rCaja.lon = XPLMGetDatad(gXp_Lon);
		_rCaja.qnh = XPLMGetDataf(gXp_Qnh);
//		_rCaja.cab = XPLMGetDataf(gXp_Pitch);
		_rCaja.cab = XPLMGetDataf(gXp_Pitch) * -1.0f; // Para compatibilidad con AHSBox
//		_rCaja.alab = XPLMGetDataf(gXp_Roll);
		_rCaja.alab = XPLMGetDataf(gXp_Roll) * -1.0f; // Para compatibilidad con AHSBox
		_rCaja.hdg = XPLMGetDataf(gXp_Hdg);
		if (XPLMGetDatai(gXp_Ap) == 2) _rCaja.autop = 1;
		else _rCaja.autop = 0;
		_rCaja.pparams = xivap.getParams();
		_rCaja.flaps = XPLMGetDataf(gXp_Flaps) * FLAPS_TOPE;
		_rCaja.tren = XPLMGetDatai(gXp_Gear) * TREN_TOPE;
		_rCaja.sim = XPLMGetDatai(gXp_SimSpeed);
		_rCaja.gs = XPLMGetDataf(gXp_GForce);
		_rCaja.fuel = XPLMGetDataf(gXp_Fuel) * KG_A_LB;
//		_rCaja.gps = XPLMGetDatai(gXp_Gps);
		_rCaja.gps = 0; // TODO: Encontrar la forma de saber si se "activa" el GPS
		_rCaja.payload = (XPLMGetDataf(gXp_Payload) * KG_A_LB) + 0.5f; // redondeo al entero más próximo
//		_rCaja.lbg = XPLMGetDataf(gXp_Lbg);
		_rCaja.lbg = 6.82; // TODO: Encontrar el DataRef de la densidad del fuel, si existe (en teoría, según el XPUIPC es de 6,82 Lb/gal)
		_rCaja.bruto = (XPLMGetDataf(gXp_Weight) * KG_A_LB) + 0.5f;
		_rCaja.pitot = XPLMGetDatai(gXp_Pitot);
		_rCaja.oat = XPLMGetDataf(gXp_Oat) + 0.5f;
		_rCaja.vis = (XPLMGetDataf(gXp_Vis) * METRO_A_SM) + 0.5f;
//		_rCaja.wknt = XPLMGetDataf(gXp_Wknt0);
		_rCaja.wknt = XPLMGetDataf(gXp_Wknt) + 0.5f;
//		_rCaja.wdir = XPLMGetDataf(gXp_Wdir0) + XPLMGetDataf(gXp_Magvar) + 0.5f;
		_rCaja.wdir = XPLMGetDataf(gXp_Wdir) + XPLMGetDataf(gXp_Magvar) + 0.5f;
//		_rCaja.atc[0] = 0; // TODO: Sacar la información del ATC de alguna parte (¿sintonización TS? ¿ATIS actual?) y averiguar tipo de string
//		atc = "PRUEBA_TWR"; // FIXME: DEBUG prueba para ver si la string es compatible
		if (xivap.online() && length(atc) > 0)
		{
//			strcpy(_rCaja.atc, pconst(atc)); // FIXME: PROBAR TIPO DE STRING SI ES CORRECTA -> ¡¡¡NO LO ES!!!
			_rCaja.atc[0] = length(atc);
			int i;
			for (i = 0; i < length(atc) && i < (MAX_ATC_LENGTH-1); ++i)
			{
				_rCaja.atc[i+1] = atc[i];
			}
			_rCaja.atc[i+1] = 0;
		}
		else
		{
			for (int i = 0; i < (MAX_ATC_LENGTH); ++i)
			{
				_rCaja.atc[i] = 0;
			}
		}

		//FIXME: DEBUG
		if (xivap.debug.bb)
		{
			xivap.addText(colRed, "Caja negra: Hora = " + itostring(_rCaja.t_hora) + ":" + itostring(_rCaja.t_min) + ":" + itostring(_rCaja.t_seg) + "; alt = " + ftoa(_rCaja.alt) + "; GL = " + ftoa(_rCaja.gl) + "; Tierra = " + itostring(_rCaja.tierra), true, true);
			xivap.addText(colRed, "Caja negra: KIAS = " + ftoa(_rCaja.kias) + "; Mach = " + ftoa(_rCaja.mach) + "; VS = " + itostring(_rCaja.vs) + "; Stall = " + itostring(_rCaja.stall) + "; Overspeed = " + itostring(_rCaja.overspeed), true, true);
			xivap.addText(colRed, "Caja negra: flaps = " + ftoa(_rCaja.flaps) + "; tren = " + ftoa(_rCaja.tren) + "; Gs = " + ftoa(_rCaja.gs) + "; OAT = " + itostring(_rCaja.oat), true, true);
		}
		if (fichero.is_open())
		{
			if (_numreg >= CAJA_INICIO_REG) // Sólo graba a partir de cierto registro, para que no empiece a registrar antes de que se "asiente" el avión
			{
				if (_rCaja.sim != 0) // Sólo graba si el simulador NO está en pausa, compatibilidad con AHSBOX
				{
					fichero.write((const char *)(&_rCaja), CAJA_TAM_REG);

					//FIXME: DEBUG
					if (xivap.debug.bb)
						xivap.addText(colRed, "Grabado registro num. " + itostring(_numreg) + " en fichero", true, true);
				}
			}
			++_numreg;
		}
	}
	else
	{
		Fin();
	}
	return 0;
}

int CajaNegra::Fin()
{
	if (_activa)
	{
		fichero.close();
		xivap.uiWindow.addMessage(colBlue, "Caja Negra: Grabacion finalizada", true, true);
		_activa = false;
		Graphics &=~64;

		//FIXME: DEBUG
		if (xivap.debug.bb)
			xivap.addText(colRed, itostring(_numreg) + " registros grabados.", true, true);
	}
	return 0;
}

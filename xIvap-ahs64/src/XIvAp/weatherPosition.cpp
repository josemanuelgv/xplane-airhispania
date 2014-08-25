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

#include "weatherPosition.h"
#include "xivap.h"
#include "helpers.h"

#include <algorithm>


WindLayer::WindLayer():
turbulence(FSD::TURB_NONE), windshear(FSD::SHEAR_GRADUAL),
alt(0), speed(0), gusts(0), direction(0), variance(0)
{
}

VisLayer::VisLayer():
visibility(0), base(0), tops(0)
{
}

CloudLayer::CloudLayer():
type(FSD::CLOUD_NONE),
base(0), tops(0), deviation(0), coverage(0),
top(FSD::CLTOP_FLAT),
turbulence(FSD::TURB_NONE),
precip(FSD::PRECIP_NONE),
precipbase(0), preciprate(0), icingrate(0)
{
}

TempLayer::TempLayer():
alt(0), day(0), daynightvar(0), dewpoint(0)
{
}

WxStation::WxStation(const string& name_):
name(name_), metar(""),
lat(0), lon(0), height(0), timestamp(0),
distance(0), qnh(0),
numWindlayers(0), numVislayers(0), numCloudlayers(0), numTemplayers(0),
navRef(XPLM_NAV_NOT_FOUND)
{
}

bool WxStation::isValid(float elapsed) const
{
	return windLayers.size() > 0 && visLayers.size() > 0
		&& tempLayers.size() > 0
		&& ((lat != 0 && lon != 0) || name == "GLOB");
}

bool WxStation::operator==(const WxStation& rhs) const
{
	if(this == &rhs) return true;
	return timestamp == rhs.timestamp && name == rhs.name;
}

void WxStation::dump() const
{
	// WX profile for EDNY (dist 33nm, valid): QNH 1022, #Wind: 4, #Vis: 1, #Cld: 3, #Tmp: 2
	string str = "WX profile for " + name + " (dist " + ftoa(distance) + "nm, ";
	str += (isValid() ? "valid" : "invalid");
	str += ", QNH " + ftoa((double)qnh);
	str += " #Wind: " + itostring(numWindlayers);
	str += " #Vis: " + itostring(numVislayers);
	str += " #Cld: " + itostring(numCloudlayers);
	str += " #Tmp: " + itostring(numTemplayers);
	xivap.addText(colLightGray, str, true, true);

	for(int i = 0; i < numWindlayers; ++i)
		xivap.addText(colLightGray, "Wind." + itostring(i) + " " + windLayers[i].dump(), true, true);
	for(int i = 0; i < numVislayers; ++i)
		xivap.addText(colLightGray, "Vis." + itostring(i) + " " + visLayers[i].dump(), true, true);
	for(int i = 0; i < numCloudlayers; ++i)
		xivap.addText(colLightGray, "Cld." + itostring(i) + " " + cloudLayers[i].dump(), true, true);
	for(int i = 0; i < numTemplayers; ++i)
		xivap.addText(colLightGray, "Temp." + itostring(i) + " " + tempLayers[i].dump(), true, true);

}

string WindLayer::dump() const
{
	string result = "Turb: ";
	switch(turbulence) {
		case FSD::TURB_NONE: result += "none"; break;
		case FSD::TURB_OCCASIONAL: result += "occas"; break;
		case FSD::TURB_LIGHT: result += "light"; break;
		case FSD::TURB_MODERATE: result += "moderate"; break;
		case FSD::TURB_SEVERE: result += "severe"; break;
	}

	result += " WShear: ";
	switch(windshear) {
		case FSD::SHEAR_GRADUAL: result += "gradual"; break;
		case FSD::SHEAR_MODERATE: result += "moderate"; break;
		case FSD::SHEAR_STEEP: result += "steep"; break;
		case FSD::SHEAR_INSTANT: result += "instant"; break;
	}
	result += " alt: " + itostring((int)alt) + " " 
		+ itostring((int)direction) + "@" + itostring((int)speed)
		+ " G" + itostring((int)gusts)
		+ " V" + itostring((int)variance);
	return result;
}

string VisLayer::dump() const
{
	string result = "Visibility " + itostring((int)visibility)
		+ " base " + itostring((int)base)
		+ " tops " + itostring((int)tops);
	return result;
}

string CloudLayer::dump() const
{
	string result;

	switch(type) {
		case FSD::CLOUD_NONE: result = "No Clds"; break;
		case FSD::CLOUD_CIRRUS: result = "Cirrus"; break;
		case FSD::CLOUD_CIRROSTRATUS: result = "CirrStrat"; break;
		case FSD::CLOUD_CIRROCUMULUS: result = "CirrCum"; break;
		case FSD::CLOUD_ALTOSTRATUS: result = "AltoStrat"; break;
		case FSD::CLOUD_ALTOCUMULUS: result = "AltoCum"; break;
		case FSD::CLOUD_STRATOCUMULUS: result = "StratCum"; break;
		case FSD::CLOUD_NIMBOSTRATUS: result = "NimbStrat"; break;
		case FSD::CLOUD_STRATUS: result = "Stratus"; break;
		case FSD::CLOUD_CUMULUS: result = "Cumulus"; break;
		case FSD::CLOUD_THUNDERSTORM: result = "Thdstrm"; break;
	}

	result += " base: " + itostring((int)base)
		+ " tops: " + itostring((int)tops)
		+ " dev: " + itostring((int)deviation)
		+ " cover: " + itostring(coverage) + "/8";
	
	result += " top: ";
	switch(top) {
		case FSD::CLTOP_FLAT: result += "flat"; break;
		case FSD::CLTOP_ROUND: result += "flat"; break;
		case FSD::CLTOP_ANVIL: result += "flat"; break;
		case FSD::CLTOP_MAX: result += "flat"; break;
	}

	result += " turb: ";
	switch(turbulence) {
		case FSD::TURB_NONE: result += "none"; break;
		case FSD::TURB_OCCASIONAL: result += "occas"; break;
		case FSD::TURB_LIGHT: result += "light"; break;
		case FSD::TURB_MODERATE: result += "moderate"; break;
		case FSD::TURB_SEVERE: result += "severe"; break;
	}

	result += " precip: ";
	switch(precip) {
		case FSD::PRECIP_NONE: result += "none"; break;
		case FSD::PRECIP_RAIN: result += "rain"; break;
		case FSD::PRECIP_SNOW: result += "snow"; break;
	}

	result += " base: " + itostring((int)precipbase)
		+ " rate: " + itostring((int)preciprate)
		+ " icing: " + itostring((int)icingrate);

	return result;
}

string TempLayer::dump() const
{
	string result = "alt: " + itostring((int)alt)
		+ " day: " + itostring((int)alt)
		+ " daynightvar: " + itostring((int)daynightvar)
		+ " dewpoint: " + itostring((int)dewpoint);
	return result;
}

FSD::CloudType WxDB::numTipoNube (string sTipoNube)
{
	if (sTipoNube == "FEW") return FSD::CLOUD_CIRROSTRATUS;
	if (sTipoNube == "SCT") return FSD::CLOUD_ALTOSTRATUS;
	if (sTipoNube == "BKN") return FSD::CLOUD_STRATOCUMULUS;
	if (sTipoNube == "OVC") return FSD::CLOUD_STRATUS;
	if (sTipoNube == "VV") return FSD::CLOUD_CUMULUS; //FIXME: No sé si meter aquí la visibilidad vertical como cielo totalmente cubierto a la altura dada
	return FSD::CLOUD_NONE; // FIXME: Si falla, dar algún error

}

void WxDB::add(const FSD::Message &m, float elapsed)
{
	string METAR = m.tokens[1];
//	std::vector<char> METAR(sMETAR.begin(), sMETAR.end());
//	METAR.push_back('\0');
//	char * pMETAR = reinterpret_cast <METAR>;
//	char * METAR = new char[sMETAR.size() + 1];
//	std::copy(sMETAR.begin(), sMETAR.end(), METAR);
//	METAR[sMETAR.size()] = '\0';
//	Decoded_METAR * Tiempo = new Decoded_METAR;
//	int res = DcdMETAR(&METAR[0], Tiempo);
	Decoded_METAR Tiempo;
	int res = DcdMETAR(&METAR[0], &Tiempo);
//FIXME: DEBUG
	if (xivap.debug.weather > 1)
		xivap.addText(colRed, "WxDB::add.res(DcdMETAR) = " + itostring(res), true, true);

//	string stationName = m.tokens[0];
//	string stationName = copy(m.tokens[1],0,pos(" ",m.tokens[1]));
	string stationName = Tiempo.stnid;
//FIXME: DEBUG
	if (xivap.debug.weather > 1)
		xivap.addText(colRed, "WxDB::add.stationName = " + stationName, true, true);
	string sMETAR = fill(1000,' ');
//	sprint_metar(&sMETAR[0],Tiempo);
	sprint_metar(&sMETAR[0],&Tiempo);
	if (xivap.debug.weather > 2)
	{
		xivap.addText(colRed, "WxDB::add.Tiempo = ", true, true);

//#ifdef WX_DEBUG
		xivap.addText(colRed, " ", true, true);
		int siglinea = 0;
		while (sMETAR != "" && siglinea != -1)
		{
			siglinea = pos(10,sMETAR);
			if (siglinea != -1)
			{
				xivap.addText(colRed, copy(sMETAR,0,siglinea+1), true, true);
				del(sMETAR,0,siglinea+1);
			}
			else 
			{
				xivap.addText(colRed, copy(sMETAR,0), true, true);
				sMETAR = "";
			}
		}
//#endif
	}
	// Final DEBUG


	WxMap::iterator it = _map.find(pconst(stationName));
	if(it == _map.end()) {
		WxStation s(stationName);
//		s.metar = METAR; // Almacena el METAR en la estación para futuras consultas

		if(stationName != "GLOB") { // X-Plane doesnt know GLOB of course

			// set lat+lon if airport is known
			// first, ask X-Plane if it knows the airport
			s.navRef = XPLMFindNavAid(NULL, stationName, NULL, NULL, NULL, xplm_Nav_Airport);
			if(s.navRef != XPLM_NAV_NOT_FOUND) {
				XPLMGetNavAidInfo(s.navRef,	NULL, &s.lat, &s.lon, &s.height,
							NULL, NULL, NULL, NULL, NULL);
			} else {
				// X-Plane doesnt know it, try our own DB...
				PosMap::const_iterator it = _positions.find(pconst(stationName));
				if(it != _positions.end()) {
					s.lat = it->second.lat;
					s.lon = it->second.lon;
					s.height = it->second.alt;
				}
			}

		}

		_map[pconst(stationName)] = s;

//FIXME: DEBUG
		if (xivap.debug.weather > 1)
			xivap.addText(colRed, "WxDB::add. METIDO " + stationName +" en _map", true, true);

		it = _map.find(pconst(stationName));
	}
	if(it == _map.end()) return; // paranoia

	it->second.metar = METAR; // Almacena el METAR en la estación para futuras consultas

	switch(m.type) {

		case _FSD_WEAGENERAL_:
			// new weather data for this station
			it->second.windLayers.clear();
			it->second.visLayers.clear();
			it->second.tempLayers.clear();
			it->second.cloudLayers.clear();
			it->second.qnh = static_cast<float>(atof(m.tokens[1]));
			it->second.numWindlayers = static_cast<int>(stringtoi(m.tokens[2]));
			it->second.numVislayers = static_cast<int>(stringtoi(m.tokens[3]));
			it->second.numCloudlayers = static_cast<int>(stringtoi(m.tokens[4]));
			it->second.numTemplayers = static_cast<int>(stringtoi(m.tokens[5]));

#//ifdef WX_DEBUG
			if (xivap.debug.weather > 2)
				if(it->second.qnh < 800)
					xivap.addText(colYellow, "QNH no correcto en mensaje del tiempo: " + m.encoded);
//#endif

			break;

		// Único caso para la red de AHS, recepción del METAR completo
		case _FSD_WEADATA_RC_AHS_:  {
			it->second.windLayers.clear();
			it->second.visLayers.clear();
			it->second.tempLayers.clear();
			it->second.cloudLayers.clear();

			it->second.numWindlayers = 0;
			it->second.numVislayers = 0;
			it->second.numTemplayers = 0;
			it->second.numCloudlayers = 0;

			WindLayer l;
			//	[ alt speed gusts direction variance turbulence windshear ]
			if (!xivap.cavok)
			{
				l.alt =				0; // Vientos en tierra (no sé cómo habría que poner la altura realmente)
				l.speed =			Tiempo.winData.windSpeed;
				if (Tiempo.winData.windGust != INT_MAX) // La librería lo inicializa al valor #define INT_MAX
				{
					l.gusts =		Tiempo.winData.windGust; // Supuestamente hay datos de rachas de viento para copiar
				}
				else
				{
					l.gusts =		0; // FIXME: No sé si poner la misma velocidad del viento o 0
				}
				if (Tiempo.winData.windVRB != 0) // Como no parece haber variable para vientos variables, pongo la máxima variación que creo que se puede poner
				{
					l.direction =	0;
					l.variance =	180;
					l.gusts =		l.speed*2.0f;
					l.turbulence =	FSD::TURB_NONE; // No parece que se pueda obtener del METAR
					l.windshear =	FSD::SHEAR_MODERATE ; // No parece que se pueda obtener del METAR (posiblemente pueda hacerse por el código WS de cizalladura en pista)
				}
				else 
				{
					l.direction =	Tiempo.winData.windDir;
					l.turbulence =	FSD::TURB_NONE; // No parece que se pueda obtener del METAR
					l.windshear =	FSD::SHEAR_GRADUAL ; // No parece que se pueda obtener del METAR (posiblemente pueda hacerse por el código WS de cizalladura en pista)
	
					if (Tiempo.minWnDir != INT_MAX) //FIXME: Creo que sería mejor poner cada dirección del viento en una capa distinta
					{
						l.direction =	Tiempo.winData.windDir; // FIXME: ¡¡¡PROBAR!!!
//						l.direction =	(Tiempo.maxWnDir + Tiempo.minWnDir) / 2;
						l.variance =	Tiempo.maxWnDir - Tiempo.minWnDir; // FIXME: No sé si debe ser la diferencia o la dirección "máxima" ¡¡¡PROBAR!!!
//						l.direction =	Tiempo.minWnDir;
//						l.variance =	Tiempo.maxWnDir; // FIXME: Creo que debe ser la dirección "máxima"
						if (Tiempo.winData.windGust == INT_MAX) // La librería lo inicializa al valor #define INT_MAX
							l.gusts =		l.speed*2.0f;
						l.windshear =	FSD::SHEAR_MODERATE; // No parece que se pueda obtener del METAR (posiblemente pueda hacerse por el código WS de cizalladura en pista)
					}
					else
					{
						l.variance =	0;
					}
	//				l.turbulence =		static_cast<FSD::WindTurb>(stringtoi(m.tokens[j++]));
	//				l.windshear =		static_cast<FSD::WindShear>(stringtoi(m.tokens[j++]));
				}
// FIXME: DEBUG
			} // if !cavok
			else
			{
				l.alt =				0; // Vientos en tierra (no sé cómo habría que poner la altura realmente)
				l.speed =			0;
				l.direction =		0;
				l.variance =		0;
				l.gusts =			0;
				l.turbulence =		FSD::TURB_NONE; // No parece que se pueda obtener del METAR
				l.windshear =		FSD::SHEAR_GRADUAL; // No parece que se pueda obtener del METAR (posiblemente pueda hacerse por el código WS de cizalladura en pista)
			}


			if (xivap.debug.weather > 1)
				xivap.addText(colRed, "WxDB::add.Vientos de " + ftoa(l.direction) + "º a " + ftoa(l.speed) + " Kt con rachas de " + ftoa(l.gusts) + " Kt variables en " + ftoa(l.variance) + "º (variable = " + itostring(Tiempo.winData.windVRB) + ")", true, true);
				
			it->second.windLayers.push_back(l);
//			it->second.numWindlayers += 1; // Prueba con una sola capa de viento con el único dato de vientos que se recibe en el METAR
			it->second.windLayers.push_back(l);
			it->second.windLayers.push_back(l);
			it->second.numWindlayers = 3; // Prueba con tres capas de viento iguales con el único dato de vientos que se recibe en el METAR


			VisLayer l2;
			// [ visibility base tops ]
			if (Tiempo.CAVOK || Tiempo.prevail_vsbyM == 9999.0f || xivap.cavok) l2.visibility = MAX_VISIBILITY; // Máxima visibilidad
			else if (Tiempo.prevail_vsbyM != (float) INT_MAX) l2.visibility = Tiempo.prevail_vsbyM; // FIXME: Añadir otras visibilidades (pista, etc.) y traducir a la unidad que tenga que ser (asumo metros, pero no lo sé)
			else if (Tiempo.prevail_vsbySM != (float) INT_MAX) l2.visibility = Tiempo.prevail_vsbySM * 1609.344f; // Statute miles pasadas a metros
			l2.base = 0; // Altitud de base de visibilidad
			l2.tops = 300; // Pongo altura 300 m por si acaso

// FIXME: DEBUG
			if (xivap.debug.weather > 1)
				xivap.addText(colRed, "WxDB::add.Visibilidad horizontal de " + ftoa(l2.visibility) + "m desde " + ftoa(l2.base) + "m hasta " + ftoa(l2.tops) + "m de altura", true, true);

			it->second.visLayers.push_back(l2);
			it->second.visLayers.push_back(l2);
			it->second.visLayers.push_back(l2);
//			it->second.numVislayers += 1;
			it->second.numVislayers = 3; // Prueba con 3 capas iguales

			CloudLayer l3;
			// [ cloudtype cloudbase cloudtops clouddeviation cloudcoverage cloudtop
			//   cloudturbulence preciptype precipbase preciprate icingrate ]
			if (Tiempo.CAVOK || xivap.cavok) // Si hay tiempo CAVOK forzado o por METAR
			{
				l3.type = FSD::CLOUD_NONE;
				l3.coverage = 0;
				l3.base = 0;
				l3.tops = 100000; // Altura estratosférica
				l3.top =			FSD::CLTOP_FLAT;
				l3.turbulence =		FSD::TURB_NONE; 					
				l3.precip =			FSD::PRECIP_NONE;
				l3.precipbase =		0; 
				l3.preciprate =		0;
				l3.icingrate =		0;

				it->second.cloudLayers.push_back(l3);
				it->second.cloudLayers.push_back(l3);
				it->second.cloudLayers.push_back(l3);
				it->second.numCloudlayers = 3; // Tres capas de nubes limpias para asegurar CAVOK
			}
			else  // Sacar información de nubes del METAR
			{

				for (int lnubes = 0; lnubes < 6 && Tiempo.cldTypHgt[lnubes].cloud_hgt_meters != INT_MAX; ++lnubes)
				{
					if (Tiempo.cldTypHgt[lnubes].other_cld_phenom == "TCU")
					{
						l3.type =		FSD::CLOUD_THUNDERSTORM; // Tipo FSD para los "towering cumulus"
						l3.coverage =	(int)numTipoNube(string(Tiempo.cldTypHgt[lnubes].cloud_type));
					}
					else if (Tiempo.cldTypHgt[lnubes].other_cld_phenom == "CB")
						{
							l3.type =		FSD::CLOUD_CUMULUS; // Tipo FSD para los "cumulus"
							l3.coverage =	(int)numTipoNube(string(Tiempo.cldTypHgt[lnubes].cloud_type)); 
						}
						else
						{
							l3.type =		numTipoNube(string(Tiempo.cldTypHgt[lnubes].cloud_type)); //static_cast<FSD::CloudType>(stringtoi(m.tokens[j++]));
							l3.coverage =	(int)numTipoNube(string(Tiempo.cldTypHgt[lnubes].cloud_type));
						}
					l3.base =			static_cast<float>(Tiempo.cldTypHgt[lnubes].cloud_hgt_meters) + it->second.height; // Hay que añadir la elevación del AD porque el dato tiene que ser en metros AMSL
					l3.tops =			l3.base + 30; // Pongo altura fija de 30 m por si acaso
					l3.deviation =		0;
					l3.top =			FSD::CLTOP_FLAT; //	static_cast<FSD::CloudTop>(stringtoi(m.tokens[j++]));
					l3.turbulence =		FSD::TURB_NONE; //static_cast<FSD::WindTurb>(stringtoi(m.tokens[j++]));
					
					l3.precip =			FSD::PRECIP_NONE; //static_cast<FSD::CloudPrecip>(stringtoi(m.tokens[j++]));
//					l3.precipbase =		static_cast<float>(Tiempo.cldTypHgt[lnubes].cloud_hgt_meters); // FIXME: Pongo la base a la altura de las nubes, pero no sé si es correcto
					l3.precipbase =		0; // FIXME: Pongo la base a la altura de tierra, pero no sé si es correcto
					l3.preciprate =		0;
					l3.icingrate =		0;
	
					// paranoia
					if(l3.tops < l3.base) l3.tops = l3.base + 250;

// FIXME: DEBUG
					if (xivap.debug.weather > 1)
						xivap.addText(colRed, "WxDB::add.Nubes tipo " + string(Tiempo.cldTypHgt[lnubes].cloud_type) + " (" + itostring ((int)l3.type) + ") a " + ftoa(l3.base) + " m (" + string(Tiempo.cldTypHgt[lnubes].cloud_hgt_char) + "00 ft)", true, true);
	
					it->second.cloudLayers.push_back(l3);
					it->second.numCloudlayers += 1;
				}
			

			 int i = 0;
			 while (i < 3 && Tiempo.WxObstruct[i][0] != NULL) // Hasta 3 capas de nubes
			 {
				if (i < it->second.cloudLayers.size()) // Si existe capa de nubes para meter esta precipitación
				{
					l3 = it->second.cloudLayers[i]; // Se copia la información de nubes existente
				}
				else
				{
					l3.base =			0;
					l3.tops =			4500; // Pongo tope de altura en 4500m
					l3.deviation =		0;
					l3.top = FSD::CLTOP_FLAT;
					l3.turbulence =		FSD::TURB_NONE;
					
					it->second.cloudLayers.push_back(l3); // Meto nueva capa para meter precipitación
					it->second.numCloudlayers += 1;
				}

				string tiemposig = string(Tiempo.WxObstruct[i]);
//				float multi = 1;
//				if (pos("+",tiemposig) != -1) multi = 2.0f; // TODO: El modificador "+" multiplica por 2 las precipitaciones (en prueba)
//				else if (pos("-",tiemposig) != -1) multi = 0.5f; // TODO: El modificador "-" divide por 2 las precipitaciones (en prueba)
				float suma = 0;
				if (pos("+",tiemposig) != -1) suma = 1.0f; // TODO: El modificador "+" multiplica por 2 las precipitaciones (en prueba)
				else if (pos("-",tiemposig) != -1) suma = -1.0f; // TODO: El modificador "-" divide por 2 las precipitaciones (en prueba)

				// TODO: Faltan más modificadores como SG (cinarra), IC (cristales de polvo), nieblas, arenas y demás que afectarían también a la capa de visibilidad...
				if (pos("DZ",tiemposig) != -1) // Llovizna
				{
					l3.precip =		FSD::PRECIP_RAIN;
					l3.precipbase = 0; // Pongo la base a tierra
					l3.preciprate = 1.0f+(suma/2.0f); // Asumo una escala entre 0 y 5.0f (que sería lluvia torrencial máxima)
					l3.icingrate =	0;
				}
				else if (pos("SHRA",tiemposig) != -1) // Chubasco o lluvia muy fuerte
				{
					l3.precip =		FSD::PRECIP_RAIN;
					l3.precipbase = 0; // Pongo la base a tierra
					l3.preciprate = 4.0f+suma; // Asumo una escala entre 0 y 5.0f (que sería lluvia torrencial máxima)
					l3.icingrate =	0;
				}
				else if (pos("RA",tiemposig) != -1) // Lluvia
				{
					l3.precip =		FSD::PRECIP_RAIN;
					l3.precipbase = 0; // Pongo la base a tierra
					l3.preciprate = 2.5f+suma; // Asumo una escala entre 0 y 5.0f (que sería lluvia torrencial máxima)
					l3.icingrate =	0;
				}
				else if (pos("SN",tiemposig) != -1) // Nieve
				{
					l3.precip =		FSD::PRECIP_SNOW;
					l3.precipbase = 0; // Pongo la base a tierra
					l3.preciprate = 3.0f+suma; // Asumo una escala entre 0 y 5.0f (que sería lluvia torrencial máxima)
					l3.icingrate =	4.0f+suma;
				}
				else if (pos("GR",tiemposig) != -1) // Granizo
				{
					l3.precip =		FSD::PRECIP_SNOW;
					l3.precipbase = 0; // Pongo la base a tierra
					l3.preciprate = 4.0f+suma; // Asumo una escala entre 0 y 100 (que sería lluvia torrencial máxima)
					l3.icingrate =	4.0f+suma;
				}
				else if (pos("GS",tiemposig) != -1) // Granizo pequeño
				{
					l3.precip =		FSD::PRECIP_SNOW;
					l3.precipbase = 0; // Pongo la base a tierra
					l3.preciprate = 1.5f+suma; // Asumo una escala entre 0 y 100 (que sería lluvia torrencial máxima)
					l3.icingrate =	2.0f+suma;
				}

// FIXME: DEBUG
				if (xivap.debug.weather > 1)
					xivap.addText(colRed, "WxDB::add.Precipitaciones (" + itostring(i) + ") tipo " + itostring((int)l3.precip) + " (" + tiemposig + ") a ritmo " + ftoa(l3.preciprate) + " de precipitación y " + ftoa(l3.icingrate) + " de engelamiento", true, true);

				it->second.cloudLayers[i]= l3;
				++i;
			 }
			} // no CAVOK
		
			TempLayer l4;
			// [ alt day daynightvar dewpoint ]
			l4.alt =			0; // Pongo altura a tierra porque sólo hay un dato (capa) disponible
			l4.day =			Tiempo.temp; // Entiendo que será en Celsius
			l4.daynightvar =	0; // No hay datos sobre variación así que entiendo que 0 significa sin variación
			l4.dewpoint =		Tiempo.dew_pt_temp; // Entiendo que será en Celsius

			it->second.tempLayers.push_back(l4);
			it->second.numTemplayers += 1;

// FIXME: DEBUG
			if (xivap.debug.weather > 2)
				xivap.addText(colRed, "WxDB::add.Temperaturas " + ftoa(l4.day) + "º C de ambiente y  " + ftoa(l4.dewpoint) + "º C de punto de rocío", true, true);

			if (Tiempo.Q_altstng) it->second.qnh = Tiempo.hectoPasc_altstng;
			else if (Tiempo.A_altstng) it->second.qnh = Tiempo.inches_altstng * QNH_ALTIMETER_FACTOR;

// FIXME: DEBUG
			if (xivap.debug.weather > 2)
				xivap.addText(colRed, "WxDB::add.QNH " + ftoa(it->second.qnh) + " mb (HPa)", true, true);

			}
			break;

		case _FSD_WEAWINDLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*7 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					WindLayer l;
					//	[ alt speed gusts direction variance turbulence windshear ]
					l.alt =				static_cast<float>(atof(m.tokens[j++]));
					l.speed =			static_cast<float>(atof(m.tokens[j++]));
					l.gusts =			static_cast<float>(atof(m.tokens[j++]));
					l.direction =		static_cast<float>(atof(m.tokens[j++]));
					l.variance =		static_cast<float>(atof(m.tokens[j++]));
					l.turbulence =		static_cast<FSD::WindTurb>(stringtoi(m.tokens[j++]));
					l.windshear =		static_cast<FSD::WindShear>(stringtoi(m.tokens[j++]));

					it->second.windLayers.push_back(l);
				}
			}
			break;

		case _FSD_WEAVISLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*3 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					VisLayer l;
					// [ visibility base tops ]
					l.visibility =		static_cast<float>(atof(m.tokens[j++]));
					l.base =			static_cast<float>(atof(m.tokens[j++]));
					l.tops =			static_cast<float>(atof(m.tokens[j++]));

					it->second.visLayers.push_back(l);
				}
			}
			break;

		case _FSD_WEACLOUDLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*11 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					CloudLayer l;
					// [ cloudtype cloudbase cloudtops clouddeviation cloudcoverage cloudtop
					//   cloudturbulence preciptype precipbase preciprate icingrate ]

					l.type =			static_cast<FSD::CloudType>(stringtoi(m.tokens[j++]));
					l.base =			static_cast<float>(atof(m.tokens[j++]));
					l.tops =			static_cast<float>(atof(m.tokens[j++]));
					l.deviation =		static_cast<float>(atof(m.tokens[j++]));
					l.coverage =		static_cast<int>(stringtoi(m.tokens[j++]));
					l.top =				static_cast<FSD::CloudTop>(stringtoi(m.tokens[j++]));
					l.turbulence =		static_cast<FSD::WindTurb>(stringtoi(m.tokens[j++]));
					l.precip =			static_cast<FSD::CloudPrecip>(stringtoi(m.tokens[j++]));
					l.precipbase =		static_cast<float>(atof(m.tokens[j++]));
					l.preciprate =		static_cast<float>(atof(m.tokens[j++]));
					l.icingrate =		static_cast<float>(atof(m.tokens[j++]));

					// paranoia
					if(l.tops < l.base) l.tops = l.base + 250;

					it->second.cloudLayers.push_back(l);
				}
			}
			break;

		case _FSD_WEATEMPLAY_: {
				if(m.tokens.size() < 2) return;

				unsigned int count = static_cast<unsigned int>(stringtoi(m.tokens[1]));
				if(m.tokens.size() < count*4 + 2) return;

				int j = 2;
				for(unsigned int i = 0; i < count; ++i) {
					TempLayer l;
					// [ alt day daynightvar dewpoint ]
					l.alt =				static_cast<float>(atof(m.tokens[j++]));
					l.day =				static_cast<float>(atof(m.tokens[j++]));
					l.daynightvar =		static_cast<float>(atof(m.tokens[j++]));
					l.dewpoint =		static_cast<float>(atof(m.tokens[j++]));

					it->second.tempLayers.push_back(l);
				}
			}
			break;
	}
	it->second.timestamp = elapsed;
//	delete Tiempo;
}

WxStation WxDB::findName(string name, float elapsed)
{
	WxMap::iterator i = _map.find(std::string((const char*)name));
	if(i != _map.end())
		if(i->second.isValid(elapsed))
			return i->second;

	return WxStation();
}

std::vector<WxStation> WxDB::estacionesCercanas(double lat, double lon, float elapsed)
{
	std::vector<WxStation> result;
//FIXME: Cambiar el método para encontrar estaciones cercanas (¿búsqueda en la "nav" dB del XPlane?)
	int cuenta = 0;
	double distancia;
	for(PosMap::iterator i = _positions.begin(); i != _positions.end(); ++i)
	{
		distancia = deg2dist(lat, lon, static_cast<double>(i->second.lat), static_cast<double>(i->second.lon));
		WxStation res;
		if (distancia <= WEATHER_GLOB_TRESHOLD)
		{
			res.name = i->second.icao;
			res.distance = distancia;
			res.lat = i->second.lat;
			res.lon = i->second.lon;
			result.push_back(res); // Si la estación está lo sucifientemente cerca de la posición del avión (<=60Nm), la mete en la lista

//FIXME: DEBUG
			if (xivap.debug.weather > 1)
				xivap.addText(colWhite, "Encontrado en la dB de X-IvAp ('icao.dat'): " + res.name + " a " + ftoa(res.distance) + " Nm", true, true);
		}

		++cuenta;
	}
	std::sort(result.begin(), result.end(), DistanceComparator());

//FIXME: DEBUG
	if (xivap.debug.weather > 1)
		xivap.addText(colRed, "Estaciones totales = " + itostring(cuenta) + " - Estaciones cercanas a menos de 60 Nm = " + itostring((int)result.size()), true, true);

	char aptBuf[64];
	float latf = static_cast<float>(lat);
	float lonf = static_cast<float>(lon);
    XPLMNavRef ref = XPLMFindNavAid(NULL, NULL, &latf, &lonf, NULL, xplm_Nav_Airport); // Busca el aeropuerto más cercano en la dB de X-Plane
    if (ref != XPLM_NAV_NOT_FOUND){
        XPLMGetNavAidInfo(ref, NULL, &latf, &lonf, NULL, NULL, NULL, aptBuf, NULL, NULL); // Obtiene los datos del aeropuerto encontrado
		string adcerca = trim(string(aptBuf));
		distancia = deg2dist(lat, lon, static_cast<double>(latf), static_cast<double>(lonf));

		if (result.size() > 0)
		{
			if (distancia < result[0].distance) // Si el aeropuerto encontrado está a menos distancia que el más cercano de la base de posiciones
			{
				WxStation res;
				res.name = copy(adcerca,0,4);
				res.distance = distancia;
				res.lat = latf;
				res.lon = lonf;
				result.insert(result.begin(),res); // Se mete el AD el primero de la lista

//FIXME: DEBUG
				if (xivap.debug.weather > 1)
					xivap.addText(colWhite, "Encontrado AD en la dB de X-plane: " + res.name + " a " + ftoa(res.distance) + " Nm", true, true);
			}
		}
		else
		{
			if (distancia <= WEATHER_GLOB_TRESHOLD) // Si el aeropuerto encontrado está a un máximo de 60 Nm
			{
				WxStation res;
				res.name = copy(adcerca,0,4);
				res.distance = distancia;
				res.lat = latf;
				res.lon = lonf;
				result.insert(result.begin(),res); // Se mete el AD el primero de la lista

//FIXME: DEBUG
				if (xivap.debug.weather > 1)
					xivap.addText(colWhite, "Encontrado AD en la dB de X-plane: " + res.name + " a " + ftoa(res.distance) + " Nm", true, true);

			}
		}
    }

	return result;
}


std::vector<WxStation> WxDB::get(double lat, double lon, float elapsed)
{
	std::vector<WxStation> result;

	for(WxMap::iterator i = _map.begin(); i != _map.end(); ++i) {
		if(i->second.isValid(elapsed)) {
			i->second.distance = deg2dist(lat, lon, i->second.lat, i->second.lon);
			result.push_back(i->second);
		}
	}

	std::sort(result.begin(), result.end(), DistanceComparator());

	return result;
}

void WxDB::discardObsolete(float elapsed)
{
	for(WxMap::iterator i = find_if(_map.begin(), _map.end(), InvalidPredicate);
		i != _map.end(); i = find_if(_map.begin(), _map.end(), InvalidPredicate))
			_map.erase(i);
}

void WxDB::loadPositions(const string& filename)
{
	_positions.clear();
	FILE *in = fopen(pconst(filename), "r");
	if(in == NULL) return;

	char l[512];
	int linesize = 511;
	string line;
	// read all positions into our map, altitudes are in feet
	// LOWS  47.794000 013.003000 1401
	WxPos pos;
	do {
		fgets(l, linesize, in);
		l[linesize] = 0;
		if(feof(in)) line = "";
		else {
			line = l;
			pos.icao = pconst(copy(line, 0, 4));
			del(line, 0, 4);
			line = trim(line);
			int p = 0;
			// search for first blank or - char
			while(line[p] != ' ' && line[p] != '-' && p < length(line)) ++p;
			if(p >= length(line)) continue;
			pos.lat = static_cast<float>(atof(copy(line, 0, p)));
			del(line, 0, p);
			line = trim(line);
			while(line[p] != ' ' && line[p] != '-' && p < length(line)) ++p;
			if(p >= length(line)) continue;
			pos.lon = static_cast<float>(atof(copy(line, 0, p)));
			del(line, 0, p);
			line = trim(line);
			pos.alt = static_cast<float>(atof(line)) / 3.2808399f; // convert feet to meters
			_positions[pos.icao] = pos;
		}
	} while(length(line) > 0);
	fclose(in);
}

void WxStation::sortWindLayers(float altitude, size_t layers)
{
	if(windLayers.size() > layers) {

		// first, sort by vertical distance
		std::sort(windLayers.begin(), windLayers.end(), WindComparatorVdist(altitude));

		if(windLayers.size() > layers) {
			// if we have enough layers, make sure that the wind layers below and above are set
			bool have_above = false;
			bool have_below = false;
			for(size_t i = 0; i < layers; ++i) {
				if(windLayers[i].alt < altitude) have_below = true;
				else if(windLayers[i].alt > altitude) have_above = true;
				else { have_below = true; have_above = true; }; // exact altitude match
			}

			if(!have_below) {
				// look for the first wind layer below
				for(size_t i = layers; i < windLayers.size(); ++i) {
					if(windLayers[i].alt < altitude) { // ... and swap it with the 3th layer in the vector
						WindLayer l = windLayers[2];   // (which is the one most far away from us)
						windLayers[2] = windLayers[i];
						windLayers[i] = l;
						return;
					}
				}
			} else if(!have_above) {
				// look for the first wind layer above
				for(size_t i = layers; i < windLayers.size(); ++i) {
					if(windLayers[i].alt > altitude) { // ... and swap it with the 3th layer in the vector
						WindLayer l = windLayers[2];   // (which is the one most far away from us)
						windLayers[2] = windLayers[i];
						windLayers[i] = l;
						return;
					}
				}
			}
		} // if layers > windLayers.size()

		// sort the first 3 entrys by their altitude
		std::vector<WindLayer>::iterator middle = windLayers.begin();
		for(size_t i = 0; i < layers && middle != windLayers.end(); ++i) ++middle;
		std::sort(windLayers.begin(), middle, WindComparatorAlt());

	}
}

void WxStation::sortVisLayers(float altitude)
{
}

void WxStation::debugDump() const
{
#ifndef APPLE
	xivap.addText(colWhite,	string("WX station dump for ") + name, true, true);
	xivap.addText(colWhite,	string("valid: ") + (isValid() ? "yes" : "no")
		+ " lat " + ftoa(lat) + " lon " + ftoa(lon) + " alt " + ftoa(height)
		+ " timestamp " + ftoa(timestamp) + " dist " + ftoa(distance) + " qnh " + ftoa(qnh),
		false, true);

	/*disabled for x64 work, itostring gives problems 24/12/2012 bvk
xivap.addText(colWhite,	string("# wind layers: ") + itostring(windLayers.size()), false, true);
	for(size_t i = 0; i < windLayers.size(); ++i) {
		xivap.addText(colWhite, string("#") + itostring(i)
			+ " alt " + ftoa(windLayers[i].alt) + " speed " + ftoa(windLayers[i].speed)
			+ " gusts " + ftoa(windLayers[i].gusts) + " dir " + ftoa(windLayers[i].direction)
			+ " variance " + ftoa(windLayers[i].variance)
			+ " turb " + itostring(windLayers[i].turbulence) + " shear " + itostring(windLayers[i].windshear),
			false, true);
	}


	xivap.addText(colWhite,	string("# visibility layers: ") + itostring(visLayers.size()), false, true);
	for(size_t i = 0; i < visLayers.size(); ++i) {
		xivap.addText(colWhite, string("#") + itostring(i)
			+ " vis " + ftoa(visLayers[i].visibility) + " base " + ftoa(visLayers[i].base)
			+ " top " + ftoa(visLayers[i].tops),
			false, true);
	}

	xivap.addText(colWhite,	string("# cloud layers: ") + itostring(cloudLayers.size()),	false, true);
	for(size_t i = 0; i < cloudLayers.size(); ++i) {
		xivap.addText(colWhite, string("#") + itostring(i)
			+ " type " + itostring(cloudLayers[i].type)
			+ " coverage " + itostring(cloudLayers[i].coverage)
			+ " base " + ftoa(cloudLayers[i].base)
			+ " tops " + ftoa(cloudLayers[i].tops) + " deviat " + ftoa(cloudLayers[i].deviation)
			+ " precbase " + ftoa(cloudLayers[i].precipbase) + " precrate " + ftoa(cloudLayers[i].preciprate)
			+ " icingrate " + ftoa(cloudLayers[i].icingrate)
			+ " toptype " + itostring(cloudLayers[i].top) + " turb " + itostring(cloudLayers[i].turbulence)
			+ " precip " + itostring(cloudLayers[i].precip),
			false, true);
	}
*/
#endif // apple
}

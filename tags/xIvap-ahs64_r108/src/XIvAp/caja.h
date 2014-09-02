/******************************************************************************
 * Grabación de caja negra para X-IvAp AHS									  *
 * Copyright (C) 2014 by Mikel N'Dong (AHS120D)								  *
 *																			  *
 ******************************************************************************/

#ifndef CAJA_H_
#define CAJA_H_

#include "helpers.h" // "common.h" // "xivap.h"
#include "XPLMDataAccess.h"
#include "common.h"
#include "SoundGraphics.h"
#include <iostream>
#include <fstream>

#define MAX_ATC_LENGTH 15

struct s_caja
{
    long long int firma; // =0x56; // ("V")
    double gl; // Elevación de terreno en pies
    UInt32 t_seg; // Segundos de la hora de grabación (offset 0x10)
    UInt32 t_hora; // Hora de la grabación
    UInt32 t_min; // Minuto de la grabación
    UInt32 tierra; // 0=False;1=True
    double kias; // Velocidad en KIAS (offset 0x20)
    double mach; // Velocidad en Mach
    int vs; // vertical speed en ftm/ppm (offset 0x30)
    UInt8 stall; // 0=false
    UInt8 overspeed; // 0= false
    UInt16 pad1; // = 0; Relleno para que encajen los siguientes registros
    double lat; // latitud
    double lon; // longitud (offset 0x40)
    double alt; // pies AMSL
    double qnh; // calaje altímetro "QNH" en pulgadas (inHg) (offset 0x50)
    double cab; // grados de cabeceo
    double alab; // grados de alabeo (offset 0x60)
    double hdg; // "Heading"
//	long long int autop; // autopiloto 0=false (Offset: 0x70)
	UInt32 autop; // autopiloto 0=false (Offset: 0x70)
//	FSD::PlaneParams pparams; // 'Plane params' añadido en offsets: 0x74 - 0x77, presuntamente sin usar
	UInt32 pparams; // 'Plane params' añadido en offsets: 0x74 - 0x77, presuntamente sin usar
	double flaps; // 0=0º;5460(0x1554)=10º;10922(0x2AAA)=20º;16383 (0x3FFF)=30º (¿Fracciones de 16384-1?)
    double tren; // tren de aterrizaje 0=subido; 16383=bajado (offset 0x80)
    UInt32 sim; // "Sim", supuestamente la velocidad de simulación, normalmente 1
	UInt32 pad2; // Relleno para que encajen los siguientes registros
    double gs; // Fuerza G soportada (offset 0x90)
    double fuel; // Combustible que queda
    UInt32 gps; // 0=false (offset 0xA0)
    UInt32 payload;
    double lbg; // libras por gramo, normalmente 6
    UInt32 bruto; // peso total (0xB0)
    UInt32 pitot; // 0=False
    int oat; // ºC
    UInt32 vis; // Visibilidad (nm)
    UInt32 wknt; // Velocidad del viento (knt) (offset 0xC0)
    UInt32 wdir; // Dirección del viento
    char atc[MAX_ATC_LENGTH + 1]; // Determinar tipo de string (offset 0xC8)
};

#define CAJA_TAM_REG 0xD8 // (216) Tamaño de cada registro de la caja negra
#define CAJA_MAGIC_NUMBER 0x56 // (86) "V" "Magic number" de inicio del registro de la caja negra

#define XP_ALT "sim/flightmodel/position/elevation" //	double	n	meters	The elevation above MSL of the aircraft
#define XP_AGL "sim/flightmodel/position/y_agl" // float	n	meters	AGL
#define XP_ZULU_HOUR "sim/cockpit2/clock_timer/zulu_time_hours" //	int	n	hours	Zulu time, hours
#define XP_ZULU_MIN "sim/cockpit2/clock_timer/zulu_time_minutes" //	int	n	minutes	Zulu time, minutes
#define XP_ZULU_SEC "sim/cockpit2/clock_timer/zulu_time_seconds" //	int	n	seconds	Zulu time, seconds
#define XP_SIM_ZULU_SECS "sim/time/zulu_time_sec" //	float	y	seconds	Zulu time  (seconds since midnight??)
#define XP_KIAS "sim/flightmodel/position/indicated_airspeed" // float	y	kias	Air speed indicated - this takes into account air density and wind direction
#define XP_MSEC "sim/flightmodel/position/groundspeed" //	float	n	meters/sec	The ground speed of the aircraft
#define XP_SOUNDS "sim/weather/speed_sound_ms" //	float	n	meters/sec	This is the speed of sound in meters/second at the plane's location.
#define XP_VSPEED "sim/flightmodel/position/vh_ind_fpm" // float	y	fpm	VVI (vertical velocity in feet per second)
//#define XP_STALL "sim/flightmodel/failures/stallwarning" //	int	y	???	Stall Warning
#define XP_STALL "sim/cockpit2/annunciators/stall_warning" //	int	n	boolean	Stall warning going off, yes or no.
#define XP_OVERSPEED "sim/flightmodel/failures/over_vne" //	int	y	???	???
#define XP_VNE "sim/aircraft/view/acf_Vne" //	float	y
#define XP_LAT "sim/flightmodel/position/latitude" // double	n	degrees	The latitude of the aircraft
#define XP_LON "sim/flightmodel/position/longitude" //	double	n	degrees	The longitude of the aircraft
#define XP_AMSL "sim/flightmodel/position/elevation" //	double	n	meters	The elevation above MSL of the aircraft
#define XP_QNH "sim/cockpit/misc/barometer_setting" //	float	y	???	The pilots altimeter setting
#define XP_PITCH "sim/flightmodel/position/theta" // float	y	degrees	The pitch relative to the plane normal to the Y axis in degrees
#define XP_ROLL "sim/flightmodel/position/phi" //	float	y	degrees	The roll of the aircraft in degrees
#define XP_HDG "sim/flightmodel/position/magpsi" //	float	n	degrees	The magnetic heading of the aircraft
//#define XP_AP "sim/cockpit2/autopilot/autopilot_on" //	int	n	boolean	Is the autopilot really on? Takes into account electrical system, failures, etc.;
#define XP_AP "sim/cockpit/autopilot/autopilot_mode" //	int	y	enum	The autopilot master mode (off=0, flight director=1, on=2)
#define XP_FLAPS "sim/flightmodel/controls/flaprat" //	float	y	[0..1]	Actual flap 1 deployment ratio
#define XP_GEAR "sim/cockpit/switches/gear_handle_status" //	int	y	boolean	Gear handle is up or down?
#define XP_SIMSPEED "sim/time/sim_speed" //	int	y	ratio	This is the multiplier for real-time...1 = realtme, 2 = 2x, 0 = paused, etc.
#define XP_GFORCE "sim/flightmodel/forces/g_nrml" //	float	n	Gs	Total g-forces on the plane as a multiple, downward
//#define XP_FUEL "sim/aircraft/weight/acf_m_fuel_tot" //	float	y	lbs	Weight of total fuel - appears to be in lbs.
#define XP_FUEL "sim/flightmodel/weight/m_fuel_total" //	float	n	kgs	Fuel Total Weight
#define XP_GPS "sim/cockpit2/radios/actuators/gps_power" //	int	y	boolean	GPS off or on, 0 or 1. FIXME: NO SIRVE PORQUE SIEMPRE ESTA A 1
#define XP_PAYLOAD "sim/flightmodel/weight/m_fixed" //	float	y	kgs	Payload Weight
#define XP_LBG "" // Parece no haber Dataref para el peso del combustible
//#define XP_WEIGHT "sim/aircraft/weight/acf_m_empty" //	float	y
#define XP_WEIGHT "sim/flightmodel/weight/m_total" //	float	n	kgs	Total Weight
#define XP_PITOT "sim/cockpit/switches/pitot_heat_on" //	int	y	bool	Is the pitot heat on
//#define XP_OAT "sim/cockpit2/temperature/outside_air_temp_degc" //	float	n	degreesC	outside air temperature, celsius
#define XP_OAT "sim/weather/temperature_ambient_c" //	float	n	degrees C	The air temperature outside the aircraft (at altitude).
#define XP_VIS "sim/weather/visibility_reported_m" //	float	y	meters >= 0	The reported visibility (e.g. what the METAR/weather window says).
#define XP_WKNT "sim/weather/wind_speed_kt" //	float	n	kts >= 0	The effective speed of the wind at the plane's location.
#define XP_WKNT0 "sim/weather/wind_speed_kt[0]" //	float	y	kts >= 0	The wind speed in k0ts.
#define XP_WKNT1 "sim/weather/wind_speed_kt[1]" //	float	y	kts >= 0	The wind speed in k0ts.
#define XP_WKNT2 "sim/weather/wind_speed_kt[2]" //	float	y	kts >= 0	The wind speed in k0ts.
#define XP_WDIR "sim/weather/wind_direction_degt" //	float	n	[0-359)	The effective direction of the wind at the plane's location. (NO ES ASI, ¡TIENE NUMEROS NEGATIVOS!)
#define XP_WDIR0 "sim/weather/wind_direction_degt[0]" //	float	y	[0 - 360)	The direction the wind is blowing from in degrees from true 0rth c lockwise.
#define XP_WDIR1 "sim/weather/wind_direction_degt[1]" //	float	y	[0 - 360)	The direction the wind is blowing from in degrees from true 0rth c lockwise.
#define XP_WDIR2 "sim/weather/wind_direction_degt[2]" //	float	y	[0 - 360)	The direction the wind is blowing from in degrees from true 0rth c lockwise.
#define XP_MAGVAR "sim/flightmodel/position/magnetic_variation" //	float	n	degrees	The local magnetic variation [PROBABLY NEEDS TO GO SOMEWHER ELSE!]
///#define XP_AVIONICS "sim/cockpit2/switches/avionics_power_on" //	int	y	boolean	Switch, 0 or 1.
#define XP_AVIONICS "sim/cockpit/electrical/avionics_on"	// int	y	bool	Is there power to the avionics
#define METROS_A_PIES 3.2808399f
#define FLAPS_TOPE 16383.0f
#define TREN_TOPE 16383.0f
#define KG_A_LB 2.20462262f
#define METRO_A_SM 0.000621371192f

#define CAJA_INTERVALO 1.0f // Intervalo de tiempo con el que se graban los registros
#define CAJA_INICIO_REG 3 // Primer registro que realmente se graba en el fichero (para evitar grabar registros erráticos antes de inicializarse el avión)

class CajaNegra
{

public:

	CajaNegra();
	~CajaNegra();
	
	/* Inicializa archivo de caja negra */
	int Inicio();
	
	/* Graba nuevo registro en caja negra en el directorio especificado por dir*/
	int Graba();
	
	/* Finalizar archivo de caja negra */
	int Fin();
	
	/* Devuelve estado de activación de caja negra */
	bool Activada() { return _activa;}

	/* Activa caja negra */
	void Activar() { _activa = true;}

	/* Desactiva caja negra */
	void Desactivar() { _activa = false;}

	/* ¿Está la caja negra habilitada? */
	bool Enabled() { return _enabled; }

	/* Habilita la caja negra */
	void Enable() { _enabled = true; }

	/* Deshabilita la caja negra */
	void Disable() { _enabled = false; }

	/* Directorio donde se guardará el archivo de caja negra */
	string dir;

	/* Nombre del fichero de la caja negra */
	string nfichero;

	/* Handler del fichero de la caja negra */
	std::ofstream fichero;
	
	/* Nombre de dependencia ATC sintonizada */
	string atc;

	char buffer[CAJA_TAM_REG];

	XPLMDataRef	gXp_AGL,
				gXp_ALT,
				gXp_ZuluH,
				gXp_ZuluM,
				gXp_ZuluS,
				gXp_SimSecs,
				gXp_Kias,
				gXp_MSec,
				gXp_SoundS,
				gXp_Vs,
				gXp_Stall,
				gXp_OverS,
				gXp_Vne,
				gXp_Lat,
				gXp_Lon,
				gXp_Amsl,
				gXp_Qnh,
				gXp_Pitch,
				gXp_Roll,
				gXp_Hdg,
				gXp_Ap,
				gXp_Flaps,
				gXp_Gear,
				gXp_SimSpeed,
				gXp_GForce,
				gXp_Fuel,
				gXp_Gps,
				gXp_Payload,
				gXp_LbsG,
				gXp_Weight,
				gXp_Pitot,
				gXp_Oat,
				gXp_Vis,
				gXp_Wknt,
				gXp_Wknt0,
				gXp_Wknt1,
				gXp_Wknt2,
				gXp_Wdir,
				gXp_Wdir0,
				gXp_Wdir1,
				gXp_Wdir2,
				gXp_Magvar,
				gXp_Avionics;

private:

	bool _activa; // ¿Está la caja negra activada?

	bool _enabled; // ¿Está la caja negra habilitada para funcionar?

	/* Cuenta de registros */
	int _numreg;

	/* Registro de caja negra */
	s_caja _rCaja;

};

#endif

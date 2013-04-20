-- Este script permite la interaccion de las radios COM de XPlane con TeamSpeak 2 para la aerolinea virtual AIRHISPANIA
-- Autor: Tomás García Fresno (LEXJ) AHS232D
-- Colaboradores: Jose Manuel Garcia Valladolid (LEVC) AHS018D

local ffi = require("ffi") 
local XB = require ("XSquawkBox")
local radio = require ("radio")

-- ****************************** Configuracion TeamSpeak tomada desde XSquawkBox (AHS232D)************************************
local server = "95.211.55.201"
local port = "8767"
local nickName = XSquawkBox.XSB_CON_REALNAME
local loginName =XSquawkBox.XSB_CON_PILOT_ID
local password = string.upper(XSquawkBox.XSB_CON_PASSWORD)
-- ****************************************************************************************************************

-- Declaracion de funciones C que pueden ser utilizadas
ffi.cdef [[ 
typedef struct __IO_FILE FILE;
FILE *stdout;
int printf(const char *fmt, ...);
void setbuf(FILE *stream, char *buf);
void *malloc(size_t size);
void *memset(void *s, int c, size_t n);
]]
-- Datarefs
dataref("Avionics_on", "sim/cockpit/electrical/avionics_on", "writable")
dataref("Plane_Latitude", "sim/flightmodel/position/latitude")
dataref("Plane_Longitude", "sim/flightmodel/position/longitude")

-- Constantes
local unicom = 12280
local unicom_dep = "UNICOM"
local modo_debug = 1
local distancia_aeropuerto = 40 -- Distancia entre el avión y el aeropuerto en millas

-- Variables
local TSpeak = ffi.load("Resources\\plugins\\FlyWithLua\\Modules\\TSRemote")
local bufferError = ffi.C.malloc(1024)

local TSAirhispania ="teamspeak://".. server .. ":" .. port .. "?nickname=".. nickName .. "?loginname=" .. loginName .. "?password=" .. password .. "?channel="
local UltimoCanal = unicom
local CanalActual = unicom
local Dependencia = unicom_dep
local listaFrecuencias = {}
local listaFrecCercanas = {}
local conectado = false

-- Funciones auxiliares
function DividirCadena(inputstr, sep)
        if sep == nil then
                sep = "%s"
        end
        t={} ; i=1
        for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
                t[i] = str
                i = i + 1
        end
        return t
end


-- Funciones C ue emplearemos para comunicarnos con TeamSpeak

ffi.cdef("int tsrQuit(void)")
ffi.cdef("int tsrConnect(const char *URL)")
ffi.cdef("int tsrDisconnect()")
ffi.cdef("void tsrGetLastError( void *pchBuffer, int BufferLength )") 
-- ffi.cdef("int tsrSwitchChannelName( const char *ChannelName, const char *ChannelPassword )")
-- ffi.cdef("int tsrGetChannels( void *tsrChannels, int &ChannelRecords );")
-- ffi.cdef("int tsrSwitchChannelID( int ChannelID, const char *ChannelPassword )")
-- ffi.cdef("int tsrSendTextMessageToChannel(int channel_id, const char *message)")
-- ffi.cdef("int tsrSendTextMessage( const char *message )")


-- FUNCIONES PARA INTERACTUAR CON TEAMSPEAK
function ObtenerNombreCanal(frecuencia)
	local canal 
	for i,v in ipairs(listaFrecCercanas) do
      if v == nil then
		break
	  end
	  
	  if string.match(v.frecuencia,frecuencia) then 
	   -- recuperamos el nombre de la dependencia
			canal= v.nombre -- habria que recuperar el resto
			break 
	  end 
    end
	return canal
end

--Conecta con teamSpeak con un canal especificado
-- formato ejemplo: teamspeak://95.211.55.201:8767?nickname=AHS232D-Tomás G Fresno?loginname=AHS232D?password=MiPasword
function Conectar(url, canal)
	if (canal~=nil) then 
		resp = TSpeak.tsrConnect(url .. canal)
		if (resp==0) then
			-- XPLMSpeakString("Conexion con " .. canal .. " establecida con exito.")
			return true
		end
	end
	return false
end
-- providing the function
function Desconectar()
	if (conectado == true) then
		TSpeak.tsrDisconnect()
		conectado = false
	end
end


function Sintonizacion()

	-- Verificamos si la frecuencia a sintonizar es distinta a la sintonizada en este momento
	if (CanalActual ~= radio.COM1) then
		-- Buscamos si la frecuencia a sintonizar existe en la lista de dependencias calculadas
		nombreCanal = ObtenerNombreCanal(radio.COM1)
		if(nombreCanal ~= nil) then
			print ("SintonizadorTS_AHS: sintonizando " .. radio.COM1 .. " en " .. nombreCanal)
			Desconectar() -- Desconectamos antes de conectar con el canal seleccionado
			conectado = Conectar(TSAirhispania, nombreCanal)
			if  (conectado == true) then
				-- Como hemos podido conectar, actualizamos el canal actual
				CanalActual = radio.COM1
				if (modo_debug == 1) then
					print ("SintonizadorTS_AHS: sintonización a " .. CanalActual .. " OK!")
				end
			else
				if (modo_debug == 1) then
					print ("SintonizadorTS_AHS: sintonización a " .. CanalActual .. " NO EFECTUADA!")
				end
			end
		else
			-- Situación en la que estamos cambiando el dial de la radio.
			-- Mientras no seleccionamos una frecuencia de un canal disponible mantenmos el TS desconectado
			Desconectar()	
		end
	end
end

function distancia2Dependencia(datosDependencia)
	local distance = 100000

	local latDep = datosDependencia.lat -- Latitud de la dependencia
	local lonDep = datosDependencia.lon -- Longitud de la dependencia
	
	-- Calculos matematicos
    deg_to_rad = math.pi/180.0
    phi1 = (90.0 - Plane_Latitude)*deg_to_rad
    phi2 = (90.0 - latDep)*deg_to_rad
    theta1 = Plane_Longitude*deg_to_rad
    theta2 = lonDep*deg_to_rad
    cos = (math.sin(phi1)*math.sin(phi2)*math.cos(theta1 - theta2) + math.cos(phi1)*math.cos(phi2))
    arc = math.acos( cos )
    distance = arc*3960 	-- distancia en millas
    -- distance = arc*6373 	-- distancia en kilometros por si fuera necesario
    return distance
end

function FiltradoAeropuertos()
	
	listaFrecCercanas = {}
	for i,v in ipairs(listaFrecuencias) do

		if (distancia2Dependencia(v)<distancia_aeropuerto) then 	-- insertamos las dependencias dentro de un rango de 40 millas
			table.insert(listaFrecCercanas, v) 	-- aqui se podria insertar informacion sobre la distancia para posteriormente ordenarlo.
		end
	end
	-- Insertamos datos de UNICOM pues siempre estara disponible
	local unicom = {frecuencia= unicom ,
							nombre = unicom_dep,
							lat=0,
							lon=0}
	table.insert(listaFrecCercanas, unicom)
		
	if (modo_debug == 1) then
		print "SintonizadorTS_AHS: calculado filtrado de aeropuertos,"
		for i,p in ipairs(listaFrecCercanas) do
			print  (p.nombre .. " " .. p.frecuencia) 
		end
    end
end

function CargarTablaFrecuencias()

	local lista ={}
		
	-- abrimos el fichero para leer los datos que contiene	
	local fichero = assert(io.open("frequencies.csv", "r"))
    for line in fichero:lines() do
		if line == nil then
			break
		end
		local items = DividirCadena(line, ",")
		local depNueva = {frecuencia= items[1] ,
							nombre = items[2],
							lat=items[3],
							lon=items[4]}

		table.insert(lista, depNueva)
		
    end
	fichero:close()
	if (modo_debug == 1) then
        print ("SintonizadorTS_AHS: cargada tabla de frecuencias!")
    end
	return lista	
end

-- Sólo si tenemos activo el script y tenemos la aviónica en marcha se ejecuta la sintonización
function PuestaEnMarcha()
	if (TSpeak_flag == true and Avionics_on) then
		Sintonizacion()
	else
		Desconectar() -- Si desactivamos el flag desconectamos TeamSpeak
	end
end

-- Carga las frecuencias del fichero (AHS018D)
function Inicializar()
	if (table.getn(listaFrecuencias) == 0) then  
		listaFrecuencias = CargarTablaFrecuencias()
		UltimoCanal = unicom
        CanalActual = unicom
		Dependencia = unicom_dep
		if (modo_debug == 1) then
			print "SintonizadorTS_AHS: inicializado!"
		end
	end
end

print "Ejecutando SintonizadorTS_AHS: sintonizador automatico de TeamSpeak para Airhispania ..."
Inicializar() 
do_sometimes("FiltradoAeropuertos()") -- se ejecuta cada 10 segundos
do_often("PuestaEnMarcha()") -- se ejecuta cada segundo			

-- Añadimos la macro y la activamos
-- add_macro("Inicializar TeamSpeak Airhispania", "Incializar()")
add_macro("SintonizadorTS_AHS", "TSpeak_flag = true", "TSpeak_flag = false", "activate")



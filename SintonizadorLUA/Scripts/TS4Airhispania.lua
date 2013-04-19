
-- Este script permite la interaccion de las radios COM de XPlane con TeamSpeak 2 para la aerolinea virtual AIRHISPANIA
-- Autor: Tomás García Fresno (LEXJ) AHS232D
-- ****************************** INTRODUCIR LOS DATOS DE CONEXION A TEAMSPEAK ************************************
local nickName = "AHS232D Tomas Garcia Fresno" 
local loginName ="AHS232D" 
local password ="MiContraseña" 
-- ****************************************************************************************************************
local ffi = require("ffi") 
local XB = require ("XSquawkBox")
local radio = require ("radio")

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

-- Variables
local TSpeak = ffi.load("Resources\\plugins\\FlyWithLua\\Modules\\TSRemote")
local bufferError = ffi.C.malloc(1024)

local TSAirhispania ="teamspeak://95.211.55.201:8767?nickname=".. nickName .. "?loginname=" .. loginName .. "?password=" .. password .. "?channel="
local UltimoCanal = 12280
local CanalActual = 12280
local Dependencia = "UNICOM"
local listaFrecuencias = {}
local listaFrecCercanas = {}

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
function Conectar(url, freq_canal)
	nombreCanal = ObtenerNombreCanal(freq_canal)
	if (nombreCanal~=nil) then 
		resp = TSpeak.tsrConnect(url .. nombreCanal)
		if (resp==0) then
			XPLMSpeakString("Conexion con " .. nombreCanal .. " establecida con exito")
		end
	else 
		Desconectar()
	end
end
-- providing the function
function Desconectar()
	TSpeak.tsrDisconnect()
end


function Sintonizacion()
		-- Memorizamos el canal actual por si lo necesitamos para volver a el
		UltimoCanal = CanalActual
		CanalActual = radio.COM1
	-- Comprobamos si la ultima emisora sintonizada es diferente que tiene la radio
		if (UltimoCanal ~= CanalActual) then
			Desconectar() -- Desconectamos antes de conectar con el canal seleccionado
			Conectar(TSAirhispania, CanalActual)
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

		if (distancia2Dependencia(v)<40) then 	-- insertamos las dependencias dentro de un rango de 40 millas
			table.insert(listaFrecCercanas, v) 	-- aqui se podria insertar informacion sobre la distancia para posteriormente ordenarlo.
		end
	end
	-- Insertamos datos de UNICOM pues siempre estara disponible
	local unicom = {frecuencia= 12280 ,
							nombre = "UNICOM",
							lat=0,
							lon=0}
	table.insert(listaFrecCercanas, unicom)
	for i,p in ipairs(listaFrecCercanas) do
		print (p.nombre .. " " .. p.frecuencia) 
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
	return lista	
end

function PuestaEnMarcha()
	if (table.getn(listaFrecuencias) == 0) then  -- lo hacemos solo la primera vez (INICIALIZACION)
		listaFrecuencias = CargarTablaFrecuencias()
		UltimoCanal = 12280
        CanalActual = 12280
		Dependencia = "UNICOM"
	end
	if (TSpeak_flag == true and Avionics_on) then
		do_sometimes("FiltradoAeropuertos()") -- se ejecuta cada 10 segundos
		Sintonizacion()
	else
		Desconectar() -- Si desactivamos el flag desconectamos TeamSpeak
	end
end


do_often("PuestaEnMarcha()")			

-- Añadimos la macro y la activamos
-- add_macro("Inicializar TeamSpeak Airhispania", "Incializar()")
add_macro("Sintonizacion TeamSpeak Airhispania", "TSpeak_flag = true", "TSpeak_flag = false", "activate")



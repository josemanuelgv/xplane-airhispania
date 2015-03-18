<h2>XPlane para AirHispania</h2>
Este proyecto proporciona tutoriales y herramientas para soportar el uso del simulador XPlane en la red de simulación de vuelo virtual de AirHispania.

<h2>Descargas disponibles</h2>

 * Descarga disponible de la cuarta release de *XivAp_AHS para XPlane 9/10 (32 y 64 bits) para Windows (r149). Binarios para descargar en https://drive.google.com/file/d/0By8c9o_8djGZOTdLYlhxVTFvaGM
 * Ultima versión de *XMakeRwys* (0.3.1) para valorar con AHS Bender vuelos registrados con AHS Box en XPlane. Binarios para descargar en https://drive.google.com/open?id=0By8c9o_8djGZY1ZXV3d1S0l2Ylk&authuser=0

<h2> Tutorial para vuelo online en AirHispania con XPlane 10: XivAp AHS </h2>

<h3>Introducción<h3>

 Este tutorial proporciona una guía para la instalación y puesta en marcha del plugin de XivAP de AirHispania para XPlane 10 (64b). Este plugin es el resultado de la adaptación de la versión Open Source de XivAp ( disponible en http://sourceforge.net/projects/x-ivap/) para el servidor de vuelo online de AirHispania.

<h3>Nuevas funcionalidades<h3>

 La versión de XivAp para AHS tiene básicamente las mismas características que su homólogo para IVAO, pero es importante resaltar algunas funcionalidades específicas para AirHispania:

 * Integración con AhsControl para la sintonización correcta de canales de TeamSpeak para cada dependencia activa.
 * Integración en el propio plugin de la caja negra (AhsBox). Simplemente con activar la aviónica, nuestro plugin genera un archivo de caja negra totalmente compatible con AhsBox y AhsBender. No necesitamos tener AhsBox ejecutándose ni los plugins de los que depende (XPUIPC).
 * Interfaz gráfica del plugin ampliada.
 * Activación del modo CAVOK.
 * Incorporación de paquete de CSL's de las aeronaves de la flota de AirHispania.

<h3>Requerimientos necesarios<h3>

  * XPlane 9 o 10 (32 o 64 bits)
  * TeamSpeak 2 RC2
 
<h3>Instalación<h3>

  <font color="red">Antes de instalar este plugin es importante asegurarnos de que no tenemos instalada la versión original del plugin XivAp para IVAO ya que ambos no pueden coexistir en el mismo simulador. Siempre podemos "mover" el plugin de IVAO a otra carpeta si no queremos borrarlo definitivamente.</font>

  En los siguientes pasos nos referimos a %XPLANE10% como la ubicación física de la carpeta donde tenemos instalado XPlane 10 en nuestro ordenador.

  * Descargamos la última versión del plugin: [https://drive.google.com/file/d/0By8c9o_8djGZOTdLYlhxVTFvaGM XivAp AHS r149]
  * Abrimos el archivo zip de la distribución de XIvap para AHS.
  * Copiamos el contenido de la carpeta "for_plugins_folder" en la carpeta %XPLANE10%/Resources/plugins.
  
  Aunque ya tenemos el plugin listo para ejecutar, es muy importante antes configurar algunos ficheros tal como expone la siguiente sección.

<h3>Configuración<h3>

  Abriremos el archivo %XPLANE10%/Resources/plugins/X-IvAp AHS/X-IvAp AHS Resources/X-IvAp.conf con nuestro editor de texto habitual. En él debemos completar las siguientes variables:
   * *BASE*: Indicaremos el ICAO de nuestro aeródromo base. Ejemplo LEVC.
   * *REALNAME*: Indicaremos nuestro nombre completo.
   * *PASSWORD*: Indicaremos nuestra clave de acceso a AirHispania.
   * *VID*: Indicaremos nuestro indicativo sin el prefijo AHS. Por ejemplo, 018D.
   * *CARPETA*: Indicaremos la ruta completa de la carpeta donde queremos que _XivAp_AHS_ nos almacene los archivos de cajas negras.

<h3>Puesta en marcha<h3>

 Ya podemos ejecutar nuestro simulador. Al iniciarse veremos la ventana principal de XivAp_AHS disponible. 

 En la interfaz podemos ver un indicativo con la etiqueta "BB" que nos indica si la caja negra está grabando o no. Para activarla o desactivarla usaremos el conmutador de aviónica de la aeronave.

 Tras conectarnos a la red, es  *muy importante*, tras activar la aviónica y, en consecuencia, la caja negra, *sintonizar la frecuencia de la dependencia de la cual partimos*. Al hacerlo debemos ser redirigidos al canal de TeamSpeak de dicha dependencia. Con esto nos aseguramos que en la caja negra se anota que partimos de una dependencia activa.

 
<h3>Otros recursos importantes<h3>
  
  Además del plugin, podemos incorporar a nuestro simulador XPlane 10 una serie de recursos que complementan el vuelo online en AirHispania. Estos recursos son:

  <h4> Valoración de vuelos con AHSBender <h4>

 En este enlace https://drive.google.com/open?id=0By8c9o_8djGZY1ZXV3d1S0l2Ylk&authuser=0 está disponible la última versión de XMakeRwys (0.3.1), una versión para XPlane de MakeRwys que permite valorar con AHSBender 9 los vuelos registrados con la caja negra integrada de XivAp_AHS.
 
Requisitos:
 * Sistema operativo Windows
 * Máquina virtual Java JRE mínimo 1.5.0 de 32bits [http://www.oracle.com/technetwork/java/javase/downloads/jre7-downloads-1880261.html Descarga Windows x86]
 * XPlane 9 o 10
 
Uso: 
 * Desempaquetar los archivos en la carpeta de instalación de XPlane.
 * Ejecutar "XMakeRwys.exe". Aparecerá una ventana de comandos en la que se rastrea la base de datos de aeródromos de nuestro simulador y se calcula la posición de las pistas y sus cabeceras.
 * Ejecutar "AhsBender.exe" para valorar vuelos. Este software se puede descargar desde la web de AirHispania. Todas las pruebas se han realizado con la versión para FS9 del mismo. Una vez instalado, hay que copiar el fichero AhsBender.exe al directorio de XPlane y, desde allí, ejecutarlo.

Opcionalmente ejecutar "AptViewer.exe" para inspeccionar y buscar aeropuertos entre todas las bases de datos apt disponibles en nuestro XPlane. Esta herramienta permite realizar búsquedas sobre toda la base de datos de aeropuertos que tenemos instalada.
  
 
  <h4>Aeródromos y elementos del terreno: AirHispania SCX para XPlane 10 <h4>
   
   Hemos realizado una conversión a XPlane 10 (usando la herramienta [http://marginal.org.uk/x-planescenery/tools.html FS2XPlane]) del magnífico proyecto AirHispania SCX, que contiene algunos aeropuertos, aeródromos y otros elementos del terreno que ayudan a la navegación aérea. Este paquete contiene:

   * Aeródromos 2.0
   * Faros 1.4
   * Toros de Osborne 2.0
   * Puntos notificacion VFR 2.0 (Globos)
   * Obstáculos 2.6
   * Cambios AENA 2.1
   * Varios 2.6

  Descargas: https://github.com/josemanuelgv/xplane-airhispania/tree/master/airhispania-xsc/AHS_XSC_Convertidos

  Instrucciones:
   1. Copiar los aeródromos a la carpeta %XPLANE10%/CustomScenery.
   2. Incorporar los paquetes en el fichero _scenery_packs.ini_ en las primeras posiciones.
   3. Ejecutar XMakeRwys para actualizar la base de datos de AhsBender y poder valorar nuestros vuelos.
   4. Desactivar la opción de "pistas siguen los contornos del terreno" (en inglés "runways follow terrain contours") en la configuración de Gráficos (Render options) para evitar ver las aeronaves de los compañeros enterradas en el terreno o levitando.



/**
 * 
 */
package org.airhispania.xplane2rc.transformer;

import java.io.File;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.apt.model.LandRunway;
import org.airhispania.xplane2rc.apt.model.LandRunwayEnd;
import org.airhispania.xplane2rc.apt.parser.AptParser;
import org.airhispania.xplane2rc.rc4.model.Airport;
import org.airhispania.xplane2rc.rc4.model.RunwayEnd;
import org.airhispania.xplane2rc.rc4.parser.RC4Parser;
import org.airhispania.xplane2rc.util.Point;

import com.grum.geocalc.EarthCalc;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         This class implements a transformation between apt.dat XP database
 *         and r4.csv runways format.
 * 
 *         There is one apt.dat default database and a set of apt.dat's being
 *         custom airports the user has installed in XPlane installation There
 *         is also a list of icao codes of airports existing in custom airports
 *         files that trasformer needs to exclude. This feature lets the user to
 *         avoid process old airports database from XPlane. When the transformer
 *         finds a icao to be excluded then uses the one existing in default
 *         apt.dat.
 * 
 * 
 *         The result is a new r4.csv with its coordinates from xplane database
 *         airports, and not from FS2004 BGL's (calculated by MakeRunWays tool)
 * 
 */
public class Apt2rcTransformer {

	public static double feets_per_meter = 3.2808;

	private File defaultAptDb;
	private List<File> customAptDbs;
	private List<String> excludedCustomIcaos;
	private File generatedR4Db;
	private List<String> statistics = new ArrayList<String>();

	private Vector<IApt2rcTransformerListener> listeners = new Vector<IApt2rcTransformerListener>();

	// local db's
	private Map<String, LandAirport> defApt;
	private List<Map<String, LandAirport>> customApts;

	/**
	 * @param defaultAptDb
	 * @param generatedR4Db
	 */
	public Apt2rcTransformer(File defaultAptDb, File generatedR4Db) {
		super();
		this.defaultAptDb = defaultAptDb;
		this.generatedR4Db = generatedR4Db;
	}

	/**
	 * @param defaultAptDb
	 * @param customAptDbs
	 * @param excludedCustomIcaos
	 * @param generatedR4Db
	 */
	public Apt2rcTransformer(File defaultAptDb, List<File> customAptDbs,
			List<String> excludedCustomIcaos, File generatedR4Db) {
		super();
		this.defaultAptDb = defaultAptDb;
		this.customAptDbs = customAptDbs;
		this.excludedCustomIcaos = excludedCustomIcaos;
		this.generatedR4Db = generatedR4Db;
	}

	public void transform() throws Exception {

		sendMessage("Begin transformation. ");

		statistics.clear();

		checkInputs();

		parseInputs();

		Map<String, Airport> result = new HashMap<String, Airport>();
		sendMessage("Calculating result r4.csv database ");

		// Main loop
		int i = 1;

		LandAirport targetLandAirport;

		// Custom apt's
		Set<String> icaos = new HashSet<String>();
		icaos.addAll(defApt.keySet());

		for (Map<String, LandAirport> customApt : customApts) {

			icaos.addAll(customApt.keySet());

			for (String icao : icaos) {

				targetLandAirport = null;

				if (!result.containsKey(icao)) {
					if (this.excludedCustomIcaos.contains(icao)) {
						if (defApt.containsKey(icao))
							targetLandAirport = defApt.get(icao);
					} else {
						if (customApt.containsKey(icao)) {
							targetLandAirport = customApt.get(icao);
							sendMessage("Airport " + icao
									+ " processed from custom Apt ");
						} else if (defApt.containsKey(icao)) {
							targetLandAirport = defApt.get(icao);
							sendMessage("Airport " + icao
									+ " processed from default Apt ");
						}
					}

					if (targetLandAirport != null) {
						try {
							Airport rairport = transformAirport(targetLandAirport);
							if (rairport != null)
								result.put(icao, rairport);

						} catch (Exception e) {
							sendMessage("Error transforming Airport " + icao
									+ ": " + e.getMessage());
						}

						i++;

					}
				}

			}

		}

		statistics.add("Airports calculated = " + (i - 1));

		sendMessage("Storing results on new r4.csv file ... ");
		RC4Parser rc4Parser = new RC4Parser(this.generatedR4Db);
		long ndata = rc4Parser.store(result);
		statistics.add("Runways generated on r4.csv = " + ndata);

	}

	protected void checkInputs() throws Exception {

		sendMessage("Checking inputs ...");

		if (this.defaultAptDb == null)
			throw new Exception("File defaultAptDb can not be null");
		// if (this.baseR4Db == null)
		// throw new Exception("File baseR4Db can not be null");
		if (this.generatedR4Db == null)
			throw new Exception("File generatedR4Db can not be null");

	}

	protected void parseInputs() throws Exception {

		sendMessage("Parsing default Apt.db database ...");
		AptParser defAptParser = new AptParser(this.defaultAptDb);
		defApt = defAptParser.parse();

		statistics.add("Default Apt: parsed " + defApt.size() + " airports");

		customApts = new ArrayList<Map<String, LandAirport>>();
		int customAptsCount = 0;
		if (this.customAptDbs != null) {
			for (File f : this.customAptDbs) {
				sendMessage("Parsing custom Apt.db database "
						+ f.getAbsolutePath());
				AptParser customAptParser = new AptParser(f);
				Map<String, LandAirport> customApt = customAptParser.parse();
				customApts.add(customApt);
				customAptsCount = customAptsCount + customApt.size();
			}
		}

		statistics.add("Custom Apt: parsed " + customAptsCount + " airports");

		if (this.excludedCustomIcaos == null)
			this.excludedCustomIcaos = new ArrayList<String>();

	}

	protected void sendMessage(String s) {
		for (IApt2rcTransformerListener lsn : listeners) {
			lsn.doMessage(s);
		}
	}

	protected Airport transformAirport(LandAirport aptAirport) {

		Airport newAirport = new Airport();
		newAirport.setRunways(new HashMap<String, RunwayEnd>());

		DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(
				Locale.ENGLISH);
		otherSymbols.setDecimalSeparator('.');

		NumberFormat formatter = new DecimalFormat("###.######", otherSymbols);

		// ICAO
		newAirport.setIcao_code(aptAirport.getIcao_code());

		// Runways
		for (LandRunway aptRw : aptAirport.getRunways()) {

			// Calculamos el punto medio entre las dos cabeceras

			Point cab0 = aptRw.getEnds().get(0).getLocation();
			Point cab1 = aptRw.getEnds().get(1).getLocation();

			Point pMedio = this.puntoMedioPista(cab0, cab1);
			double distancia = EarthCalc.getDistance(cab0, cab1);

			for (LandRunwayEnd aptRwe : aptRw.getEnds()) {

				RunwayEnd r4Rw = new RunwayEnd();

				// Convert runway id format
				String r4Number = "0" + aptRwe.getNumber();
				if (r4Number.contains("R")) {
					r4Number = r4Number.replace("R", "2");
				} else if (r4Number.contains("L")) {
					r4Number = r4Number.replace("L", "1");
				} else if (r4Number.contains("C")) {
					r4Number = r4Number.replace("C", "3");
				} else
					r4Number = r4Number + "0";

				if (r4Number.length() == 3)
					r4Number = "0" + r4Number;

				r4Rw.setNumber(r4Number);

				// Locations
				r4Rw.setStartLocation(aptRwe.getLocation());
				r4Rw.setEndLocation(pMedio);

				// Elevation (ft)
				r4Rw.setElevation(aptAirport.getElevation());

				// Bearing
				double bearing = 0;
				if (aptRwe.getLocation().equals(cab0))
					bearing = 360 - EarthCalc.getBearing(cab0, cab1);
				else
					bearing = 360 - EarthCalc.getBearing(cab1, cab0);
				r4Rw.setBearing(formatter.format(bearing));

				// Distance (ft)
				Double d = distancia * feets_per_meter;
				r4Rw.setLength(Integer.toString(d.intValue()));

				r4Rw.setIlsloc("0");
				r4Rw.setMagVar("0");
				r4Rw.setWidth(formatter.format(aptRw.getWidth()
						* feets_per_meter));
				r4Rw.setThresholdOffset(formatter.format(aptRwe
						.getDisplacedThreshold() * feets_per_meter));

				newAirport.getRunways().put(r4Number, r4Rw);
			}

		}

		return newAirport;

	}

	public void registerListener(IApt2rcTransformerListener l) {
		if (!listeners.contains(l)) {
			listeners.add(l);
		}
	}

	public void unRegisterListener(IApt2rcTransformerListener l) {
		if (listeners.contains(l)) {
			listeners.remove(l);
		}
	}

	/**
	 * @return the statistics
	 */
	public List<String> getStatistics() {
		return statistics;
	}

	/**
	 * Calcula el punto medio de una pista dadas las coordenadas de sus
	 * cabeceras
	 * 
	 * @param cab0
	 * @param cab1
	 * @return
	 */
	protected Point puntoMedioPista(Point cab0, Point cab1) {

		// Calculamos el ángulo entre los dos puntos y su distancia
		// para luego aplicar el desplazamiento adecuado a
		// rwR4.endLocation
		double bearing = EarthCalc.getBearing(cab0, cab1); // in
															// decimal
															// degrees
		double distance = EarthCalc.getDistance(cab0, cab1); // in
																// meters

		// Desplazamos el punto de una de las cabeceras a la mitad de distancia
		com.grum.geocalc.Point endLocation = (com.grum.geocalc.Point) EarthCalc
				.pointRadialDistance(cab1, bearing, distance / 2);

		return new Point(endLocation.getLatitude(), endLocation.getLongitude());
	}

}

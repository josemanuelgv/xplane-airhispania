/**
 * 
 */
package org.airhispania.xplane2rc.transformer;

import java.io.File;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.Vector;

import org.airhispania.xplane2rc.apt.model.LandAirport;
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
 *         This class generates a new r4.csv having the airports existing in the
 *         internal r4.csv (located in resources folder) with its coordinates
 *         recalculated from the multiple apt.dat files given to the
 *         transformer.
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

	private File defaultAptDb;
	private List<File> customAptDbs;
	private List<String> excludedCustomIcaos;
	private File baseR4Db;
	private InputStream baseR4DbStream;
	private File generatedR4Db;
	private List<String> statistics = new ArrayList<String>();

	private Vector<IApt2rcTransformerListener> listeners = new Vector<IApt2rcTransformerListener>();

	// local db's
	private Map<String, LandAirport> defApt;
	private Map<String, Airport> baseR4;
	private List<Map<String, LandAirport>> customApts;

	/**
	 * @param defaultAptDb
	 * @param baseR4Db
	 * @param generatedR4Db
	 */
	public Apt2rcTransformer(File defaultAptDb, File baseR4Db,
			File generatedR4Db) {
		super();
		this.defaultAptDb = defaultAptDb;
		this.baseR4Db = baseR4Db;
		this.generatedR4Db = generatedR4Db;
	}

	/**
	 * @param defaultAptDb
	 * @param customAptDbs
	 * @param excludedCustomIcaos
	 * @param baseR4Db
	 * @param generatedR4Db
	 */
	public Apt2rcTransformer(File defaultAptDb, List<File> customAptDbs,
			List<String> excludedCustomIcaos, File baseR4Db, File generatedR4Db) {
		super();
		this.defaultAptDb = defaultAptDb;
		this.customAptDbs = customAptDbs;
		this.excludedCustomIcaos = excludedCustomIcaos;
		this.baseR4Db = baseR4Db;
		this.generatedR4Db = generatedR4Db;
	}

	/**
	 * @param defaultAptDb
	 * @param customAptDbs
	 * @param excludedCustomIcaos
	 * @param baseR4Db
	 * @param generatedR4Db
	 */
	public Apt2rcTransformer(File defaultAptDb, List<File> customAptDbs,
			List<String> excludedCustomIcaos, InputStream baseR4Db,
			File generatedR4Db) {
		super();
		this.defaultAptDb = defaultAptDb;
		this.customAptDbs = customAptDbs;
		this.excludedCustomIcaos = excludedCustomIcaos;
		this.baseR4DbStream = baseR4Db;
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
		int notFound = 0;

		LandAirport targetLandAirport;
		for (String customIcao : baseR4.keySet()) {

			targetLandAirport = null;
			if (this.excludedCustomIcaos.contains(customIcao)) {
				if (defApt.containsKey(customIcao))
					targetLandAirport = defApt.get(customIcao);
			} else {
				for (Map<String, LandAirport> customApt : customApts) {
					if (customApt.containsKey(customIcao)) {
						targetLandAirport = customApt.get(customIcao);
						sendMessage("Airport " + customIcao
								+ " processed from custom Apt ");
					}

				}
				if (targetLandAirport == null) {
					if (defApt.containsKey(customIcao))
						targetLandAirport = defApt.get(customIcao);
				}
			}

			if (targetLandAirport != null) {
				Airport rairport = transformAirport(targetLandAirport,
						baseR4.get(customIcao));
				if (rairport != null)
					result.put(customIcao, rairport);

				// else
				// sendMessage("Airport "
				// + customIcao
				// +
				// " definitions not similar between apt.dat and base r4.csv");
			} else {
				sendMessage("Airport " + customIcao
						+ " has not been found on apt.dat files");
				notFound++;
			}

			i++;
		}

		statistics.add("Airports calculated = " + (i - 1));
		statistics.add("Airports not found = " + notFound);
		// statistics.add("Airports not similar = " + notSim);

		sendMessage("Storing results on new r4.csv file ... ");
		RC4Parser rc4Parser = new RC4Parser(this.generatedR4Db);
		long ndata = rc4Parser.store(result);
		statistics.add("Runways generated on r4.csv = " + ndata);

	}

	public void diff(String icaoFilter) throws Exception {

		sendMessage("Begin diff calculation ");

		if ((icaoFilter == null) || (icaoFilter.length() == 0)
				|| (icaoFilter.equalsIgnoreCase("*"))) {
			icaoFilter = ".*";
		}

		checkInputs();

		parseInputs();

		for (String customIcao : baseR4.keySet()) {

			if (!customIcao.matches(icaoFilter))
				continue;

			LandAirport targetLandAirport = null;
			if (this.excludedCustomIcaos.contains(customIcao)) {
				if (defApt.containsKey(customIcao))
					targetLandAirport = defApt.get(customIcao);
			} else {
				for (Map<String, LandAirport> customApt : customApts) {
					if (customApt.containsKey(customIcao))
						targetLandAirport = customApt.get(customIcao);
				}
				if (targetLandAirport == null) {
					if (defApt.containsKey(customIcao))
						targetLandAirport = defApt.get(customIcao);
				}
			}

			if (targetLandAirport != null) {

				Airport r4airport = baseR4.get(customIcao);
				for (String customRwy : r4airport.getRunways().keySet()) {

					RunwayEnd rwR4 = r4airport.getRunways().get(customRwy);

					LandRunwayEnd rwApt = targetLandAirport
							.getLandRunwayEndByNumber(rwR4.getNumberAsApt());

					if (rwApt != null) {
						double distance = EarthCalc.getDistance(
								rwR4.getStartLocation(), rwApt.getLocation());
						sendMessage("Airport " + customIcao + " Runway "
								+ rwApt.getNumber() + " distance = " + distance
								+ " meters");
					}
				}

			}

		}

		sendMessage("End diff calculation ");
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

		sendMessage("Parsing base r4ref.csv database ...");
		RC4Parser rc4Parser;
		if (this.baseR4Db != null)
			rc4Parser = new RC4Parser(this.baseR4Db);
		else
			rc4Parser = new RC4Parser(this.baseR4DbStream);
		baseR4 = rc4Parser.parse();
		baseR4 = new TreeMap<String, Airport>(baseR4);

		statistics.add("Reference R4: parsed " + baseR4.size() + " airports");

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

	protected Airport transformAirport(LandAirport aptAirport, Airport r4Airport) {

		Airport newAirport = null;

		// First, verify the airport definitions are similar
		// if (aptAirport.getRunwayEndCount() == r4Airport.getRunways().size())
		// {

		newAirport = r4Airport.clone();

		for (String customRwy : r4Airport.getRunways().keySet()) {

			RunwayEnd rwR4 = newAirport.getRunways().get(customRwy);
			LandRunwayEnd rwApt = aptAirport.getLandRunwayEndByNumber(rwR4
					.getNumberAsApt());

			if (rwApt != null) {

				// Calculamos el ángulo entre los dos puntos y su distancia
				// para luego aplicar el desplazamiento adecuado a
				// rwR4.endLocation
				double bearing = EarthCalc.getBearing(rwR4.getStartLocation(),
						rwApt.getLocation()); // in
												// decimal
												// degrees
				double distance = EarthCalc.getDistance(
						rwR4.getStartLocation(), rwApt.getLocation()); // in
																		// meters

				rwR4.setStartLocation(rwApt.getLocation());

				// Aplicamos el desplazamiento a rwR4.endLocation
				com.grum.geocalc.Point endLocation = (com.grum.geocalc.Point) EarthCalc
						.pointRadialDistance(rwR4.getEndLocation(), bearing,
								distance);

				rwR4.setEndLocation(new Point(endLocation.getLatitude(),
						endLocation.getLongitude()));

			} else
				sendMessage("Runway End " + rwR4.getNumber() + " of airport "
						+ r4Airport.getIcao_code() + " not found on APT");

		}
		// } else
		// sendMessage("Airport " + aptAirport.getIcao_code()
		// + " definition differs");

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

}

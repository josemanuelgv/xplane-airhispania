/**
 * 
 */
package org.airhispania.xplane2rc.apt.parser;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.apt.model.LandRunway;
import org.airhispania.xplane2rc.apt.model.LandRunwayEnd;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         Parser for apt.dat files Generates a graph of apt.model objects for
 *         the given file
 */
public class AptParser {

	public static String APT_XP10_HEADER = "1000 Version";
	public static String APT_XP09_HEADER = "850 Version";

	private File fileToParser;

	private Vector<IAptParserListener> listeners = new Vector<IAptParserListener>();

	/**
	 * Local variables used by the parser
	 */
	private LandAirport currentAirport = null;

	/**
	 * Container for parsed airports indexed by icao code
	 */
	private Map<String, LandAirport> parsedAirports = new HashMap<String, LandAirport>();

	/**
	 * @param fileToParser
	 */
	public AptParser(File fileToParser) {
		super();
		this.fileToParser = fileToParser;
	}

	public void registerListener(IAptParserListener l) {
		if (!listeners.contains(l)) {
			listeners.add(l);
		}
	}

	public void unRegisterListener(IAptParserListener l) {
		if (listeners.contains(l)) {
			listeners.remove(l);
		}
	}

	public Map<String, LandAirport> parse() throws Exception {

		if (fileToParser == null)
			throw new Exception("Input file can not be null");

		if (!fileToParser.exists())
			throw new Exception("Input file does not exists");

		parsedAirports.clear();

		FileInputStream fstream = new FileInputStream(fileToParser);

		// Get the object of DataInputStream
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		String strLine;
		// Read File Line By Line
		while ((strLine = br.readLine()) != null) {
			parseLine(strLine);
		}
		// Close the input stream
		in.close();

		return parsedAirports;
	}

	private void parseLine(String l) throws Exception {

		if (l == null)
			return;

		// Check for headers lines
		if (l.startsWith(APT_XP10_HEADER) || l.startsWith(APT_XP09_HEADER))
			return;

		try {

			// Check for LandAirport line
			if ((l.length() > 2)
					&& (LandAirport.APT_CODE == Integer.parseInt(l.substring(0,
							3).trim()))) {
				parseLandAirportLine(l);
			}

			// Check for LandRunway line
			if ((l.length() > 3)
					&& (LandRunway.APT_CODE == Integer.parseInt(l.substring(0,
							3).trim())) && l.substring(0, 4).contains(" ")) {
				parseLandRunwayLine(l);
			}

		} catch (Exception e) {
			throw new Exception(e.getMessage() + " error parsing line \"" + l
					+ "\"");
		}

	}

	private void parseLandAirportLine(String l) throws Exception {

		String elevation;
		String icao;
		String name = "";

		String[] initdata = l.split(" ");
		List<String> data = new ArrayList<String>();

		for (int i = 0; i < initdata.length; i++) {
			if (initdata[i].length() != 0)
				data.add(initdata[i]);
		}

		// Parse elevation
		elevation = data.get(1).trim();

		// Parse icao
		icao = data.get(4).trim();

		// Parse name
		for (int i = 5; i < data.size(); i++) {
			name = name + data.get(i) + " ";
		}

		currentAirport = new LandAirport();
		currentAirport.setElevation(elevation);
		currentAirport.setIcao_code(icao);
		currentAirport.setName(name);
		currentAirport.setRunways(new ArrayList<LandRunway>());

		parsedAirports.put(this.currentAirport.getIcao_code(),
				this.currentAirport);

		for (IAptParserListener lsn : listeners) {
			lsn.landAirportParsed();
		}
	}

	private void parseLandRunwayLine(String l) throws Exception {

		Float width;
		char surface;
		LandRunway rwy = null;

		String[] initdata = l.split(" ");
		List<String> data = new ArrayList<String>();

		for (int i = 0; i < initdata.length; i++) {
			if (initdata[i].length() != 0)
				data.add(initdata[i]);
		}

		// Parse width (1)
		width = Float.parseFloat(data.get(1));

		// Parse surface (2)
		surface = data.get(2).trim().charAt(0);

		rwy = new LandRunway();
		rwy.setSurface_type(surface);
		rwy.setWidth(width);
		rwy.setEnds(new ArrayList<LandRunwayEnd>());

		// Parse runway ends

		if (data.size() >= 17) {
			// First runway number
			String fr_endNumber = data.get(8).trim();

			// First runway latitude
			Double fr_latitude = Double.parseDouble(data.get(9).trim());

			// First runway longitude
			Double fr_longitude = Double.parseDouble(data.get(10).trim());

			LandRunwayEnd lre = new LandRunwayEnd();
			lre.setNumber(fr_endNumber);
			lre.setLatitude(fr_latitude);
			lre.setLongitude(fr_longitude);

			for (IAptParserListener lsn : listeners) {
				lsn.runwayEndParsed();
			}

			rwy.getEnds().add(lre);
		}

		if (data.size() == 26) {
			// Second runway number
			String sc_endNumber = data.get(17).trim();

			// Second runway latitude
			Double sc_latitude = Double.parseDouble(data.get(18).trim());

			// Second runway longitude
			Double sc_longitude = Double.parseDouble(data.get(19).trim());

			LandRunwayEnd lre = new LandRunwayEnd();
			lre.setNumber(sc_endNumber);
			lre.setLatitude(sc_latitude);
			lre.setLongitude(sc_longitude);

			for (IAptParserListener lsn : listeners) {
				lsn.runwayEndParsed();
			}

			rwy.getEnds().add(lre);
		}

		currentAirport.getRunways().add(rwy);

		for (IAptParserListener lsn : listeners) {
			lsn.runwayParsed();
		}

	}

	public static void main(String[] args) throws Exception {

		if (args.length < 1)
			throw new Exception(
					"apt.dat file location is needed as an argument");

		// Testing code
		String aptFileLocation = args[0];

		AptParser parser = new AptParser(new File(aptFileLocation));
		parser.registerListener(new IAptParserListener() {

			public void landAirportParsed() {
				System.out.print("|");
			}

			public void runwayParsed() {

			}

			public void runwayEndParsed() {
				System.out.print(".");
			}

		});

		Map<String, LandAirport> l = parser.parse();

		for (String a : l.keySet()) {
			System.out.println(l.get(a).toString());
		}

		System.out.println("");
		System.out.println("Airports parsed: " + l.size());
		System.out.println("");

	}
}

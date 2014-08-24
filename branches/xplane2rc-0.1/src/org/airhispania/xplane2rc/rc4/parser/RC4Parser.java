/**
 * 
 */
package org.airhispania.xplane2rc.rc4.parser;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

import org.airhispania.xplane2rc.rc4.model.Airport;
import org.airhispania.xplane2rc.rc4.model.RunwayEnd;
import org.airhispania.xplane2rc.util.Point;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         Parser for Radar Contact r4.csv files
 */
public class RC4Parser {

	private File fileToParser;
	private InputStream stream;

	/**
	 * Container for parsed airports indexed by icao code
	 */
	private Map<String, Airport> parsedAirports = new HashMap<String, Airport>();

	private Airport currentAirport = null;

	/**
	 * @param fileToParser
	 */
	public RC4Parser(File fileToParser) {
		super();
		this.fileToParser = fileToParser;
	}

	public RC4Parser(InputStream stream) {
		super();
		this.stream = stream;
	}

	public Map<String, Airport> parse() throws Exception {

		if (this.stream == null) {
			if (fileToParser == null)
				throw new Exception("Input file can not be null");

			if (!fileToParser.exists())
				throw new Exception("Input file does not exists");

			parsedAirports.clear();

			this.stream = new FileInputStream(fileToParser);
		}

		// Get the object of DataInputStream
		DataInputStream in = new DataInputStream(stream);
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

		String[] cols = l.split(",");

		String icao = cols[0];

		if (this.currentAirport != null) {
			if (!this.currentAirport.getIcao_code().equalsIgnoreCase(icao)) {
				parsedAirports.put(this.currentAirport.getIcao_code(),
						this.currentAirport);
				this.currentAirport = new Airport();
				this.currentAirport.setIcao_code(icao);
			}
		} else {
			this.currentAirport = new Airport();
			this.currentAirport.setIcao_code(icao);
		}

		RunwayEnd r = new RunwayEnd();
		r.setNumber(cols[1]);
		r.setStartLocation(new Point(cols[2], cols[3]));
		r.setElevation(cols[4]);
		r.setBearing(cols[5]);
		r.setLength(cols[6]);
		r.setIlsloc(cols[7]);
		r.setWidth(cols[8]);
		r.setUnk0(cols[9]);
		r.setEndLocation(new Point(cols[10], cols[11]));
		r.setUnk1(cols[12]);

		this.currentAirport.getRunways().put(cols[1], r);
	}

	public long store(Map<String, Airport> data) throws IOException {

		long stored = 0;

		DecimalFormatSymbols otherSymbols = new DecimalFormatSymbols(
				Locale.ENGLISH);
		otherSymbols.setDecimalSeparator('.');

		NumberFormat formatter = new DecimalFormat("###.######", otherSymbols);

		List<String> content = new ArrayList<String>();
		for (String icao : data.keySet()) {
			for (String r : data.get(icao).getRunways().keySet()) {
				RunwayEnd rwy = data.get(icao).getRunways().get(r);
				String line = icao + ",";
				line = line + rwy.getNumber() + ",";
				line = line
						+ formatter
								.format(rwy.getStartLocation().getLatitude())
						+ ",";
				line = line
						+ formatter.format(rwy.getStartLocation()
								.getLongitude()) + ",";
				line = line + rwy.getElevation() + ",";
				line = line + rwy.getBearing() + ",";
				line = line + rwy.getLength() + ",";
				line = line + rwy.getIlsloc() + ",";
				line = line + rwy.getWidth() + ",";
				line = line + rwy.getUnk0() + ",";
				line = line
						+ formatter.format(rwy.getEndLocation().getLatitude())
						+ ",";
				line = line
						+ formatter.format(rwy.getEndLocation().getLongitude())
						+ ",";
				line = line + rwy.getUnk1();
				content.add(line);

			}
		}

		BufferedWriter out = new BufferedWriter(new FileWriter(
				this.fileToParser));
		for (String l : content) {
			out.write(l + "\n");
			stored++;
		}
		out.close();
		return stored;
	}

	public static void main(String[] args) throws Exception {

		if (args.length < 1)
			throw new Exception("r4.csv file location is needed as an argument");

		// Testing code
		String r4FileLocation = args[0];

		RC4Parser parser = new RC4Parser(new File(r4FileLocation));

		Map<String, Airport> l = parser.parse();

		for (String a : l.keySet()) {
			System.out.print(l.get(a).toString());
		}

		System.out.println("");
		System.out.println("Airports parsed: " + l.size());
		System.out.println("");

	}
}

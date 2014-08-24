/**
 * 
 */
package org.airhispania.xplane2rc.model;

import java.io.File;
import java.util.Map;
import java.util.TreeMap;

import org.airhispania.xplane2rc.apt.model.LandAirport;
import org.airhispania.xplane2rc.apt.parser.AptParser;
import org.airhispania.xplane2rc.apt.parser.IAptParserListener;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Scenery {

	public static String APT_FILE_PATTERN = ".*\\\\Earth\\snav\\sdata\\\\.*apt.*\\.dat$";
	public static String DEFAULT_APT_FILE_PATTERN = ".*Resources\\\\default\\sscenery\\\\default\\sapt\\sdat\\\\Earth\\snav\\sdata\\\\apt\\.dat$";

	private String title;
	private File aptfile;
	private Map<String, LandAirport> airports;

	long airportsParsed = 0;

	/**
	 * @param title
	 * @param aptfile
	 */
	public Scenery(String title, File aptfile) {
		super();
		this.title = title;
		this.aptfile = aptfile;
	}

	/**
	 * @return the title
	 */
	public String getTitle() {
		return title;
	}

	/**
	 * @param title
	 *            the title to set
	 */
	public void setTitle(String title) {
		this.title = title;
	}

	/**
	 * @return the aptfile
	 */
	public File getAptfile() {
		return aptfile;
	}

	/**
	 * @param aptfile
	 *            the aptfile to set
	 */
	public void setAptfile(File aptfile) {
		this.aptfile = aptfile;
	}

	/**
	 * @return the airports
	 */
	public Map<String, LandAirport> getAirports() {
		return airports;
	}

	/**
	 * @param airports
	 *            the airports to set
	 */
	public void setAirports(Map<String, LandAirport> airports) {
		this.airports = airports;
	}

	public long parse() throws Exception {

		AptParser parser = new AptParser(this.aptfile);

		parser.registerListener(new IAptParserListener() {

			public void landAirportParsed() {
				airportsParsed++;

			}

			public void runwayParsed() {
				// TODO Auto-generated method stub

			}

			public void runwayEndParsed() {
				// TODO Auto-generated method stub

			}

		});
		Map<String, LandAirport> airports = parser.parse();
		this.setAirports(new TreeMap<String, LandAirport>(airports));

		return airportsParsed;
	}

	public boolean isDefaultXplaneDatabase() {
		return aptfile.getAbsolutePath().matches(DEFAULT_APT_FILE_PATTERN);
	}
}

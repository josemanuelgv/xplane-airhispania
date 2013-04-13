/**
 * 
 */
package org.airhispania.xplane2rc.apt.model;

import java.util.List;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         AIRPORT DATA (APT.DAT) FILE SPECIFICATION VERSION 1000
 */
public class LandAirport {

	public static int APT_CODE = 1;

	/**
	 * Elevation of airport in feet above mean sea level (AMSL)
	 */
	private String elevation;

	/**
	 * Airport ICAO code. If no ICAO code exists, use FAA code (USA only)
	 * Maximum four characters. Must be unique.
	 */
	private String icao_code;

	/**
	 * Airport name. May contain spaces. Do not use special (accented)
	 * characters Text string (up to 40 characters)
	 */
	private String name;

	private List<LandRunway> runways;

	/**
	 * @return the elevation
	 */
	public String getElevation() {
		return elevation;
	}

	/**
	 * @param elevation
	 *            the elevation to set
	 */
	public void setElevation(String elevation) {
		this.elevation = elevation;
	}

	/**
	 * @return the icao_code
	 */
	public String getIcao_code() {
		return icao_code;
	}

	/**
	 * @param icao_code
	 *            the icao_code to set
	 */
	public void setIcao_code(String icao_code) {
		this.icao_code = icao_code;
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * @param name
	 *            the name to set
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * @return the runways
	 */
	public List<LandRunway> getRunways() {
		return runways;
	}

	/**
	 * @param runways
	 *            the runways to set
	 */
	public void setRunways(List<LandRunway> runways) {
		this.runways = runways;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		String r = "AIRPORT ";
		r = r + this.icao_code + " " + this.name + " " + this.elevation
				+ " fts = ";
		for (LandRunway e : this.getRunways()) {
			r = r + e.toString();
		}
		return r;
	}

	public LandRunwayEnd getLandRunwayEndByNumber(String number) {
		LandRunwayEnd res = null;
		for (LandRunway r : this.getRunways()) {
			for (LandRunwayEnd re : r.getEnds()) {
				if (re.getNumber().equalsIgnoreCase(number))
					res = re;
			}
		}
		return res;
	}

	public int getRunwayEndCount() {
		int c = 0;
		LandRunwayEnd res = null;
		for (LandRunway r : this.getRunways()) {
			c = c + r.getEnds().size();
		}
		return c;
	}
}

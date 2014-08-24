/**
 * 
 */
package org.airhispania.xplane2rc.rc4.model;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         Radar Contact Airport properties
 */
public class Airport {

	private String icao_code;

	private Map<String, RunwayEnd> runways = new HashMap<String, RunwayEnd>();

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
	 * @return the runways
	 */
	public Map<String, RunwayEnd> getRunways() {
		return runways;
	}

	/**
	 * @param runways
	 *            the runways to set
	 */
	public void setRunways(Map<String, RunwayEnd> runways) {
		this.runways = runways;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		String r = "";
		for (String s : this.getRunways().keySet()) {
			r = this.icao_code + "," + this.getRunways().get(s).toString()
					+ "\n";
		}

		return r;
	}

	public Airport clone() {
		Airport r = new Airport();
		r.setIcao_code(this.getIcao_code());
		for (String re : this.getRunways().keySet()) {
			r.getRunways().put(re, this.getRunways().get(re).clone());
		}
		return r;
	}

}

/**
 * 
 */
package org.airhispania.xplane2rc.apt.model;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class ComFrequency {

	public static int[] APT_CODES = { 53, 54, 55 };
	public static Map<Integer, String> APT_CODES_DEP_SUFFIX;
	static {
		APT_CODES_DEP_SUFFIX = new HashMap<Integer, String>();
		APT_CODES_DEP_SUFFIX.put(53, "GND");
		APT_CODES_DEP_SUFFIX.put(54, "TWR");
		APT_CODES_DEP_SUFFIX.put(55, "APP");
	}

	private int code;
	private String frequency;
	private String dependency;

	/**
	 * @return the code
	 */
	public int getCode() {
		return code;
	}

	/**
	 * @param code
	 *            the code to set
	 */
	public void setCode(int code) {
		this.code = code;
	}

	/**
	 * @return the frequency
	 */
	public String getFrequency() {
		return frequency;
	}

	/**
	 * @param frequency
	 *            the frequency to set
	 */
	public void setFrequency(String frequency) {
		this.frequency = frequency;
	}

	/**
	 * @return the dependency
	 */
	public String getDependency() {
		return dependency;
	}

	/**
	 * @param dependency
	 *            the dependency to set
	 */
	public void setDependency(String dependency) {
		this.dependency = dependency;
	}

	public static boolean validFreqCode(int s) {
		return (s == APT_CODES[0]) || (s == APT_CODES[1])
				|| (s == APT_CODES[2]);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return frequency + "_" + dependency;
	}

}

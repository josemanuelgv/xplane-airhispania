/**
 * 
 */
package org.airhispania.fscom2ts.configuration;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Configuration {

	private int updateFrequency = 3000; // Default 3 seconds
	private String frequencyFiles = "";
	private int maxrad_nm = 40; // Radio en el cual debe estar la dependencia
								// para conectar
	private String unicomFreq; // Sintonizando esta frecuencia, se conecta
								// directamente al TS sin procesar ninguna
								// dependencia
	private String unicomChannel;

	/**
	 * @return the updateFrequency
	 */
	public int getUpdateFrequency() {
		return updateFrequency;
	}

	/**
	 * @param updateFrequency
	 *            the updateFrequency to set
	 */
	public void setUpdateFrequency(int updateFrequency) {
		this.updateFrequency = updateFrequency;
	}

	/**
	 * @return the frequencyFiles
	 */
	public String getFrequencyFiles() {
		return frequencyFiles;
	}

	/**
	 * @param frequencyFiles
	 *            the frequencyFiles to set
	 */
	public void setFrequencyFiles(String frequencyFiles) {
		this.frequencyFiles = frequencyFiles;
	}

	/**
	 * @return the maxrad_nm
	 */
	public int getMaxrad_nm() {
		return maxrad_nm;
	}

	/**
	 * @param maxrad_nm
	 *            the maxrad_nm to set
	 */
	public void setMaxrad_nm(int maxrad_nm) {
		this.maxrad_nm = maxrad_nm;
	}

	/**
	 * @return the unicomFreq
	 */
	public String getUnicomFreq() {
		return unicomFreq;
	}

	/**
	 * @param unicomFreq
	 *            the unicomFreq to set
	 */
	public void setUnicomFreq(String unicomFreq) {
		this.unicomFreq = unicomFreq;
	}

	/**
	 * @return the unicomChannel
	 */
	public String getUnicomChannel() {
		return unicomChannel;
	}

	/**
	 * @param unicomChannel
	 *            the unicomChannel to set
	 */
	public void setUnicomChannel(String unicomChannel) {
		this.unicomChannel = unicomChannel;
	}

	public static Configuration parse(File f) throws FileNotFoundException,
			IOException {
		Properties prop = new Properties();
		prop.load(new FileInputStream(f));
		Configuration cfg = new Configuration();

		cfg.setUpdateFrequency(Integer.parseInt(prop
				.getProperty("updateFrequency")));
		cfg.setFrequencyFiles(prop.getProperty("frequencyFiles"));
		cfg.setMaxrad_nm(Integer.parseInt(prop.getProperty("maxrad_nm")));
		cfg.setUnicomFreq(prop.getProperty("unicom_freq"));
		cfg.setUnicomChannel(prop.getProperty("unicom_channel"));
		return cfg;

	}
}

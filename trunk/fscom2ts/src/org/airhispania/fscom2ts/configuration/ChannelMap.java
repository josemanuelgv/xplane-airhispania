/**
 * 
 */
package org.airhispania.fscom2ts.configuration;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 *
 */
/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class ChannelMap {

	private TSConnection tsConnection;
	// Nombre de la dependencia
	private String dependency;
	// Frecuencia en formato ###.##
	private String frequency;
	private String channel;
	private Double latitude;
	private Double longitude;

	/**
	 * @return the tsConnection
	 */
	public TSConnection getTsConnection() {
		return tsConnection;
	}

	/**
	 * @param tsConnection
	 *            the tsConnection to set
	 */
	public void setTsConnection(TSConnection tsConnection) {
		this.tsConnection = tsConnection;
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
	 * @return the channel
	 */
	public String getChannel() {
		return channel;
	}

	/**
	 * @param channel
	 *            the channel to set
	 */
	public void setChannel(String channel) {
		this.channel = channel;
	}

	/**
	 * @return the latitude
	 */
	public Double getLatitude() {
		return latitude;
	}

	/**
	 * @param latitude
	 *            the latitude to set
	 */
	public void setLatitude(Double latitude) {
		this.latitude = latitude;
	}

	/**
	 * @return the longitude
	 */
	public Double getLongitude() {
		return longitude;
	}

	/**
	 * @param longitude
	 *            the longitude to set
	 */
	public void setLongitude(Double longitude) {
		this.longitude = longitude;
	}

	/**
	 * Calcula la distancia en millas desde el punto dado a la ubicación de la
	 * dependencia
	 * 
	 * @param lat
	 * @param lon
	 * @return
	 */
	public Double distanciaMillas(Double lat, Double lon) {
		Double r = 0.0;

		double deg_to_rad = Math.PI / 180;
		double phi1 = (90.0 - lat) * deg_to_rad;
		double phi2 = (90.0 - this.latitude) * deg_to_rad;
		double theta1 = lon * deg_to_rad;
		double theta2 = this.longitude * deg_to_rad;
		double cos = (Math.sin(phi1) * Math.sin(phi2)
				* Math.cos(theta1 - theta2) + Math.cos(phi1) * Math.cos(phi2));
		double arc = Math.acos(cos);
		r = new Double(arc * 3960);

		return r;
	}

	public static List<ChannelMap> parse(File f, TSConnection tsconn)
			throws Exception {

		List<ChannelMap> l = new ArrayList<ChannelMap>();

		FileInputStream fstream = new FileInputStream(f);

		// Get the object of DataInputStream
		DataInputStream in = new DataInputStream(fstream);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		String strLine;
		// Read File Line By Line
		while ((strLine = br.readLine()) != null) {
			String[] data = strLine.split(",");
			ChannelMap cm = new ChannelMap();
			// Soportamos solo una conexión

			cm.setTsConnection(tsconn);

			// if ((data.length > 0) && (data[0] != null)
			// && (data[0].length() > 0)) {
			// int tscon = Integer.parseInt(data[0]);
			// if (tsconns.get(tscon) != null) {
			// cm.setTsConnection(tsconns.get(tscon));
			// } else {
			// in.close();
			// throw new Exception(
			// "No se pudo obtener la definición de conexión de TeamSpeak para la dependencia en la linea: "
			// + strLine);
			// }
			// }
			if ((data.length > 0) && (data[0] != null)
					&& (data[0].length() > 0)) {
				// El formato interno de la frenquencia lleva el separador de .
				// pero en el fichero no lo lleva
				if (!data[0].contains("."))
					data[0] = data[0].substring(0, 3) + "."
							+ data[0].substring(3, 5);
				cm.setFrequency(data[0]);
			}
			if ((data.length > 1) && (data[1] != null)
					&& (data[1].length() > 0)) {
				cm.setDependency(data[1]);
			}

			// El canal de TS se corresponde con el nombre de la dependencia
			cm.setChannel(data[1]);

			if ((data.length > 2) && (data[2] != null)
					&& (data[2].length() > 0)) {
				cm.setLatitude(Double.parseDouble(data[2]));
			}

			if ((data.length > 3) && (data[3] != null)
					&& (data[3].length() > 0)) {
				cm.setLongitude(Double.parseDouble(data[3]));
			}

			l.add(cm);
		}
		// Close the input stream
		in.close();

		return l;
	}
}

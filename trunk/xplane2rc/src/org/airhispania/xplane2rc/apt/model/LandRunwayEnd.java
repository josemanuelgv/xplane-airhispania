/**
 * 
 */
package org.airhispania.xplane2rc.apt.model;

import org.airhispania.xplane2rc.util.Point;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         AIRPORT DATA (APT.DAT) FILE SPECIFICATION VERSION 1000
 */
public class LandRunwayEnd {

	/**
	 * Runway number (eg. “31R”, “02”). Leading zeros are required.
	 */
	private String number;

	/**
	 * Latitude of runway end (on runway centerline) in decimal degrees Eight
	 * decimal places supported
	 */
	private Double latitude;

	/**
	 * Longitude of runway end (on runway centerline) in decimal degrees Eight
	 * decimal places supported
	 */
	private Double longitude;

	/**
	 * Length of displaced threshold in metres
	 */
	private Double displacedThreshold;

	/**
	 * @return the number
	 */
	public String getNumber() {
		return number;
	}

	/**
	 * @param number
	 *            the number to set
	 */
	public void setNumber(String number) {
		this.number = number;
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

	public Point getLocation() {
		return new Point(this.latitude, this.longitude);
	}

	/**
	 * @return the displacedThreshold
	 */
	public Double getDisplacedThreshold() {
		return displacedThreshold;
	}

	/**
	 * @param displacedThreshold
	 *            the displacedThreshold to set
	 */
	public void setDisplacedThreshold(Double displacedThreshold) {
		this.displacedThreshold = displacedThreshold;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "   " + number + " on [" + latitude + ", " + longitude + "]";
	}

}

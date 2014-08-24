/**
 * 
 */
package org.airhispania.xplane2rc.rc4.model;

import org.airhispania.xplane2rc.util.Point;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         Radar Contact runway end properties
 */
public class RunwayEnd {

	private String number;
	private Point startLocation;
	private String elevation;
	private String bearing;
	private String length;
	private String ilsloc;
	private String width;
	private String unk0;
	private Point endLocation;
	private String unk1;

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
	 * @return the startLocation
	 */
	public Point getStartLocation() {
		return startLocation;
	}

	/**
	 * @param startLocation
	 *            the startLocation to set
	 */
	public void setStartLocation(Point startLocation) {
		this.startLocation = startLocation;
	}

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
	 * @return the bearing
	 */
	public String getBearing() {
		return bearing;
	}

	/**
	 * @param bearing
	 *            the bearing to set
	 */
	public void setBearing(String bearing) {
		this.bearing = bearing;
	}

	/**
	 * @return the length
	 */
	public String getLength() {
		return length;
	}

	/**
	 * @param length
	 *            the length to set
	 */
	public void setLength(String length) {
		this.length = length;
	}

	/**
	 * @return the ilsloc
	 */
	public String getIlsloc() {
		return ilsloc;
	}

	/**
	 * @param ilsloc
	 *            the ilsloc to set
	 */
	public void setIlsloc(String ilsloc) {
		this.ilsloc = ilsloc;
	}

	/**
	 * @return the width
	 */
	public String getWidth() {
		return width;
	}

	/**
	 * @param width
	 *            the width to set
	 */
	public void setWidth(String width) {
		this.width = width;
	}

	/**
	 * @return the unk0
	 */
	public String getUnk0() {
		return unk0;
	}

	/**
	 * @param unk0
	 *            the unk0 to set
	 */
	public void setUnk0(String unk0) {
		this.unk0 = unk0;
	}

	/**
	 * @return the endLocation
	 */
	public Point getEndLocation() {
		return endLocation;
	}

	/**
	 * @param endLocation
	 *            the endLocation to set
	 */
	public void setEndLocation(Point endLocation) {
		this.endLocation = endLocation;
	}

	/**
	 * @return the unk1
	 */
	public String getUnk1() {
		return unk1;
	}

	/**
	 * @param unk1
	 *            the unk1 to set
	 */
	public void setUnk1(String unk1) {
		this.unk1 = unk1;
	}

	/**
	 * Converts to number of runway end to the format of XPlane Apt.dat database
	 * 
	 * @return
	 */
	public String getNumberAsApt() {
		String n = this.number;

		if (n.length() == 4)
			n = n.substring(1);

		int firstDigit = Integer.parseInt(n.substring(0, 1));
		if (firstDigit > 0) {
			// Three digits with L or R
			if (n.charAt(2) == '0')
				n = n.substring(0, 2);
			else if (n.charAt(2) == '1')
				n = n.substring(0, 2) + 'L';
			else if (n.charAt(2) == '2')
				n = n.substring(0, 2) + 'R';
		} else {
			n = n.substring(0, 2); // Two digits
		}

		return n;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return number + "," + startLocation + "," + elevation + "," + bearing
				+ "," + length + "," + ilsloc + "," + width + "," + unk0 + ","
				+ endLocation + "," + unk1;
	}

	public RunwayEnd clone() {
		RunwayEnd r = new RunwayEnd();
		r.setBearing(this.getBearing());
		r.setElevation(this.getElevation());
		r.setEndLocation(new Point(this.getEndLocation().getLatitude(), this
				.getEndLocation().getLongitude()));
		r.setStartLocation(new Point(this.getStartLocation().getLatitude(),
				this.getStartLocation().getLongitude()));
		r.setIlsloc(this.getIlsloc());
		r.setLength(this.getLength());
		r.setNumber(this.getNumber());
		r.setUnk0(this.getUnk0());
		r.setUnk1(this.getUnk1());
		r.setWidth(this.getWidth());
		return r;
	}
}

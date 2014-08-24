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
public class LandRunway {

	public static int APT_CODE = 100;

	/**
	 * Width of runway in metres. Two decimal places recommended. Must be >=
	 * 1.00
	 */
	private Float width;

	/**
	 * Code defining the surface type (concrete, asphalt, etc). Integer value
	 * for a Surface Type Code (see below)
	 */
	private char surface_type;

	private List<LandRunwayEnd> ends;

	/**
	 * @return the ends
	 */
	public List<LandRunwayEnd> getEnds() {
		return ends;
	}

	/**
	 * @param ends
	 *            the ends to set
	 */
	public void setEnds(List<LandRunwayEnd> ends) {
		this.ends = ends;
	}

	/**
	 * @return the width
	 */
	public Float getWidth() {
		return width;
	}

	/**
	 * @param width
	 *            the width to set
	 */
	public void setWidth(Float width) {
		this.width = width;
	}

	/**
	 * @return the surface_type
	 */
	public char getSurface_type() {
		return surface_type;
	}

	/**
	 * @param surface_type
	 *            the surface_type to set
	 */
	public void setSurface_type(char surface_type) {
		this.surface_type = surface_type;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		String r = " RUNWAY ";
		r = r + width + "m : ";
		for (LandRunwayEnd e : this.getEnds()) {
			r = r + e.toString() + " ";
		}

		return r;
	}

}

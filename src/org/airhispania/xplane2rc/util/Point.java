/**
 * 
 */
package org.airhispania.xplane2rc.util;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class Point extends com.grum.geocalc.Point {

	/**
	 * 
	 */
	public Point(String lat, String lon) {
		super(new com.grum.geocalc.DegreeCoordinate(Double.parseDouble(lat)),
				new com.grum.geocalc.DegreeCoordinate(Double.parseDouble(lon)));
	}

	public Point(double lat, double lon) {
		super(new com.grum.geocalc.DegreeCoordinate(lat),
				new com.grum.geocalc.DegreeCoordinate(lon));
	}

}

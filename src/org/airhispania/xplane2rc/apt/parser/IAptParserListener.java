/**
 * 
 */
package org.airhispania.xplane2rc.apt.parser;

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 *         Used for been notified when the parser parses elements from input
 *         file
 */
public interface IAptParserListener {

	public void landAirportParsed();

	public void runwayParsed();

	public void runwayEndParsed();

}

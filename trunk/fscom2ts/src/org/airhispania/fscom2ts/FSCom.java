package org.airhispania.fscom2ts;
import com.flightsim.fsuipc.FSNavRadio;

/**
 * 
 */

/**
 * @author Jose Manuel García Valladolid - josemanuelgv@gmail.com
 * 
 */
public class FSCom extends FSNavRadio {

	public FSCom() {
		super();
		iStandbyFreq = 0x311a;
		iFreq = 0x34e;
		iID = 0x3000;
		iName = 0x3006;
		iSwap = 0x3123;
		iLocNeedle = 0x0c48;
		iGlideSlope = 0x0c49;
	}

}
